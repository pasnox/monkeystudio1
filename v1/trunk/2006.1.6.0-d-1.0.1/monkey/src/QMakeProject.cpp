/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : jeudi 05 janvier 2006 - 20h05
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : QMakeProject.cpp
 * LICENSE      : GPL
 * COMMENTARY   : This class read/write QT Project (QMake Project) file : .pro
 ********************************************************************************************************/
#include "QMakeProject.h"
#include "Options.h"
#include "Tools.h"
#include "main.h"
#include "InitCompletion.h"
//
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QTreeWidgetItem>
#include <QMessageBox>
//
const QStringList QMakeProject::uiExtension = QStringList() << "ui" << "UI";
const QStringList QMakeProject::headerExtension = QStringList() << "h" << "H" << "hpp" << "HPP";
const QStringList QMakeProject::sourceExtension = QStringList() << "c" << "C" << "cpp" << "CPP";
const QStringList QMakeProject::translationExtension = QStringList() << "ts" << "TS";
const QStringList QMakeProject::textExtension = QStringList() << "txt" << "TXT" << "ini" << "INI" << "cfg" << "CFG";
const QStringList QMakeProject::resourceExtension = QStringList() << "qrc" << "QRC";
const QStringList QMakeProject::rcExtension = QStringList() << "rc" << "RC";
const QStringList QMakeProject::resExtension = QStringList() << "res" << "RES";
const QStringList QMakeProject::defExtension = QStringList() << "def" << "DEF";
//
static QStringList stringKeywords = QStringList() << "TEMPLATE" << "LANGUAGE" << "TARGET" << "DESTDIR" << "VERSION" << "CONFIG" << "QT" << "LIBS" << "DEFINES" << "INCLUDEPATH" << "DEPENDPATH" << "VPATH" << "RESOURCES" << "DEF_FILE" << "RC_FILE" << "RES_FILE" << "AUTHOR" << "EMAIL" << "LICENSE";
static QStringList stringListKeywords = QStringList() << "FORMS" << "HEADERS" << "SOURCES" << "TRANSLATIONS" << "TEXTS" << "SUBDIRS" << "OPENEDFILES" << "DEBUGGER";
//
static const QStringList itemsHeader = QStringList() << QString::null
	<< QObject::tr( "Form Files" )
	<< QObject::tr( "Header Files" )
	<< QObject::tr( "Source Files" )
	<< QObject::tr( "Translation Files" )
	<< QObject::tr( "Text Files" );
//
static const QStringList itemsIcon = QStringList() << ":/Icons/Icons/project.png"
	<< ":/Icons/Icons/form.png"
	<< ":/Icons/Icons/h.png"
	<< ":/Icons/Icons/cpp.png"
	<< ":/Icons/Icons/linguist.png"
	<< ":/Icons/Icons/text.png";
