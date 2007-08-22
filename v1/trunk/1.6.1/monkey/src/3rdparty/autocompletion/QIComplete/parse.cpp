/********************************************************************************************************
 * PROGRAM	  : 
 * DATE - TIME  : lundi 10 avril 2006 - 22:28
 * AUTHOR	   : IComplete Team and Anacr0x ( fred.julian at gmail.com )
 * FILENAME	 : 
 * LICENSE	  : GPL
 * COMMENTARY   : Modified file (with qt) of the icomplete project
 ********************************************************************************************************/

#include <QRegExp>
#include <QFile>
#include <QProcess>

#include "parse.h"
#include "tree.h"

char* Parse::scanForIdent(const char **expr)
{
	static char ident[256];
	int valid_chars = 0;	/* number of identified characters in the ident string */

	char c;
	while ((c = **expr) != '\0')
	{
		if (valid_chars == 0 && isspace(c))
		{
			(*expr)++;
			continue;
		}
		// XXX: Namespace support
		else if (isalpha(c) || c == '_')	// ???: || c == ':')
		{
			ident[valid_chars] = c;
			if (++valid_chars == 255)
			{
				ident[255] = '\0';
				return ident;
			}
		}
		/* a digit may be part of an ident but not from the start */
		else if (isdigit(c))
		{
			if (valid_chars)
			{
				ident[valid_chars] = c;
				if (++valid_chars == 255)
				{
					ident[255] = '\0';
					return ident;
				}
			}
			else
				return NULL;

		}
		else
			break;

		(*expr)++;
	}

	if (valid_chars)
	{
		ident[valid_chars] = '\0';
		return ident;
	}
	else
		return NULL;
}

bool Parse::scanForFuncdef(const QString &expr)
{
	const char *pExpr = expr.toAscii();
	char c;
	while ((c = *pExpr) != '\0')
	{
		switch (c)
		{
		case ' ':
		case '\t':
		case '\n':
			pExpr++;
			continue;

		case '(':
			return true;
		default:
			return false;
		}
	}
	return false;
}

QString Parse::getTypeOfToken(const QString &ident, const QString &className,
                              Scope * scope, bool token_is_function)
{
	/* if we have a variable and already found a local definition, just return it after duplicating */
	if (!token_is_function && scope->localdef.length())
		return scope->localdef;

	/* if the identifier is this-> return the current class */
	if (ident == "this")
		return scope->scope;

	Tree *tree = NULL;
	if (className.length())
	{
		tree = Tree::buildInheritanceTree(className);
		if (!tree)
			return NULL;
	}

	tagFileInfo info;
	tagEntry entry;
	tagFile *tfile = tagsOpen(tagsFilePath.toAscii(), &info);
	if (tfile && info.status.opened)
	{
		if (tagsFind(tfile, &entry, ident.toAscii(), TAG_OBSERVECASE | TAG_FULLMATCH) ==
		        TagSuccess)
		{
			do
			{
				if (tree && !tree->isMemberOfScope(&entry, scope))
					continue;

				const char *kind = tagsField(&entry, "kind");
				if (token_is_function)	/* only list if tag is a function */
				{
					if (!kind
					        || (strcmp(kind, "function") && strcmp(kind, "prototype")))
						continue;
				}
				else		/* or a variable */
				{
					//brc: add externvar for extern variables like cout
					if (!kind
					        || (strcmp(kind, "variable") && strcmp(kind, "externvar")
					            //brc: namespace workarround: add namespace
					            && strcmp(kind, "namespace") && strcmp(kind, "member")))
						continue;
				}

				/* need to duplicate the pattern, don't ask me why */
				QString type = extractTypeQualifier(entry.address.pattern, ident);
				if(tree)
					tree->freeTree();
				tagsClose(tfile);
				return type;
			}
			while (tagsFindNext(tfile, &entry) == TagSuccess);
		}
		tagsClose(tfile);
	}
	return NULL;
}

