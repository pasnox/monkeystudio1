/********************************************************************************************************
 * PROGRAM	  : 
 * DATE - TIME  : lundi 10 avril 2006 - 22:28
 * AUTHOR	   : Anacr0x ( fred.julian at gmail.com )
 * FILENAME	 : InitCompletion.h
 * LICENSE	  : GPL
 * COMMENTARY   : Initialisation class for the icomplete's autocompletion
 ********************************************************************************************************/
#include "InitCompletion.h"
#include "./QIComplete/parse.h"
#include "./QIComplete/readtags.h"
#include "./QIComplete/tree.h"

#include <QDir>
#include <QProcess>

#ifdef _WIN32
#define NEW_LINE "\r\n"
#else
#define NEW_LINE "\n"
#endif

void InitCompletion::setTempFilePath (const QString &Path)
{
	tagsIncludesPath = Path + '/' + "tags_includes";
	tagsFilePath = Path + '/' + "tags";
	smallTagsFilePath = Path + '/' + "small-tags";
	parsedFilePath = Path + '/' + "parsed_file";
}

void InitCompletion::addIncludes (QStringList includesPath)
{
	QDir dir;
	QFileInfoList list;

	for(int i = 0; i < includesPath.size(); i++)
	{
		dir.setPath(includesPath[i]);
		if (dir.exists() && !cpp_includes.contains(includesPath[i]))
			cpp_includes << includesPath[i];
		
		list = dir.entryInfoList(QDir::Dirs | QDir::Readable | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Hidden);
		for (int j = 0; j < list.size(); ++j)
			includesPath.insert(i+1, list[j].absoluteFilePath());
	}
}

QStringList InitCompletion::includesList(const QString &parsedText)
{
	QStringList list;
	
	/* find include'ed files */
	QRegExp rx("#\\s*include\\s*(<[^>]+>|\"[^\"]+\")");
	QString include;

	int pos = 0;
	while ((pos = rx.indexIn(parsedText, pos)) != -1)
	{
		include = rx.cap(1);

		include.remove(0, 1);
		include.remove(include.length() - 1, 1);
		list << include;

		pos += rx.matchedLength();
	}
	
	return list;
}

QString InitCompletion::includesPathList(const QString &parsedText)
{
	QString list, fullpath, buf;
	QStringList includes = includesList(parsedText);
	while (!includes.empty())
	{
		QFile *temp_fd = getFiledescriptor(includes.first(), fullpath);
		includes.removeFirst();
		if (temp_fd)
		{
			if(!list.contains(fullpath))
			{
				list += fullpath + NEW_LINE;
				buf = temp_fd->readAll();
				includes << includesList(buf);
			}
			
			temp_fd->close();
			delete temp_fd;
		}
	}
	
	return list;
}

/* creates a simple hash for all #include lines of a line */
long InitCompletion::calculateHash(const QString &parsedText)
{
	long res = 0;

	QStringList includes = includesList(parsedText);
	QString s;
	foreach (s, includes)
	{
		for (int i = 0; i < s.length(); i++)
			res += (i + 1) * s[i].toAscii();
	}
		
	return res;
}

/* forks and executes ctags to rebuild a tags file
 * storing cache_value in the tags file */
