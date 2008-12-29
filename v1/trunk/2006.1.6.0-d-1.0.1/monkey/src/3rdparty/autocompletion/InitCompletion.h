/********************************************************************************************************
 * PROGRAM	  : 
 * DATE - TIME  : lundi 10 avril 2006 - 22:28
 * AUTHOR	   : Anacr0x ( fred.julian at gmail.com )
//  * FILENAME	 : InitCompletion.h
 * LICENSE	  : GPL
 * COMMENTARY   : Initialisation class for the icomplete's autocompletion
 ********************************************************************************************************/
#ifndef _INIT_COMPLETION_H
#define _INIT_COMPLETION_H

#include <QStringList>
#include <QList>
#include <QObject>
#include <QFile>

class Tree;
struct Expression;
struct Scope;

struct Tag
{
	QString name;
	QString parameters;
	QString longName;

	bool isFunction;
};

class InitCompletion : public QObject
{
	Q_OBJECT

public:
	InitCompletion (QObject *parent = 0) : QObject(parent) {};

	void setTempFilePath (const QString &Path);
	void setCtagsCmdPath (const QString &cmdPath);
	void addIncludes (QStringList includesPath);
	QList<Tag> startParse(const QString &text, bool showAllResults = false);
	QString className(const QString &text);

	/*
		* @param: filename is a name like "string.h"
		* @return: the file descriptor (fd) and stores
		*          "/usr/include/string.h" in fullname
	*/
	QFile* getFiledescriptor(const QString &filename, QString &fullname);

	QString tagsFilePath,
		tagsIncludesPath,
		ctagsCmdPath,
		smallTagsFilePath,
		parsedFilePath;

private:
	Expression getExpression(const QString &text, Scope &sc, bool showAllResults = false);
	
	/* creates a simple hash for all #include lines of a line */
	long calculateHash(const QString &ParsedText);
	/* forks and executes ctags to rebuild a tags file
	* storing cache_value in the tags file */
	bool buildTagsFile(long cache_value, const QString &parsedText);
	
	QStringList includesList(const QString &parsedText);
	QString includesPathList(const QString &parsedText);

	QStringList cpp_includes;
};

#endif