QString Parse::extractTypeQualifier(const QString &str, const QString &expr)
{
	/*
	 * this regexp catches things like:
	 * a) std::vector<char*> exp1[124] [12], *exp2, expr;
	 * b) QclassName* expr1, expr2, expr;
	 * c) int a,b; char r[12] = "test, argument", q[2] = { 't', 'e' }, expr;
	 *
	 * it CAN be fooled, if you really want it, but it should
	 * work for 99% of all situations.
	 *
	 * QString
	 *              var;
	 * in 2 lines does not work, because //-comments would often bring wrong results
	 */

#define STRING      "\".*\""
#define BRACKETEXPR "\\{.*\\}"
#define IDENT       "[a-zA-Z_][a-zA-Z0-9_]*"
#define WS          "[ \t\r\n]*"
#define PTR         "[\\*&]?\\*?"
#define INITIALIZER "=(" WS IDENT WS ")|=(" WS STRING WS ")|=(" WS BRACKETEXPR WS ")"
#define CONSTRUCTOR "(\\(" WS IDENT WS "\\))|(\\(" WS STRING WS "\\))"
#define ARRAY       WS "\\[" WS "[0-9]*" WS "\\]" WS

	QString pattern = "(" IDENT ")"				// the 'std' in example a)
	                  "(::" IDENT ")*"			// ::vector
	                  "(" WS "<[^>;]*>)?[ \t\n*&]{1}"	// <char *>
	                  "(" WS PTR WS IDENT WS "(" ARRAY ")*" WS "((" INITIALIZER ")?|(" CONSTRUCTOR ")?)" WS "," WS ")*" // other variables for the same ident (string i,j,k;)
	                  "(" WS "[*&])?";			// check again for pointer/reference type

	QRegExp rx(pattern + WS + expr + "[^.-]");
	QString match;

	int pos = 0;
	while ((pos = rx.indexIn(str, pos)) != -1)
	{
		match = rx.cap(1);
		pos += rx.matchedLength();
	}

	return match;
}

bool Parse::getTypeOfExpression(const QString &expr, Expression * exp, Scope * scope)
{
	char *ident = NULL;

	/* very basic stack implementation to keep track of tokens */
	const int max_items = 20;
	const char *stack[max_items];
	int num_stack = 0;

	/* skip nested brackets */
	int brackets = 0, square_brackets = 0;

	bool in_ident = false;	/* if the current position is within an identifier */
	bool extract_ident = false;	/* if we extract the next string which looks like an identifier - only after: . -> and ( */

	int len = expr.length();
	if (!len)
		return false;

	const char * first = expr.toAscii(),
	                     *start = first + len;

	while (--start >= first)
	{
		/* skip brackets */
		if (brackets > 0 || square_brackets > 0)
		{
			if (*start == '(')
				--brackets;
			else if (*start == ')')
				++brackets;
			else if (*start == '[')
				--square_brackets;
			else if (*start == ']')
				++square_brackets;
			continue;
		}
		/* identifier */
		if (isdigit(*start))
			in_ident = false;
		else if (isalpha(*start) || *start == '_')
			in_ident = true;
		else
		{
			switch (*start)
			{
				/* skip whitespace */
			case ' ':
			case '\t':
				if (in_ident)
					goto extract;
				else
				{
					in_ident = false;
					continue;
				}

				/* continue searching to the left, if we
				 * have a . or -> accessor */
			case '.':
				if (in_ident && extract_ident)
				{
					const char *ident = start + 1;
					if (num_stack < max_items)
						stack[num_stack++] = ident;
					else
						return false;
				}
				in_ident = false;
				extract_ident = true;
				continue;
			case '>':		/* pointer access */
			case ':':		/* static access */
				if ((*start == '>' && (start - 1 >= first && *(start - 1) == '-'))
				        || (*start == ':'
				            && (start - 1 >= first && *(start - 1) == ':')))
				{
					
					if (in_ident && extract_ident)
					{
						const char *ident = start + 1;
						if (num_stack < max_items)
							stack[num_stack++] = ident;
						else
							return false;
					}
					in_ident = false;
					extract_ident = true;
					--start;
					continue;
				}
				else
				{
					start++;
					goto extract;
				}
			case '(':		/* start of a function */
				if (extract_ident)
				{
					start++;
					goto extract;
				}
				else
				{
					extract_ident = true;
					in_ident = false;
					break;
				}

			case ')':
				if (in_ident)	/* probably a cast - (const char*)str */
				{
					start++;
					goto extract;
				}
				brackets++;
				break;

			case ']':
				if (in_ident)
				{
					start++;
					goto extract;
				}
				square_brackets++;
				break;

			default:
				start++;
				goto extract;
			}
		}
	}

extract:
		
	/* ident is the leftmost token, stack[*] the ones to the right
	 * Example: str.left(2,5).toUpper()
	 *          ^^^ ^^^^      ^^^^^^^
	 *        ident stack[1]  stack[0]
	 */
	ident = scanForIdent(&start);
	if (!ident)
		return false;

	/* we need this little kludge with old_type to avoid mem leaks */
	QString type, old_type;

	/* for static access, parsing is done, just return the identifier */
	if (exp->access != AccessStatic)
	{
		QString saved_start = start;	// start can change, so we save it
		getScopeAndLocals(scope, expr, ident);

		/* if we have the start of a function/method, don't return the type
		 * of this function, but className, which it is member of */
		type = getTypeOfToken(ident, NULL, scope, scanForFuncdef(saved_start));

		/* members can't be local variables */
		scope->localdef[0] = '\0';
		while (type!="" && num_stack > 0)
		{
			ident = scanForIdent(&stack[--num_stack]);
			old_type = type;
			//<brc code>: namespace workaround: if the "type" of an identifier is namespace ignore it
			if (type == "namespace")
				old_type = "";
			//<\end brc code>
			type = getTypeOfToken(ident, old_type.toAscii(), scope,
			                      scanForFuncdef(stack[num_stack]));
		}
	}
	else			/* static member */
		type = ident;
	
	/* copy result into passed Expression argument */
	if (type!="" && !(exp->access == AccessInFunction))
	{
		exp->className = type;
		exp->function[0] = '\0';
		return true;
	}
	else if (exp->access == AccessInFunction)
	{
		exp->className = old_type;
		exp->function = ident;
		return true;
	}
	return false;
}