bool InitCompletion::buildTagsFile(long cache_value, const QString &parsedText)
{
	QString pathList = includesPathList(parsedText);

	QFile includesListFile(tagsIncludesPath);
	if (includesListFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		includesListFile.write (pathList.toLocal8Bit());
		includesListFile.close();

		/* save the cache information in the tags file */
		QFile tags(tagsFilePath);
		if (tags.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QString head = "!_TAG_FILE_FORMAT	2	/extended format; --format=1 will not append ;\" to lines/" NEW_LINE
					"!_TAG_FILE_SORTED	1	/0=unsorted, 1=sorted, 2=foldcase/" NEW_LINE
					"!_TAG_PROGRAM_AUTHOR	Darren Hiebert	/dhiebert@users.sourceforge.net/" NEW_LINE
					"!_TAG_PROGRAM_NAME	Exuberant Ctags	//" NEW_LINE
					"!_TAG_PROGRAM_URL	http://ctags.sourceforge.net	/official site/" NEW_LINE
					"!_TAG_PROGRAM_VERSION	5.5.4	//" NEW_LINE
					"!_TAG_CACHE\t" + QString::number(cache_value) + NEW_LINE;
			tags.write(head.toLocal8Bit());
			tags.close();
		}

		QString command = ctagsCmdPath + " -f \"" + tagsFilePath +
				"\" --append --language-force=c++ --fields=afiKmsSzn --c++-kinds=cdefgmnpstuvx -L \""
				+ tagsIncludesPath +'\"';

		// I don't process any user input, so system() should be safe enough
		QProcess ctags;
		ctags.execute(command);
		
		includesListFile.remove();
		
		if (ctags.exitStatus() == QProcess::NormalExit)
			return true;
	}
	return false;
}

Expression InitCompletion::getExpression(const QString &text, Scope &sc, bool showAllResults)
{
	Tree::parent = this;
	
	long cache_value = calculateHash(text);

	/* automatic cache mode */
	/* we save a hash value for all included files from the current file
	in the tags file, if it matches the hash of the current buffer, nothing
	has changed, and reuse the existing tags file */

	bool build_cache = true;
	QFile fCache(tagsFilePath);
	if(fCache.open(QIODevice::ReadOnly))
	{
		QString pattern = "!_TAG_CACHE\t" + QString::number(cache_value) + NEW_LINE;
		for (int i=0; i<10; i++)
			if (fCache.readLine() == pattern)
				build_cache = false;

		fCache.close();
	}
	if (build_cache)
		buildTagsFile(cache_value, text);

	/* We create a file with the parsed text */
	QFile f(parsedFilePath);
	f.open(QIODevice::WriteOnly | QIODevice::Text);
	f.write (text.toLocal8Bit());
	f.close();
	
	/* real parsing starts here */
	Parse parse(ctagsCmdPath, tagsFilePath, parsedFilePath, smallTagsFilePath);
	return parse.parseExpression(text, &sc, showAllResults);
}

QList<Tag> InitCompletion::startParse(const QString &text, bool showAllResults)
{
	Scope sc;
	Expression exp = getExpression(text, sc, showAllResults);
	
	/* we have all relevant information, so just list the entries */
	if (exp.access == ParseError)
		return QList <Tag> ();

	return Tree::findEntries(&exp, &sc);
}

QString InitCompletion::className(const QString &text)
{
	Scope sc;
	Expression exp = getExpression(text, sc);
	if (exp.access == ParseError)
		return QString();

	return exp.className;
}

void InitCompletion::setCtagsCmdPath (const QString &cmdPath)
{
	if (cmdPath.indexOf(' ')!=-1)
		ctagsCmdPath = QString('\"') + cmdPath + '\"';
	else
		ctagsCmdPath = cmdPath;
}

QFile* InitCompletion::getFiledescriptor(const QString &filename, QString &fullname)
{
	QFile *fd = new QFile();

	/* absolute path name */
	if (QFileInfo(filename).isAbsolute())
	{
		fd->setFileName(filename);
		if(fd->open(QIODevice::ReadOnly))
		{
			fullname = QFileInfo(filename).canonicalFilePath();
			return fd;
		}
	}
	else
	{
		/* relative pathname */
		for (int i = 0; i < cpp_includes.size(); i++)
		{
			fd->setFileName(cpp_includes.at(i) + '/' + filename);
			if(fd->open(QIODevice::ReadOnly))
			{
				fullname = QFileInfo(cpp_includes.at(i) + '/' + filename).canonicalFilePath();
				return fd;
			}
		}
	}

	// Nothing was found
	fd->close();
	delete fd;
	return NULL;
}