//
QString QMakeProject::qtExtensions()
{
	QString extensions;
	extensions += QString( tr( "UI Files (%1);;" ) ).arg( QString( "*." + uiExtension.join( " *" ).replace( "*", "*." ) ) );
	extensions += QString( tr( "Header Files (%1);;") ).arg( QString( "*." + headerExtension.join( " *" ).replace( "*", "*." ) ) );
	extensions += QString( tr( "Source Files (%1);;") ).arg( QString( "*." + sourceExtension.join( " *" ).replace( "*", "*." ) ) );
	//extensions += QString( tr( "Translation Files (%1);;") ).arg( QString( "*." + translationExtension.join( " *" ).replace( "*", "*." ) ) );
	extensions += QString( tr( "Text Files (%1);;") ).arg( QString( "*." + textExtension.join( " *" ).replace( "*", "*." ) ) );
	extensions += tr( "All Files (*)" );
	return extensions;
}
//
QStringList QMakeProject::getProjectNames( QMakeProject* searchProject, const QString& notThis )
{
	QStringList projects;
	if ( searchProject->stringValue( "TEMPLATE" ).toLower()  != notThis )
		projects << searchProject->name();
	else
		foreach ( QMakeProject* project, searchProject->subProjects() )
			projects << getProjectNames( project, notThis );
	return projects;
}
//
QMakeProject* QMakeProject::getProjectByName( const QString& projectName, QMakeProject* searchProject )
{
	if ( searchProject->name() == projectName )
		return searchProject;
	if ( !searchProject->subProjects().isEmpty() )
	{
		foreach( QMakeProject* project, searchProject->subProjects() )
		{
			if ( ( project = getProjectByName( projectName, project ) ) )
			{
				return project;
				break;
			}
		}
	}
	return 0;
}
//
QMakeProject* QMakeProject::getProjectByFilePath( const QString& projectFilePath, QMakeProject* searchProject )
{
	if ( searchProject->filePath() == projectFilePath )
		return searchProject;
	if ( !searchProject->subProjects().isEmpty() )
	{
		foreach( QMakeProject* project, searchProject->subProjects() )
		{
			if ( ( project = getProjectByFilePath( projectFilePath, project ) ) )
			{
				return project;
				break;
			}
		}
	}
	return 0;
}
//
QMakeProject::QMakeProject( const QString& filePath, QTreeWidget* tree, QObject* parent )
	: QObject( parent ), mFilePath( QFileInfo( filePath ).absoluteFilePath() ),
	mTree( tree ), mOpen( false ), mModified( false ), mLocked( false ), mCompletion( 0 )
{
	mSubProjects.clear();
	stringValues.clear();
	stringListValues.clear();
}
//
QMakeProject::~QMakeProject()
{
	close();
}
// PARSING MEMBERS
QString QMakeProject::parse_part( const QString& part )
{
	QString res;
	bool inName = false;
	QString currName;
	for ( int i = 0; i < (int)part.length(); ++i )
	{
		QChar c = part[i];
		if (!inName)
		{
			if (c != ' ' && c != '\t' && c != '\n' && c != '=' && c != '\\' && c != '+')
				inName = true;
			else
				continue;
		}
		if (inName)
		{
			if (c == '\n')
				break;
			res += c;
		}
	}
	return res;
}
//
QStringList QMakeProject::parse_multiline_part( const QString& contents, const QString& key, int* start )
{
	if (start)
		*start = -1;
	QString lastWord;
	int braceCount = 0;
	for (int i = 0; i < (int)contents.length(); ++i)
	{
		QChar c(contents[i]);
		switch (c.toAscii())
		{
		case '{':
			braceCount++;
			lastWord = "";
			break;
		case '}':
			braceCount--;
			lastWord = "";
			break;
		case ' ': case '\t': case '\\': case '\n':
			lastWord = "";
			break;
		default:
			lastWord += c;
		}
		// ### we should read the 'bla { SOURCES= ... }' stuff as well (braceCount > 0)
		if (lastWord == key && braceCount == 0)
		{
			if (start)
				*start = i -lastWord.length() +1;
			QStringList lst;
			bool inName = false;
			QString currName;
			bool hadEqual = false;
			for (; i < (int)contents.length(); ++i)
			{
				c = contents[i];
				if (!hadEqual && c != '=')
					continue;
				if (!hadEqual)
				{
					hadEqual = true;
					continue;
				}
				if (
					(   c.isLetter() ||
						c.isDigit()  ||
						c == '.'     ||
						c == '/'     ||
						c == '_'     ||
						c == '\\'    ||
						c == '\"'    ||
						c == '\''    ||
						c == '='     ||
						c == '$'     ||
						c == '-'     ||
						c == '('     ||
						c == ')'     ||
						c == ':'     ||
						c == '+'     ||
						c == ','     ||
						c == '~'   ) &&
					c != ' ' &&
					c != '\t' &&
					c != '\n'
				   )
				{
					if (!inName)
						currName = QString::null;
					if (c != '\\' || contents[i+1] != '\n')
					{
						currName += c;
						inName = true;
					}
				}
				else
				{
					if (inName)
					{
						inName = false;
						if (currName.simplified() != "\\")
							lst.append( currName );
					}
					if (c == '\n' && i > 0 && contents[(int)i -1] != '\\')
						break;
				}
			}
			return lst;
		}
	}
	return QStringList();
}
//
void QMakeProject::remove_contents( QString& contents, const QString& s )
{
	int i = contents.indexOf( s );
	if (i != -1)
	{
		int start = i;
		int end = contents.indexOf('\n', i);
		if (end == -1)
			end = contents.length() -1;
		contents.remove(start, end -start +1);
	}
}
//
void QMakeProject::remove_multiline_contents( QString& contents, const QString& s, int* strt )
{
	int i;
	parse_multiline_part(contents, s, &i);
	if (strt)
		*strt = i;
	int start = i;
	bool lastWasBackspash = false;
	if (i != -1 && (i == 0 || contents[i -1] != '{' || contents[i -1] != ':'))
	{
		for (; i < (int)contents.length(); ++i)
		{
			if (contents[i] == '\n' && !lastWasBackspash)
				break;
			lastWasBackspash = (contents[i] == '\\' || lastWasBackspash && (contents[i] == ' ' || contents[i] == '\t'));
		}
		contents.remove(start, i -start +1);
	}
}
// PLATFORM SETTINGS
// READING
//
void QMakeProject::readPlatformSettings( const QString& contents )
{
	const QString platforms[] = { "", "win32", "unix", "mac", QString::null };
	QString setting, platform;
	// String Values
	for ( int x = 0; x < stringKeywords.count(); x++ )
	{
		setting = stringKeywords.at( x );
		for ( int i = 0; platforms[i] != QString::null; i++ )
		{
			platform = platforms[i];
			if ( !platform.isEmpty() )
				platform += ":";
			QStringList lst = parse_multiline_part( contents, platform + setting );
			platform = platforms[i];
			if ( platform.isEmpty() )
				platform = "(all)";
			stringValues[ setting ].remove( platform );
			stringValues[ setting ].insert( platform, lst.join( " " ) );
		}
	}
	// StringList Values
	for ( int x = 0; x < stringListKeywords.count(); x++ )
	{
		setting = stringListKeywords.at( x );
		for ( int i = 0; platforms[i] != QString::null; i++ )
		{
			platform = platforms[i];
			if ( !platform.isEmpty() )
				platform += ":";
			QStringList lst = parse_multiline_part( contents, platform + setting );
			platform = platforms[i];
			if ( platform.isEmpty() )
				platform = "(all)";
			stringListValues[ setting ].remove( platform );
			stringListValues[ setting ].insert( platform, lst );
		}
	}
}
// REMOVING
void QMakeProject::removePlatformSettings( QString& contents )
{
	QList<QString> sl;
	QString keyword;
	// String Value
	foreach ( QString s, stringValues.keys() )
	{
		sl = stringValues[ s ].keys();
		sl.swap( 0, sl.count() -1 ); // put (all) at end to avoid bug
		foreach ( QString p, sl )
		{
			keyword = QString::null;
			if ( p != "(all)" )
				keyword = p + ":";
			keyword += s;
			remove_contents( contents, keyword );
		}
	}
	// StringList Value
	foreach ( QString s, stringListValues.keys() )
	{
		sl = stringListValues[ s ].keys();
		sl.swap( 0, sl.count() -1 ); // put (all) at end to avoid bug
		foreach ( QString p, sl )
		{
			keyword = QString::null;
			if ( p != "(all)" )
				keyword = p + ":";
			keyword += s;
			remove_multiline_contents( contents, keyword );
		}
	}
}
// WRITING
void QMakeProject::writePlatformSettings( QString& contents )
{
	static QStringList l = QStringList() << "TARGET" << "DESTDIR" << "VERSION" << "OBJECTS_DIR" << "UI_DIR" << "MOC_DIR" << "RCC_DIR";
	int c, i;
	// String Value
	foreach ( QString s, stringValues.keys() ) // TEMPLATE VERSION ...
	{
		foreach ( QString p, stringValues[ s ].keys() ) // (all), win32, unix, mac
		{
			if ( stringValues[ s ][ p ].isEmpty() )
				continue;
			if ( p != "(all)" )
			{
				contents += p + ":" + s + "\t";
				if ( !l.contains( s ) )
					contents += "+";
				contents += "= ";
			}
			else
				contents += s + "\t" + ( s == "CONFIG" ? "+= " : "= " );
			contents += stringValues[ s ][ p ] + "\n";
		}
	}
	contents += "\n";
	// StringList Value
	foreach ( QString s, stringListValues.keys() ) // SOURCES, HEADERS, FORMS ...
	{
		foreach ( QString p, stringListValues[ s ].keys() ) // (all), win32, unix, mac
		{
			if ( stringListValues[ s ][ p ].isEmpty() )
				continue;
			if ( p != "(all)" )
				contents += p + ":" + s + "\t+= ";
			else
				contents += s + "\t= ";
			c = stringListValues[ s ][ p ].count();
			i = 0;
			foreach ( QString v, stringListValues[ s ][ p ] ) // stringlist values
			{
				contents += v + ( i != c -1 ? " \\\n\t" : "\n" );
				i++;
			}
			contents += "\n";
		}
	}
}
//
bool QMakeProject::parse()
{
	QFile f( mFilePath );
	if ( !f.exists() || !f.open( QFile::ReadOnly | QFile::Text ) )
		return false;
	QTextStream ts( &f );
	QString contents = ts.readAll();
	f.close();
	readPlatformSettings( contents );
	return true;
}
// PROJECT
bool QMakeProject::open()
{
	if ( mFilePath.isEmpty() )
		return false;
	mLocked = true;
	bool _open = parse();
	if ( _open )
	{
		// init completion
		completion();
		// emit opened
		setOpen();
	}
	mLocked = false;
	return _open;
}
//
void QMakeProject::setOpen()
{
	createTreeProject();
	mOpen = true;
	emit opened( this );
	// if it s a subdirs project, create subprojects
	if ( stringValue( "TEMPLATE" ) == "subdirs" )
	{
		QStringList projects = stringListValue( "SUBDIRS" );
		QMakeProject* project;
		foreach ( QString value, projects )
		{
			// set full file path
			value = QString( "%1/%2/%3.pro" ).arg( absolutePath(), value, value.mid( value.lastIndexOf( "/" ) +1 ) );
			project = new QMakeProject( value, 0, this );
			// append to subprojects list of the parent
			mSubProjects.append( project );
			// redirect signals to parent
			connect( project, SIGNAL( opened( QMakeProject* ) ), this, SIGNAL( opened( QMakeProject* ) ) );
			connect( project, SIGNAL( closed( QMakeProject* ) ), this, SIGNAL( closed( QMakeProject* ) ) );
			connect( project, SIGNAL( modified( QMakeProject*, bool ) ), this, SIGNAL( modified( QMakeProject*, bool ) ) );
			connect( project, SIGNAL( openFile( QMakeProject*, const QString& ) ), this, SIGNAL( openFile( QMakeProject*, const QString& ) ) );
			// open the project
			project->open();
		}
	}
}
//
bool QMakeProject::isOpen()
{
	return mOpen;
}
//
void QMakeProject::setModified( bool _modified )
{
	mModified = _modified;
	emit modified( this, mModified );
}
//
bool QMakeProject::isModified()
{
	return mModified;
}
//
void QMakeProject::setLocked( bool locked )
{
	mLocked = locked;
}
//
bool QMakeProject::isLocked()
{
	return mLocked;
}
//
bool QMakeProject::save( bool locked )
{
	if ( !isOpen() )
		return false;
	if ( !isModified() )
	{
		mLocked = locked;
		return true;
	}
	QFile f( mFilePath );
	QString contents = "";
	bool hasPreviousContents = false;
	if ( f.open( QFile::ReadOnly | QFile::Text ) )
	{
		QTextStream ts( &f );
		contents = ts.readAll();
		f.close();
		hasPreviousContents = true;
		removePlatformSettings( contents );
		writePlatformSettings( contents );
		// Qt 4 Developper Studio Marks
		if ( !contents.contains( _PROGRAM_NAME ) )
			contents.prepend( QString( "warning( \"This file has been generated by %1 (%2) - %3\" )\n\n" ).arg( _PROGRAM_NAME ).arg( _PROGRAM_VERSION ).arg( _PROGRAM_WEBSITE ) );
		// Replace more than 2 newlines by 2 newlines
		while ( contents.indexOf( "\n\n\n" ) != -1 )
			contents.replace( "\n\n\n", "\n\n" );
		//
		if ( !f.open( QFile::WriteOnly | QFile::Text ) )
			return false;
		QTextStream os( &f );
		os << contents;
		f.close();
		setModified( false );
		mLocked = locked;
		return true;
	}
	return false;
}
//
bool QMakeProject::close()
{
	if ( !isOpen() )
		return false;
	foreach( QMakeProject* project, mSubProjects )
		delete project;
	if ( isModified() && QMessageBox::question( 0, _QUESTION, QString( tr( "Project %1 has been modified, save it?" ) ).arg( name() ), tr( "Yes" ), tr( "No" ) ) == 0 )
			save( true );
	setClose();
	if ( parent() )
	{
		QMakeProject* parentProject = qobject_cast<QMakeProject*>( parent() );
		if ( parentProject && parentProject->mSubProjects.contains( this ) )
			parentProject->mSubProjects.removeAll( this );
	}
	// Freeing memory project, Really Need for memory ?!
	if ( treeItem() )
		delete treeItem();
	foreach ( StringValues values, stringValues )
		values.clear();
	stringValues.clear();
	foreach ( StringListValues values, stringListValues )
		values.clear();
	stringListValues.clear();
	mFilePath.clear();
	return true;
}
//
void QMakeProject::setClose()
{
	mOpen = false;
	mLocked = true;
	emit closed( this );
}
//
void QMakeProject::setTreeItem( QTreeWidgetItem* item )
{
	mProjectTree[ name() ][ "(all)" ] = item;
}
//
QTreeWidgetItem* QMakeProject::treeItem()
{
	if ( mProjectTree.contains( name() ) && mProjectTree[ name() ].contains( "(all)" ) )
		return mProjectTree[ name() ][ "(all)" ];
	return 0;
}
//
QString QMakeProject::name()
{
	return QFileInfo( mFilePath ).completeBaseName();
}
//
QString QMakeProject::filePath()
{
	return mFilePath;
}
//
QString QMakeProject::absolutePath()
{
	return QFileInfo( mFilePath ).absolutePath();
}
//
QList<QMakeProject*>& QMakeProject::subProjects()
{
	return mSubProjects;
}
//
//
QStringList parseString( const QString& value )
{
	QString values = value;
	bool libs = false; //cbSetting->currentText().toLower() == "libs";
	QStringList list;
	int i;
	while ( !values.isEmpty() )
	{
		if ( ( i = values.indexOf( "\"" ) ) != -1 )
		{
			list << values.left( i -( libs ? 3 : 0 ) ).split( " ", QString::SkipEmptyParts );
			values = values.mid( i +1 -( libs ? 3 : 0 ) );
			if ( ( i = values.indexOf( "\"", libs ? 3 : 0 ) ) == -1 )
			{
				qWarning( "Can't parse the data, abording." );
				break;
			}
			list << values.left( i +( libs ? 1 : 0 ) );
			values = values.mid( i +1 );
		}
		else
		{
			list << values.split( " ", QString::SkipEmptyParts );
			values.clear();
		}
	}
	return list;
}
//
InitCompletion* QMakeProject::completion()
{
	if ( !mCompletion )
	{
		mCompletion = new InitCompletion( this );
		mCompletion->setTempFilePath( QCoreApplication::instance()->applicationDirPath() );
		mCompletion->setCtagsCmdPath( "ctags" );
	}
	// refresh include path
	QStringList l;
	// project path
	l << absolutePath();
	// qt include path
	l << Options::self()->value( "Options/General/QtPath" ).toString().append( "/include" );
	// project include path
	QStringList ls = QStringList() << "INCLUDEPATH" << "DEPENDPATH" << "VPATH";
	foreach ( QString s, ls )
	{
		if ( cStringValues().contains( s ) )
		{
			foreach ( QString s1, cStringValues().value( s ).keys() )
			{
				QStringList p = parseString( cStringValues().value( s ).value( s1 ) );
				//
				foreach ( QString d, p )
				{
#ifdef Q_OS_WIN32
					if ( d[1] == ':' )
						l << d;
					else
						l << absolutePath().append( "/" + d );
#else
					if ( d[0] == '/' )
						l << d;
					else
						l << absolutePath().append( "/" + d );
#endif
				}
			}
		}
	}
#ifndef Q_OS_WIN32
	// users includes
	l << "/usr/include";
#endif
	// add list
	mCompletion->addIncludes( l );
	//
	return mCompletion;
}
//
void QMakeProject::setStringValues( const HashStringValues& values  )
{
	if ( mLocked )
		return;
	stringValues = values;
}
//
void QMakeProject::setStringValues( const QString& setting, const QString& platform , const QString& value )
{
	if ( mLocked )
		return;
	stringValues[ setting ][ platform ] = value;
	setModified( true );
}
//
void QMakeProject::addStringValue( const QString& setting, const QString& platform, const QString& value )
{
	if ( mLocked )
		return;
	if ( !stringValues.value( setting ).value( platform ).contains( value ) )
	{
		stringValues[ setting ][ platform ].append( QString( " %1" ).arg( value ) ).trimmed();
		setModified( true );
	}
}
//
void QMakeProject::removeStringValue( const QString& setting, const QString& platform, const QString& value )
{
	if ( mLocked )
		return;
	if ( stringValues.value( setting ).value( platform ).contains( value ) )
	{;
		stringValues[ setting ][ platform ].remove( value ).simplified();
		setModified( true );
	}
}
//
const QString QMakeProject::stringValue( const QString& setting, const QString& platform )
{
	return stringValues.value( setting ).value( platform );
}
//
const HashStringValues QMakeProject::cStringValues()
{
	return stringValues;
}
//
void QMakeProject::setStringListValues( const HashStringListValues& values  )
{
	if ( mLocked )
		return;
	stringListValues = values;
}
//
void QMakeProject::setStringListValues( const QString& setting, const QString& platform, const QStringList& value )
{
	if ( mLocked )
		return;
	stringListValues[ setting ][ platform ] = value;
	setModified( true );
}
//
void QMakeProject::addStringListValue( const QString& setting, const QString& platform, const QString& value )
{
	if ( mLocked )
		return;
	if ( !stringListValues.value( setting ).value( platform ).contains( relative( value ) ) )
	{
		stringListValues[ setting ][ platform ] << relative( value );
		updateTreeItem( setting, platform, value, true );
		setModified( true );
	}
}
//
void QMakeProject::addStringListValues( const QString& setting, const QString& platform, const QStringList& values )
{
	if ( mLocked )
		return;
	foreach ( QString value, values )
		addStringListValue( setting, platform, value );
}
//
void QMakeProject::removeStringListValue( const QString& setting, const QString& platform, const QString& value )
{
	if ( mLocked )
		return;
	if ( stringListValues.value( setting ).value( platform ).contains( relative( value ) ) )
	{
		stringListValues[ setting ][ platform ].removeAll( relative( value ) );
		updateTreeItem( setting, platform, value, false );
		setModified( true );
	}
}
//
const QStringList QMakeProject::stringListValue( const QString& setting, const QString& platform )
{
	return stringListValues.value( setting ).value( platform );
}
//
const HashStringListValues QMakeProject::cStringListValues()
{
	return stringListValues;
}
//
// Building
QString QMakeProject::qmake()
{
	return Options::qtBin( "qmake" ).append( Tools::quoted( filePath() ).prepend( " " ) );
}
//
QString QMakeProject::make()
{
#ifdef Q_WS_WIN
	QString make = Options::mingwBin( "mingw32-make" );
	QString _template = stringValue( "TEMPLATE" );
	QString config = stringValue( "CONFIG" );
	if ( _template.toLower() != "subdirs" )
		make.append( config.contains( "release", Qt::CaseInsensitive ) ? " release" : " debug" );
	return make;
#else
	return Options::mingwBin( "make" );
#endif
}
//
QString QMakeProject::makeClean()
{
#ifdef Q_WS_WIN
	return make().append( "-clean" );
#else
	return make().append( " clean" );
#endif
}
//
QString QMakeProject::lupdate()
{
	return Options::qtBin( "lupdate" ).append( Tools::quoted( filePath() ).prepend( " " ) );
}
//
QString QMakeProject::lrelease()
{
	return Options::qtBin( "lrelease" ).append( Tools::quoted( filePath() ).prepend( " " ) );
}
//
QString QMakeProject::execute()
{
	QString target;
	QString destdir;
	QString filePath;
	//
	target = stringValue( "TARGET", currentPlatform() ).isEmpty() ? stringValue( "TARGET" ) : stringValue( "TARGET", currentPlatform() );
	if ( target.isEmpty() )
		target = name();
#ifdef Q_WS_MAC // need some fix if project is not in a bundle
	//The target will be in a .app directory
	QString realTarget( target );
	realTarget.append( ".app/Contents/MacOS/" ).append( target );
	target = realTarget;
#endif
	destdir = stringValue( "DESTDIR", currentPlatform() ).isEmpty() ? stringValue( "DESTDIR" ) : stringValue( "DESTDIR", currentPlatform() );
	if ( currentPlatform() == "win32" && destdir.isEmpty() )
		destdir = stringValue( "CONFIG" ).contains( "debug", Qt::CaseInsensitive ) ? "debug" : "release";
	if ( !destdir.isEmpty() )
	{
		if ( destdir[0] == '/' )
			filePath = destdir.append( "/" ).append( target );
		else
			filePath = absolutePath().append( "/" ).append( destdir ).append( "/" ).append( target );
	}
	else
		filePath = absolutePath().append( "/" ).append( target );
	return Tools::quoted( filePath );
}
//
// Tools
const QString QMakeProject::currentPlatform()
{
#ifdef Q_WS_WIN
	return "win32";
#elif defined( Q_WS_X11 )
	return "unix";
#elif defined( Q_WS_MAC )
	return "mac";
#endif
	return "(all)";
}
//
const QString QMakeProject::absolute( const QString& fileName )
{
	return Tools::makeAbsolute( fileName, mFilePath );
}
//
const QString QMakeProject::relative( const QString& filePath )
{
	return Tools::makeRelative( filePath, mFilePath );
}
//
pFileType QMakeProject::getFileTypeFromFilePath( const QString& filePath )
{
	if ( filePath.isEmpty() )
		return pInvalidate;
	//QString absoluteFilePath = absolute( filePath );
	if ( !QFileInfo( filePath ).exists() ) // absoluteFilePath
		return pNotExists;
	QString suffix = QFileInfo( filePath ).suffix(); // absoluteFilePath
	if ( suffix.isEmpty() )
		return pOther;
	else if ( uiExtension.contains( suffix ) )
		return pForm;
	else if ( headerExtension.contains( suffix ) )
		return pHeader;
	else if ( sourceExtension.contains( suffix ) )
		return pSource;
	else if ( translationExtension.contains( suffix ) )
		return pTranslation;
	else if ( textExtension.contains( suffix ) )
		return pText;
	else if ( resourceExtension.contains( suffix ) )
		return pResource;
	else if ( rcExtension.contains( suffix ) )
		return pRc;
	else if ( resExtension.contains( suffix ) )
		return pRes;
	else if ( defExtension.contains( suffix ) )
		return pDef;
	else
		return pOther;
}
//
void QMakeProject::createTreeProject()
{
	// Creating Tree
	const QStringList platforms = QStringList() << "(all)" << "win32" << "unix" << "mac";
	QString setting, projectFilePath = filePath();
	QTreeWidgetItem *mTreeItem = 0, *topItem, *subItem, *item;
	QStringList items;
	// Creating All Project Tree Items
	if ( mTree )
		mTreeItem = new QTreeWidgetItem( mTree );
	else
	{
		QMakeProject* project = qobject_cast<QMakeProject*>( parent() );
		if ( project && project->treeItem() )
			mTreeItem = new QTreeWidgetItem( project->treeItem() );
	}
	if ( !mTreeItem )
		return;
	// Setting Project Item
	mTreeItem->setIcon( 0, QIcon( itemsIcon.at( 0 ) ) );
	mTreeItem->setText( 0, name() );
	mTreeItem->setTextColor( 0, Qt::blue );
	mTreeItem->setData( 0, QTreeWidgetItem::UserType +1, pProject );
	mTreeItem->setData( 0, QTreeWidgetItem::UserType +2, filePath() );
	setTreeItem( mTreeItem );
	if ( stringValue( "TEMPLATE" ) == "subdirs" )
		return;
	for ( int i = 0; i < 5; i++ )
	{
		// Items like Forms etc
		topItem = new QTreeWidgetItem( mTreeItem );
		topItem->setText( 0, itemsHeader.at( i +1 ) );
		topItem->setIcon( 0, QIcon( itemsIcon.at( i +1 ) ) );
		topItem->setTextColor( 0, Qt::blue );
		topItem->setData( 0, QTreeWidgetItem::UserType +1, i );
		topItem->setData( 0, QTreeWidgetItem::UserType +2, filePath() );
		setting = stringListKeywords.at( i );
		mProjectTree[ setting ][ "(all)" ] = topItem;
		//
		foreach ( QString platform, platforms )
		{
			if ( platform != "(all)" && setting != "TRANSLATIONS" )
			{
				// Items like win32
				subItem = new QTreeWidgetItem( topItem );
				subItem->setText( 0, platform );
				subItem->setIcon( 0, QIcon( itemsIcon.at( i +1 ) ) );
				subItem->setTextColor( 0, Qt::blue );
				subItem->setData( 0, QTreeWidgetItem::UserType +1, i );
				subItem->setData( 0, QTreeWidgetItem::UserType +2, filePath() );
				mProjectTree[ setting ][ platform ] = subItem;
			}
			// Items File
			subItem = mProjectTree[ setting ][ platform ];
			items = stringListValue( setting, platform );
			foreach ( QString file, items )
			{
				item = new QTreeWidgetItem( subItem );
				item->setText( 0, file );
				item->setData( 0, QTreeWidgetItem::UserType +1, i +pForm );
				item->setData( 0, QTreeWidgetItem::UserType +2, absolutePath().append( "/%1" ).arg( file ) );
			}
		}
	}
	// Reload Opened Files
	if ( Options::self()->value( "Options/General/ReloadLastProjectFiles", true ).toBool() )
	{
		QStringList files = stringListValue( "OPENEDFILES" );
		foreach ( QString fileName, files )
			emit openFile( this, absolute( fileName ) );
	}
}
//
void QMakeProject::updateTreeItem( const QString& setting, const QString& platform, const QString& value, bool add )
{
	if ( !mProjectTree.contains( setting ) || !mProjectTree[ setting ].contains( platform ) )
		return;
	const QStringList platforms = QStringList() << "win32" << "unix" << "mac";
	QTreeWidgetItem *topItem = mProjectTree[ setting ][ platform ], *item;
	if ( add )
	{
		item = new QTreeWidgetItem;
		item->setText( 0, relative( value ) );
		item->setData( 0, QTreeWidgetItem::UserType +1, getFileTypeFromFilePath( value ) );
		item->setData( 0, QTreeWidgetItem::UserType +2, value );
		// if the topItem is a platform
		if ( platforms.contains( topItem->text( 0 ) ) )
			topItem->addChild( item );
		else
		{
			int childs = topItem->childCount();
			// if not a platform, and contain platforms
			if ( childs > 0 && topItem->child( childs -1 )->text( 0 ) == "mac" )
				topItem->insertChild( childs -3, item );
			else
				topItem->addChild( item );
		}
	}
	else
	{
		for ( int i = 0; i < topItem->childCount(); i++ )
		{
			item = topItem->child( i );
			if ( item->data( 0, QTreeWidgetItem::UserType +2 ).toString() == value )
			{
				delete item;
				break;
			}
		}
	}
}