Expression Parse::parseExpression(QString expression, Scope * scope, bool showAllResults)
{
	Expression exp;
	exp.access = ParseError;

	int len = expression.length();
	if (len < 1)
		return exp;

	// we found that the user has beginning to write
	if(!showAllResults)
	{
		const char *start = expression.toAscii(), *p = start;
		while (*p)
			p++;
		p--;
		while (p != start && *p != '.' && *p != '\n' && *p != '\r' && *p != ' ' && *p != '\t' && *p != ';' && *p != '{' && *p != '}' && *p != '(' && *p != ',' && (p-1) != start && (*p != '>' || *(p - 1) != '-') && (*p != ':' || *(p - 1) != ':'))
			p--;
		exp.writing = p+1;
	}

	/* A global function should be completed, if nothing else is found */
	exp.access = AccessGlobal;

	bool allow_AccessInFunction = true;	// false, when other characters than whitespace were typed
	/* search for the type of the correct completion */
	while (--len >= 0)
	{
		char last = ((const char*)expression.toAscii())[len];
		switch (last)
		{
		case ' ':
		case '\t':
			/* skip initial spaces */
			if (exp.access == AccessGlobal)
			{
				/*                    exp.access = ParseError;*/
				continue;
			}
			else
			{
				exp.access = AccessGlobal;
				goto extract;
			}

		case '>':
			if (len && expression[len - 1] == '-')
			{
				exp.access = AccessPointer;
				expression[len + 1] = '\0';	/* truncate the string */
				goto extract;
			}
			else
			{
				exp.access = AccessGlobal;	/* XXX: AccessError ? */
				goto extract;
			}
		case '.':
			exp.access = AccessMembers;
			expression[len + 1] = '\0';	/* truncate the string */
			goto extract;
		case ':':
			if (len && expression[len - 1] == ':')
			{
				exp.access = AccessStatic;
				expression[len + 1] = '\0';	/* truncate the string */
				goto extract;
			}
			else
			{
				exp.access = AccessGlobal;
				goto extract;
			}
		case '(':
			if (allow_AccessInFunction)
			{
				exp.access = AccessInFunction;
				expression[len + 1] = '\0';	/* truncate the string */
			}
			goto extract;

		default:
			if (isalpha(last) || last == '_')
			{
				/* we only list function definitions if after the opening
				 * ( bracket of the function is no identifier */
				allow_AccessInFunction = false;
				break;
			}
			else
				goto extract;
		}
	}

extract:
	/* now extract the type out of the string */
	if (exp.access == AccessMembers || exp.access == AccessPointer
	        || exp.access == AccessStatic || exp.access == AccessInFunction)
	{
		if (!getTypeOfExpression(expression.toAscii(), &exp, scope))
			exp.access = ParseError;
	}
	else if (exp.access == AccessGlobal)
		getScopeAndLocals(scope, expression.toAscii(), NULL);

	return exp;
}

