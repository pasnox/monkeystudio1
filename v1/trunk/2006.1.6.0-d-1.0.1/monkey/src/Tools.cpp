/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : vendredi 27 janvier 2006 - 00h23
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : Tools.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "Tools.h"
#include "UIMain.h"
#include "UIOptions.h"
#include "Options.h"
//
#include <QObject>
#include <QLayout>
#include <QFileInfo>
#include <QDir>
#include <QListWidget>
#include <QProcess>
#if defined( Q_OS_WIN32 )
#include <qt_windows.h>
#endif
//
QString Tools::makeAbsolute( const QString& source, const QString& from )
{
	if ( source.isEmpty() )
		return QString::null;
	QFileInfo file( QDir( from.isEmpty() ? "." : QFileInfo( from ).absolutePath() ), source );
	if ( !file.exists() )
		return source;
	return file.absoluteFilePath();
}
//
QString Tools::makeRelative( const QString& source, const QString& from )
{
	if ( source.isEmpty() )
		return QString::null;
	QFileInfo fileSource( source );
	QFileInfo fileFrom(( from.isEmpty() ? "." : from ) );
	fileSource.makeAbsolute();
	fileFrom.makeAbsolute();
	//
	if ( !fileSource.exists() )
		return source;
	//
	QString sSource = fileSource.absoluteFilePath(); // A file or path
	QString sFrom = fileFrom.absoluteFilePath(); // A file or path
	if ( fileFrom.isFile() )
		sFrom = fileFrom.absolutePath(); // A path
	//
	// from is contained in source ...
	if ( sSource.left( sFrom.length() ) == sFrom )
	{
		sSource = sSource.remove( 0, sFrom.length() +1 );
		return sSource;
	}
	//
#if defined( Q_OS_WIN32 )
	// if drive differs...
	if ( sSource.left( 3 ) != sFrom.left( 3 ) )
		return source;
	sSource = sSource.remove( 0, 3 ); // remove drive
	sFrom = sFrom.remove( 0, 3 );
#else
	sSource = sSource.remove( 0, 1 ); // remove racine
	sFrom = sFrom.remove( 0, 1 );
#endif
	// source is back
	// check if a piece of from is in source
	QString tmp;
	int last = sFrom.length();
	for ( int i = sFrom.length() -1; i > -1; i-- )
	{
		if ( sFrom[i] == '/' )
		{
			tmp += "../";
			last = i +1;
		}
		if ( sSource.indexOf( sFrom.left( last ) ) != -1 ) // a piece of from in source
		{
			sSource = tmp.append( sSource.mid( i +1 ) );
			return sSource;
			break;
		}
	}
	// no piece of from in source
	for ( int i = 0; i < sFrom.count( "/" ); i++ )
		tmp += "../";
	sSource = tmp.append( sSource );
	return sSource;
}
//
void Tools::setLayout( QObject* parent, int margin, int spacing )
{
	QList<QLayout*> layouts = parent->findChildren<QLayout*>();
	for ( int i = 0; i < layouts.count(); i++ )
	{
		layouts[i]->setMargin( margin );
		layouts[i]->setSpacing( spacing );
		if ( layouts[i]->findChildren<QLayout*>().count() > 0 )
			setLayout( layouts[i], margin, spacing );
	}
}
//
void Tools::alignItems( QListWidget* lwWidget, Qt::Alignment alignment, Qt::ItemFlags itemFlags )
{
	for ( int i = 0; i < lwWidget->count(); i++ )
	{
		lwWidget->item( i )->setTextAlignment( alignment );
		lwWidget->item( i )->setFlags( itemFlags );
	}
}
//
QString Tools::quoted( const QString& command )
{
	if ( !command.contains( QChar( 32 ) ) )
		return command;
	return QString( "\"%1\"" ).arg( command ); 
}
//
bool Tools::isKDERunning()
{
    return !qgetenv( "KDE_FULL_SESSION" ).isEmpty();
}
//
bool Tools::isGnomeRunning()
{
    return !qgetenv( "GNOME_FULL_SESSION" ).isEmpty();
}
//
void Tools::openUrl( const QUrl& url )
{
	if ( !url.isValid() )
		return;
	// Web
	if ( url.scheme() == QLatin1String( "http" ) || url.scheme() == QLatin1String( "ftp" ) || url.scheme() == QLatin1String( "mailto" ) )
	{
		QString webbrowser = Options::self()->value( "Options/General/WebBrowser" ).toString();
#if defined( Q_OS_WIN32 )
		if ( webbrowser.isEmpty() )
		{
			QT_WA( { ShellExecute( UIMain::self()->winId(), 0, (TCHAR*)url.toString().utf16(), 0, 0, SW_SHOWNORMAL ); },
			{ ShellExecuteA( UIMain::self()->winId(), 0, url.toString().toLocal8Bit(), 0, 0, SW_SHOWNORMAL ); } ) ;
			return;
		}
#endif
		if ( webbrowser.isEmpty() )
		{
#if defined( Q_OS_MAC )
			webbrowser = "open";
#elif defined( Q_WS_X11 )
			if ( isKDERunning() )
				webbrowser = "kfmclient";
/*
			else if ( isGnomeRunning() )
				webbrowser = "";
*/
#endif
		}
		if ( webbrowser.isEmpty() )
		{
			int result = QMessageBox::information( UIMain::self(), QObject::tr( "Help" ), QObject::tr( "Currently no Web browser is selected.\nPlease use the settings dialog to specify one!\n" ), QObject::tr( "Open" ), QObject::tr( "Cancel" ) );
			if ( result == 0 )
			{
				if ( !UIOptions::self( UIMain::self() )->exec() )
					return;
				webbrowser = Options::self()->value( "Options/General/WebBrowser" ).toString();
			}
			if ( webbrowser.isEmpty() )
				return;
		}
		QProcess* proc = new QProcess( UIMain::self() );
		QObject::connect( proc, SIGNAL( finished( int ) ), proc, SLOT( deleteLater() ) );
		QStringList args;
		if ( webbrowser == QLatin1String( "kfmclient" ) )
			args.append( QLatin1String( "exec" ) );
		args.append( url.toString() );
		proc->start( webbrowser, args );
		return;
	}
	// Pdf
	if ( url.path().right( 3 ).toLower() == QLatin1String( "pdf" ) )
	{
		QString pdfbrowser = Options::self()->value( "Options/General/PdfViewer" ).toString();
#if defined( Q_OS_WIN32 )
		if ( pdfbrowser.isEmpty() )
		{
			QT_WA( { ShellExecute( UIMain::self()->winId(), 0, (TCHAR*)url.toString().utf16(), 0, 0, SW_SHOWNORMAL ); },
			{ ShellExecuteA( UIMain::self()->winId(), 0, url.toString().toLocal8Bit(), 0, 0, SW_SHOWNORMAL ); } ) ;
			return;
		}
#endif
		if ( pdfbrowser.isEmpty() )
		{
#if defined( Q_OS_MAC )
			pdfbrowser = "open";
#elif defined( Q_WS_X11 )
			if ( isKDERunning() )
				pdfbrowser = "kfmclient";
/*
			else if ( isGnomeRunning() )
				pdfbrowser = "";
*/
#endif
		}
		if ( pdfbrowser.isEmpty() )
		{
			int result = QMessageBox::information( UIMain::self(), QObject::tr( "Help" ), QObject::tr( "Currently no PDF viewer is selected.\nPlease use the settings dialog to specify one!\n" ), QObject::tr( "Open" ), QObject::tr( "Cancel" ) );
			if ( result == 0 )
			{
				if ( !UIOptions::self( UIMain::self() )->exec() )
						return;
				pdfbrowser = Options::self()->value( "Options/General/PdfViewer" ).toString();
			}
			if ( pdfbrowser.isEmpty() )
				return;
		}
		QFileInfo info( pdfbrowser );
		if( !info.exists() )
		{
				QMessageBox::information( UIMain::self(), QObject::tr( "Help" ), QObject::tr( "Unable to start the PDF Viewer\n\n"
				"%1\n\n"
				"Please make sure that the executable exists and is located at\n"
				"the specified location." ).arg( pdfbrowser ) );
			return;
		}
		QProcess* proc = new QProcess( UIMain::self() );
		QObject::connect( proc, SIGNAL( finished() ), proc, SLOT( deleteLater() ) );
		QStringList args;
		if ( pdfbrowser == QLatin1String( "kfmclient" ) )
			args.append( QLatin1String( "exec" ) );
		args.append( url.toString() );
		proc->start( pdfbrowser, args );
		return;
	}
	// File
	if ( url.scheme() == QLatin1String( "file" ) )
		shellExecute( url.toLocalFile() );
}
//
void Tools::shellExecute( const QString& fileName )
{
	if ( !QFile::exists( fileName ) )
		return;
	QString shell;
#if defined( Q_OS_WIN32 )
	QT_WA( { ShellExecute( UIMain::self()->winId(), 0, (TCHAR*)fileName.utf16(), 0, 0, SW_SHOWNORMAL ); },
	{ ShellExecuteA( UIMain::self()->winId(), 0, fileName.toLocal8Bit(), 0, 0, SW_SHOWNORMAL ); } ) ;
	return;
#elif defined( Q_OS_MAC )
	shell = "open";
#elif defined( Q_WS_X11 )
	if ( isKDERunning() )
		shell = "kfmclient";
/*
	else if ( isGnomeRunning() )
		shell = "";
*/
#endif
	QProcess* proc = new QProcess( UIMain::self() );
	QObject::connect( proc, SIGNAL( finished( int ) ), proc, SLOT( deleteLater() ) );
	QStringList args;
	if ( shell == QLatin1String( "kfmclient" ) )
		args.append( QLatin1String( "exec" ) );
	args.append( fileName );
	proc->start( shell, args );
}
