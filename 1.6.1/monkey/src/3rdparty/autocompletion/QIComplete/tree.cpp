/********************************************************************************************************
 * PROGRAM	  : 
 * DATE - TIME  : lundi 10 avril 2006 - 22:28
 * AUTHOR	   : IComplete Team and Anacr0x ( fred.julian at gmail.com )
 * FILENAME	 : 
 * LICENSE	  : GPL
 * COMMENTARY   : Modified file (with qt) of the icomplete project
 ********************************************************************************************************/
#include <QRegExp>

#include "tree.h"
#include "parse.h"
#include "readtags.h"

InitCompletion* Tree::parent = NULL;

//brc: new subroutine
void Tree::addTreeChild(const QString &className)
{
	/* check this class if not already in the tree to
	 * avoid a hangup with a broken (=cirular) tags file */
	if (findInTree(className) == NOT_FOUND)
	{
		Tree *child = Tree::buildInheritanceTree(className);
		inherits << child;
	}
}

Tree* Tree::buildInheritanceTree(const QString &className)
{
	Tree *root = new Tree;
	root->name = className;

	tagEntry entry;
	tagFileInfo info;
	tagFile *tfile = tagsOpen(parent->tagsFilePath.toAscii(), &info);

	//brc: split namespace part and classname
	char nbuffer[256];
	char *nspace = NULL;
	strncpy(nbuffer, className.toAscii(), 256);
	nbuffer[255] = '\0';
	char *cname = nbuffer;
	char *p = nbuffer;
	char *np = NULL;

	while ((np = strstr(p, "::")))
		p = np + 2;
	if (p != nbuffer)
	{
		*(p - 2) = '\0';
		nspace = nbuffer;
		cname = p;
	}

	Tree *tree = root;

	//brc: namespace workaround: if no namespace is present, try to find the class
	//                           also in std::
	if (!nspace)
	{
		char buffer[512];
		strcpy(buffer, "std::");
		strcat(buffer, cname);
		tree->addTreeChild(buffer);
	}

	int i = 0;			// counters
	if (tfile && info.status.opened)
	{
		if (tagsFind(tfile, &entry, cname, TAG_OBSERVECASE | TAG_FULLMATCH) ==
		        TagSuccess)
		{
			do
			{
				//brc:follow typedefs ...
				if (!strcmp(entry.kind, "typedef") && entry.address.pattern)
				{
					const char *field_namespace = tagsField(&entry, "namespace");
					if ((nspace && field_namespace
					        && !strcmp(field_namespace, nspace)) || !nspace)
					{
						char pattern[256];
						strncpy(pattern, entry.address.pattern, 256);
						pattern[255] = '\0';
						char *p = strstr(pattern, "typedef");
						if (p && strstr(pattern, cname))
						{
							p += strlen("typedef");
							char *p1;
							if ((p1 = strstr(p, "typename")))
								p = p1 + strlen("typename");
							if (strstr(p, "struct"))
								continue;
							if ((p = strtok(p, " \n\t<")))
							{
								tree->freeTree();
								if (nspace)
								{
									char buffer[512];
									strcpy(buffer, nspace);
									strcat(buffer, "::");
									strcat(buffer, p);
									return Tree::buildInheritanceTree(buffer);
								}
								else
								{
									return Tree::buildInheritanceTree(p);
								}
							}
						}
					}
				}
				/* need to get the entry for the class definition */
				if (!strcmp(entry.kind, "class")
				        || !strcmp(entry.kind, "struct") || !strcmp(entry.kind, "union"))
				{
					/* and look for the entry "inherits" to see which are superclassNamees of className */
					for (; i < entry.fields.count; i++)
					{
						if (!strcmp(entry.fields.list[i].key, "inherits"))
						{
							/* split the inherits string to single classNamees for multiple inheritance */
							char *allinherits = strdup(entry.fields.list[i].value);
							char *buffer = NULL;
							char *inherit = strtok(allinherits, ",");
							while (inherit != NULL)
							{
								tree->addTreeChild(inherit);

								/* next token */
								inherit = strtok(buffer, ",");
							}
							free(allinherits);
							goto end;
						}
					}
					// no more superclassNamees
					goto end;
				}
			}
			while (tagsFindNext(tfile, &entry));
		}
end:
		tagsClose(tfile);
	}
	return root;
}

void Tree::freeTree()
{
	for (int i = 0; i < inherits.size(); i++)
		inherits[i]->freeTree();
}

Match Tree::findInTree(const QString &className)
{
	if ( name == className)
		return DIRECT_HIT;

	for (int i = 0; i < inherits.size(); i++)
		if (inherits[i]->findInTree(className) >= DIRECT_HIT)
			return SUBCLASS;

	return NOT_FOUND;
}

bool Tree::isMemberOfScope(const tagEntry * entry, const Scope * scope)
{
	const char *field_className = tagsField(entry, "class");
	const char *field_struct = tagsField(entry, "struct");
	const char *field_union = tagsField(entry, "union");
	//const char *field_namespace = tagsField(entry, "namespace");
	const char *field_access = tagsField(entry, "access");

	const char *tag_className = NULL;
	if (field_className)
		tag_className = field_className;
	else if (field_struct)
		tag_className = field_struct;
	else if (field_union)
		tag_className = field_union;
	else
		return false;

	/* tag must be part of (inherited) className to show it */
	Match match_tag = findInTree(tag_className);
	if (match_tag == NOT_FOUND)
		return false;
	if (field_union)
		return true;
	if (!field_access)
		return false;
	if (!strcmp(field_access, "public"))
		return true;

	/* if access is not public, current scope must be in one of the
	 * (inherited) classNamees */
	Match match_curscope = findInTree(scope->scope);
	if (!strcmp(field_access, "protected") && match_curscope >= DIRECT_HIT)
		return true;
	else if (!strcmp(field_access, "private") && match_curscope == DIRECT_HIT)
		return true;

	return false;
}