Tag Parse::prettifyTag(const tagEntry * entry)
{
	const char *signature = tagsField(entry, "signature");
	const char *kind = tagsField(entry, "kind");
	const char *access = tagsField(entry, "access");

	Tag tag;
	tag.isFunction = signature;
	tag.name = entry->name;

	char *p = (char *) entry->address.pattern;

	/* for a macro the pattern is already parsed */
	if (!strcmp(kind, "macro"))
	{
		/* NOTE: exuberant-ctags 5.5.4 does not provide a valid pattern for found macros
		 * work around it, by getting the line myself */
		char pat_macro[512];
		unsigned long line = entry->address.lineNumber;
		if (line == 0)		/* sometimes ctags can't find the correct line */
			return tag;

		FILE *fileMacro = fopen(entry->file, "r");
		if (fileMacro)
		{
			while ((p = fgets(pat_macro, 512, fileMacro)) != NULL)
			{
				line--;
				if (line <= 0)
				{
					/* remove leading spaces */
					p++;	/* skip over # - it is added later again */
					while (*p == ' ' || *p == '\t')
						p++;

					tag.longName = '#' + p;
					/* remove new line at the end */
					tag.longName.remove(tag.longName.length() - 1, 1);
					break;
				}
			}
			tag.longName += " [macro]";
			fclose(fileMacro);
			return tag;
		}
	}
	/* special handling for enumerator */
	if (!strcmp(kind, "enumerator"))
	{
		/* skip whitespace from variable/function patterns */
		size_t skip = strspn(p, "/^ \t");
		p += skip;
		/* remove trailing $/ characters */
		char *pos = NULL;
		if ((pos = strstr(p, "$/")) != NULL)
			*pos = '\0';
		/* replace \/\/ and \/ * *\/ to correctly show comments */
		while ((pos = strstr(p, "\\/\\/")) != NULL)
			memcpy(pos, "  //", 4);
		while ((pos = strstr(p, "\\/*")) != NULL)
			memcpy(pos, " /*", 3);
		while ((pos = strstr(p, "*\\/")) != NULL)
			memcpy(pos, " */", 3);

		tag.longName += QString(p) + " [enumerator]";
		return tag;
	}

	size_t skip = strspn(p, "/^ \t");
	p += skip;
	/* remove trailing $/ characters */
	char *pos = NULL;
	if ((pos = strstr(p, "$/")) != NULL)
		*pos = '\0';

	tag.longName += p;

	/* if it is a function, add signature as well */
	if (signature)
		tag.parameters += signature;

	if (access && (!strcmp(access, "private") || !strcmp(access, "protected")))
		tag.longName += '[' + access + ']';

	return tag;
}


bool Parse::getScopeAndLocals(Scope * sc, const QString &expr, const QString &ident)
{
	// initialize scope if nothing better is found
	sc->scope = "";
	sc->localdef = "";

	/* create a tags file for `expr' with function names only.
	 * The function with the highest line number is our valid scope
	 * --sort=no, because tags generation probably faster, and
	 * so I just have to look for the last entry to find 'my' tag
	 */

	QString command =
	    ctagsCmdPath +
	    " --language-force=c++ --sort=no --fields=fKmnsz --c++-kinds=fn -o \"" +
	    smallTagsFilePath + "\" \"" + parsedFilePath + '\"';

	// I don't process any user input, so system() should be safe enough
	QProcess ctags;
	ctags.execute(command);
	QFile::remove (parsedFilePath);
	if (ctags.exitStatus() == QProcess::CrashExit)
		return false;

	/* find the last entry, this is our current scope */
	tagFileInfo info;
	tagFile *tfile = tagsOpen(smallTagsFilePath.toAscii(), &info);
	tagEntry entry;
	const char *scope = NULL;
	if (tfile && info.status.opened)
	{
		if (tagsFirst(tfile, &entry) == TagSuccess)
		{
			do
				scope = tagsField(&entry, "class");
			while (tagsNext(tfile, &entry) == TagSuccess);
		}
		tagsClose(tfile);
	}

	/* must be before the 'type = extract_type_qualifier()' code, which modifies scope */
	if (scope)
		sc->scope = scope;

	/* get local definition (if any) */
	if (ident!="")
	{
		QString type = extractTypeQualifier(expr, ident);
		if (type.length())
		{
			sc->localdef = type;
		}
		else
			sc->localdef = "";
	}

	QFile::remove (smallTagsFilePath);
	
	return true;
}
