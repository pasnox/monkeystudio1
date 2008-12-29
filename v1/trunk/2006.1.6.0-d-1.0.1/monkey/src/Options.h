/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 30 janvier 2006 - 17h47
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : Options.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef OPTIONS_H
#define OPTIONS_H
//
#include <QStringList>
#include <QSettings>
#include <QPointer>
//
class QMainWindow;
//class QTextCharFormat;
//
enum TabsMethod { tmSpaces = 0, tmTabs };
enum IndentMethod { imNone = 0, imDefault, imSmart };
enum SourceMethod { smMaximized = 0, smVertically, smHorizontally };
//
class Options : public QSettings
{
	Q_OBJECT
	//
public:
	static Options* self();
	//
	static QString mingwBin( const QString& );
	static QString qtBin( const QString& );
	static QString templateFile( const QString& );
	static QString translationFile( const QString& );
	static QString documentationFile( const QString& );
	//
	static void restoreState( QMainWindow* );
	static void saveState( QMainWindow* );
	//
	static QStringList languages();
	static void setDefaultOptions();
	//
private:
	Options();
	static QPointer<Options> mSelf;
	static QStringList mLanguages;
};
//
#endif // OPTIONS_H