QList<Tag> Tree::findEntries(const Expression * exp, const Scope * scope)
{
	QList < Tag > TagList;

	tagEntry entry;
	tagFileInfo info;
	tagFile *tfile = tagsOpen(parent->tagsFilePath.toAscii(), &info);
	char lasttag[256] = "";	/* store last tag to avoid duplicates */

	/* create an inheritance tree of our class or current scope, depending what we want to complete */
	Tree *tree = NULL;
	if (exp->access & AccessMembers || exp->access & AccessPointer
	        || exp->access & AccessStatic || exp->access & AccessInFunction)
		tree = Tree::buildInheritanceTree(exp->className);
	else if (exp->access & AccessGlobal)
		tree = Tree::buildInheritanceTree(scope->scope);

	if (!tree)
	{
		printf("Couldn't build inheritance tree\n");
		return QList < Tag > ();
	}


	/* parse the tags file */
	if (tfile && info.status.opened)
	{
		/* we can do a binary search for function definitions */
		if (exp->access == AccessInFunction
		        && tagsFind(tfile, &entry, exp->function.toAscii(),
		                    TAG_FULLMATCH | TAG_OBSERVECASE) == TagSuccess)
		{
			do
			{
				bool show_this_tag = false;
				const char *field_kind = tagsField(&entry, "kind");

				if (field_kind
				        && (!strcmp(field_kind, "member")
				            || !strcmp(field_kind, "method")))
					show_this_tag = tree->isMemberOfScope(&entry, scope);
				else if (!strcmp(field_kind, "function")
				         || !strcmp(field_kind, "prototype"))
				{
					if (exp->className == "")	// only a function, not method of a class
						show_this_tag = true;
					else
						show_this_tag = tree->isMemberOfScope(&entry, scope);
				}
				if (!show_this_tag)
					continue;

				/* output the tag if it is valid and no duplicate either to stdout
				 * or to a file if the -o flag was given */
				Tag tag;
				tag = Parse::prettifyTag(&entry);
				if (exp->writing[0] == 0
				        || tag.name.startsWith(exp->writing))
					TagList << tag;
			}
			while (tagsFindNext(tfile, &entry) == TagSuccess);
		}

		// otherwise loop through all tags
		// and filter those which match one of our inherited class names
		else if (tagsFirst(tfile, &entry) == TagSuccess)
		{
			do
			{
				bool show_this_tag = false;
				const char *field_kind = tagsField(&entry, "kind");
				const char *field_className = tagsField(&entry, "class");
				const char *field_struct = tagsField(&entry, "struct");
				const char *field_union = tagsField(&entry, "union");
				const char *field_namespace = tagsField(&entry, "namespace");
				const char *field_access = tagsField(&entry, "access");

				switch (exp->access)
				{
					/*
					 * find MEMBERS of classes/structs
					 */
				case AccessMembers:
				case AccessPointer:
					if (strstr(entry.address.pattern, "static "))
						continue;
					/* Qt 3 defines this, and checking for it does not harm other users */
					if (strstr(entry.address.pattern, "QT_STATIC_CONST "))
						continue;

					// don't show destructors, can't call them directly anyways.
					if (entry.name[0] == '~')
					{
						show_this_tag = false;
						break;
					}

					// TODO: namespace support
					if (field_kind
					        && (!strcmp(field_kind, "member")
					            || !strcmp(field_kind, "function")
					            || !strcmp(field_kind, "prototype")
					            || !strcmp(field_kind, "method")))
						show_this_tag = tree->isMemberOfScope(&entry, scope);

					break;
					/*
					 * find STATIC functions/variables (everything after a ::)
					 */
				case AccessStatic:
					/* when we find a tag, the following conditions must be met, to show the tag:
					 * - must be part of a className or namespace
					 * - must contain "static" on the line
					 * - must be "public"
					 */
					if (field_className)
					{
						if (exp->className == QString(field_className) &&
						        field_access && !strcmp(field_access, "public"))
							/* XXX: Workaround ctags limitation and try to find out
							 * ourselves, if this tag is a static one, if it 
							 * has "static" in the pattern */
							if (strstr(entry.address.pattern, "static ") ||
							        strstr(entry.address.pattern, "QT_STATIC_CONST "))
								show_this_tag = true;
					}
					else if (field_namespace)
						if (exp->className == QString(field_namespace))
							show_this_tag = true;
					break;

					// find GLOBAL functions/variables/defines

				case AccessGlobal:
					// everthing which is not part of a className or struct matches
					if (!field_className && !field_struct && !field_union)
						show_this_tag = true;
					else	// also show locals of current scope
						show_this_tag = tree->isMemberOfScope(&entry, scope);
					break;

				default:
					// things like if() while() for(), etc.
					break;
				}
				/* output the tag if it is valid and no duplicate either to stdout
				 * or to a file if the -o flag was given */
				if (show_this_tag
				        && (exp->access == AccessInFunction
				            || strcmp(entry.name, lasttag)))
				{
					Tag tag;
					if (exp->access & AccessInFunction)
						tag = Parse::prettifyTag(&entry);
					else
					{
						strncpy(lasttag, entry.name, 256);
						tag = Parse::prettifyTag(&entry);
					}

					if (exp->writing[0] == 0
					        || tag.name.startsWith(exp->writing))
						TagList << tag;
				}
			}
			while (tagsNext(tfile, &entry) == TagSuccess);
		}
		tagsClose(tfile);
	}
	else
		return QList < Tag > ();

	tree->freeTree();

	return TagList;
}
