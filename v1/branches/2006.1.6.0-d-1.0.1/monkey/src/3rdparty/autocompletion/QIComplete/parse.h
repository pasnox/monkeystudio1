/********************************************************************************************************
 * PROGRAM	  : 
 * DATE - TIME  : lundi 10 avril 2006 - 22:28
 * AUTHOR	   : IComplete Team and Anacr0x ( fred.julian at gmail.com )
 * FILENAME	 : 
 * LICENSE	  : GPL
 * COMMENTARY   : Modified file (with qt) of the icomplete project
 ********************************************************************************************************/
#ifndef __PARSE_H__
#define __PARSE_H__

#include <QString>
#include <QFile>
#include "readtags.h"
#include "../InitCompletion.h"

typedef enum
{
    /* for now, these options are exclusive
     * but define them as a bitfield for possible
     * future improvements */
    ParseError = 0x00,
    AccessPointer = 0x01,
    AccessMembers = 0x02,
    AccessStatic = 0x04,
    AccessGlobal = 0x08,
    AccessInFunction = 0x10
} Access;


struct Expression
{
	Access access;
	QString className;
	QString function;
	QString writing;
	QString text;
};

struct Scope
{
	QString scope;		// if the cursor is in a function Foo::bar(), store Foo as the scope
	QString localdef;		// stores the type of a local definition or "" if none found
};


class Parse
{
	QString ctagsCmdPath;
	QString tagsFilePath;
	QString parsedFilePath;
	QString smallTagsFilePath;

public:
	Parse (const QString &CtagsCmdPath, const QString &TagsFilePath, const QString &ParsedFilePath, const QString &SmallTagsFilePath ) : ctagsCmdPath(CtagsCmdPath), tagsFilePath(TagsFilePath), parsedFilePath(ParsedFilePath), smallTagsFilePath(SmallTagsFilePath) {};

	/*
	* for a given expr="str" and a string="std::string *str = new std::string("test");"
	* return the type qualifier "std::string"
	*/
	QString extractTypeQualifier(const QString &string, const QString &expr);

	/*
	* For a given expr "var->xy" or "var::" return `var'
	* and the AccessModifier (static, member,...)
	*/
	Expression parseExpression(QString expr, Scope * scope, bool showAllResults = false);

	/* fills the passed *exp pointer with information about the current expression
	* "QWidget w; w.getRect()." will return exp.className=QRect and exp.function=getRect
	*/
	bool getTypeOfExpression(const QString &expr, Expression * exp, Scope * scope);

	/*
	* prettified tags entry look like these:
	*  rect\nQRect r;
	*  setRect()\nvoid setRect (QRect &rect);
	*/
	static Tag prettifyTag(const tagEntry * entry);

	/*
	* extracts the scope at the end of a given code `expr'
	* and save the line of a local variable definition
	* Example:
	*
	* namespace Test {
	*   void MyclassName::function(int var)
	*   {
	* 
	* If expr is the above code and ident is 'var', return:
	* scope = "Test::MyclassName"
	* localdef = "int"
	*/
	bool getScopeAndLocals(Scope * sc, const QString &expr, const QString &ident);

	/*
	* for a given expression "myvar->getX().toFloat"
	* the function should just return "myvar"
	* and move the **expr pointer after the ident
	* returns NULL, if no identifier is found, and then the value of expr is undefined
	*/
	char* scanForIdent(const char **expr);

	/* if the current identfier is the name of a function */
	bool scanForFuncdef(const QString &expr);

	/* for ident=getRect and className=QWidget return QRect
	* @param token_is_function = true, if the token is a function or false if a variable
	* caller must free() the returned string */
	QString getTypeOfToken(const QString &ident, const QString &className, Scope * scope, bool token_is_function);
};

#endif				/* __PARSE_H__ */
