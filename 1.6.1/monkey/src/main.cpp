/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : jeudi 05 janvier 2006 - 19h18
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : main.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include <QSplashScreen>
#include <QLocale>
#include <QTranslator>
//
#include "main.h"
#include "UIMain.h"
#include "Options.h"
//
int main(int argc, char ** argv)
{
	// Create Application
	QApplication app( argc, argv );
	app.addLibraryPath( "plugins" );
	app.setStyle( Options::self()->value( "Options/Style", "plastique" ).toString() );
	app.setOrganizationName( _ORGANIZATION );
	app.setOrganizationDomain( QString( "www.%1.com" ).arg( _ORGANIZATION ).toLower() );
	app.setApplicationName( _PROGRAM_NAME ); 
	// Create SplashScreen
	QSplashScreen* splash = new QSplashScreen( QPixmap( ":/Icons/Icons/helpsplashscreen.png" ) );
	splash->show();
	splash->showMessage( QObject::tr( "Initializing Application..." ), Qt::AlignRight | Qt::AlignTop );
	// Load Options
	Options::self();
	//  Set Default User Widget Button Color
	Options::self()->setValue( "Options/PaletteButtonColor", qApp->palette().color( QPalette::Button ).name() );
	splash->showMessage( QObject::tr( "Options loaded" ), Qt::AlignRight | Qt::AlignTop );
	// Load Locale Language
	QTranslator translator;
	translator.load( Options::translationFile( QLocale::languageToString( QLocale::system().language() ) ) );
	app.installTranslator( &translator );
	splash->showMessage( QObject::tr( "Language changed to locale" ), Qt::AlignRight | Qt::AlignTop );
	// Load Main Window
	UIMain::self()->setWindowTitle( _PROGRAM_TITLE );
	Options::restoreState( UIMain::self() );
	UIMain::self()->show();
	splash->showMessage( QObject::tr( "Application Ready" ), Qt::AlignRight | Qt::AlignTop );
	// Delete SplashScreen
	splash->finish( UIMain::self() );
	delete splash;
	// First Time Setting
	if ( Options::self()->value( "Options/FirstTimeSetting", true ).toBool() )
		UIMain::self()->on_actionEditOptions_triggered();
	// Load Command Line Project or Reload Last
	// Command Line
	if ( argc > 1 )
		UIMain::self()->openProject( argv[1] );
	// Last
	else if ( Options::self()->value( "Options/General/ReloadLastProject", true ).toBool() && !Options::self()->value( "Options/General/RecentProjects" ).toStringList().isEmpty() )
		UIMain::self()->openProject( Options::self()->value( "Options/General/RecentProjects" ).toStringList()[0] );
	//
	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();
}
