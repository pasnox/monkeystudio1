/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 09 janvier 2006 - 14h21
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UINewFiles.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "UINewFiles.h"
#include "UIMain.h"
#include "main.h"
#include "QMakeProject.h"
//
#include <QDir>
#include <QFile>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
//
QPointer<UINewFiles> UINewFiles::_self = 0L;
//
UINewFiles* UINewFiles::self( QWidget* parent, UINewFilesType type )
{
	if ( !_self )
		_self = new UINewFiles( parent, type );
	return _self;
}
//
UINewFiles::UINewFiles( QWidget* parent, UINewFilesType type )
	: QDialog( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
	connect( tbProjectsLocation, SIGNAL( clicked() ), this, SLOT( tbBrowse_clicked() ) );
	connect( tbFilesLocation, SIGNAL( clicked() ), this, SLOT( tbBrowse_clicked() ) );
	connect( tbTemplatesLocation, SIGNAL( clicked() ), this, SLOT( tbBrowse_clicked() ) );
	switch( type )
	{
	case ftProject:
		twTemplates->setCurrentIndex( 0 );
		tabFiles->setEnabled( false );
		tabTemplates->setEnabled( false );
		break;
	case ftFile:
		twTemplates->setCurrentIndex( 1 );
		tabProjects->setEnabled( false );
		tabTemplates->setEnabled( false );
		leFilesLocation->setText( UIMain::self()->currentProject()->absolutePath().append( "/src" ) );
		break;
	case ftTemplate:
		twTemplates->setCurrentIndex( 2 );
		tabProjects->setEnabled( false );
		tabFiles->setEnabled( false );
		leFilesLocation->setText( UIMain::self()->currentProject()->absolutePath().append( "/src" ) );
		break;
	}
}
//
void UINewFiles::tbBrowse_clicked()
{
	QObject* object = sender();
	QString pathName;
	//
	if ( object == tbProjectsLocation )
	{
		pathName = QFileDialog::getExistingDirectory( this, tr( "Choose a path for your project" ), leProjectsLocation->text() );
		if ( !pathName.isEmpty() )
			leProjectsLocation->setText( pathName );
	}
	else if ( object == tbFilesLocation )
	{
		pathName = QFileDialog::getExistingDirectory( this, tr( "Choose a path for your file" ), leFilesLocation->text() );
		if ( !pathName.isEmpty() )
			leFilesLocation->setText( pathName );
	}
	else if ( object == tbTemplatesLocation )
	{
		pathName = QFileDialog::getExistingDirectory( this, tr( "Choose a path for your file" ), leTemplatesLocation->text() );
		if ( !pathName.isEmpty() )
			leTemplatesLocation->setText( pathName );
	}
}
//
void UINewFiles::accept()
{
	if ( tabProjects->isEnabled() )
	{
		if ( leProjectsFileName->text().isEmpty() )
			return;
		hide();
		if ( !projectAccepted() )
		{
			QMessageBox::warning( this, _WARNING, tr( "Unable to create the project." ) );
			show();
			return;
		}
	}
	else if ( tabFiles->isEnabled() )
	{
		if ( leFilesFileName->text().isEmpty() )
			return;
		hide();
		if ( !fileAccepted() )
		{
			QMessageBox::warning( this, _WARNING, tr( "Unable to create the file." ) );
			show();
			return;
		}
	}
	else if ( tabTemplates->isEnabled() )
	{
		if ( leTemplatesFileName->text().isEmpty() )
			return;
		hide();
		if ( !templateAccepted() )
		{
			QMessageBox::warning( this, _WARNING, tr( "Unable to create the template" ) );
			show();
			return;
		}
	}
	QDialog::accept();
}
//
bool UINewFiles::projectAccepted()
{
	QString projectFileName;
	projectFileName = leProjectsLocation->text();
	if ( !projectFileName.isEmpty() && !projectFileName.endsWith( "/" ) )
		projectFileName.append( "/" );
	projectFileName.append( leProjectsFileName->text() );
	if ( projectFileName.left( 4 ).toLower() != ".pro" )
		projectFileName.append( ".pro" );
	// Checking if file already exists
	if ( QFile::exists( projectFileName ) )
		return false;
	// Creating directory if it not exists
	QString path = QFileInfo( projectFileName ).path();
	QDir d( path );
	if ( !d.mkpath( d.path() ) )
		return false;
	// Creating project
	QString projectContents;
	switch( lwProjects->currentRow() )
	{
	case 0:
		{
			// GUI
			projectContents += "TEMPLATE\t= app\n";
			projectContents += "CONFIG\t+= release warn_on thread x11 windows app_bundle qt\n";
			projectContents += "QT\t= core gui\n";
			break;
		}
	case 1:
		{
			// CONSOLE
			projectContents += "TEMPLATE\t= app\n";
			projectContents += "CONFIG\t+= release warn_on thread console qt\n";
			projectContents += "QT\t= core\n";
			break;
		}
	case 2:
		{
			// DLL
			projectContents += "TEMPLATE\t= lib\n";
			projectContents += "CONFIG\t+= release warn_on thread lib_bundle dll qt\n";
			projectContents += "QT\t= core\n";
			break;
		}
	case 3:
		{
			// STATIC LIB
			projectContents += "TEMPLATE\t= lib\n";
			projectContents += "CONFIG\t+= release warn_on thread lib_bundle staticlib qt\n";
			projectContents += "QT\t= core\n";
			break;
		}
	case 4:
		{
			// PLUGIN
			projectContents += "TEMPLATE\t= lib\n";
			projectContents += "CONFIG\t+= release warn_on thread lib_bundle plugin qt\n";
			projectContents += "QT\t= core\n";
			break;
		}
	}
	// COMMON
	projectContents += "LANGUAGE\t= C++\n";
	projectContents += "unix:OBJECTS_DIR\t= build/.o/unix\n";
	projectContents += "win32:OBJECTS_DIR\t= build/.o/win32\n";
	projectContents += "mac:OBJECTS_DIR\t= build/.o/mac\n";
	projectContents += "UI_DIR\t= build/.ui\n";
	projectContents += "MOC_DIR\t= build/.moc\n";
	projectContents += "RCC_DIR\t= build/.rcc\n";
	projectContents += "INCLUDEPATH\t= . src src/ui\n";
	//
	QFile file( projectFileName );
	if ( file.open( QFile::WriteOnly | QFile::Text ) )
		file.write( projectContents.toAscii() );
	else
		return false;
	file.close();
	// Creating some default folders
	d.mkpath( "src/ui" );
	d.mkpath( "translations" );
	d.mkpath( "src/resources/images" );
	UIMain::self()->openProject( projectFileName );
	return true;
}
//
bool UINewFiles::fileAccepted()
{
	QString fileName, suffix, buffer, include, platform;
	QStringList platforms = QStringList() << "(all)" << "win32" << "unix" << "mac";
	bool ok;
	QMakeProject* project = 0L;
	//
	ok = false;
	while ( !ok || platform.isEmpty() )
		platform = QInputDialog::getItem( this, tr( "Choose ..." ), tr( "Choose the window manager for the file(s)." ), platforms, 0, true, &ok );
	//
	switch( lwFiles->currentRow() )
	{
	case 0:
		{
			// Text File
			fileName = leFilesLocation->text();
			if ( !fileName.isEmpty() && !fileName.endsWith( "/" ) )
				fileName.append( "/" );
			if ( !fileName.isEmpty() )
			{
				QDir d( fileName );
				if ( !d.mkpath( d.path() ) )
					return false;
			}
			fileName.append( leFilesFileName->text() );
			if ( QFile::exists( fileName ) )
				return false;
			suffix = QFileInfo( fileName ).suffix();
			if ( !QMakeProject::textExtension.contains( suffix, Qt::CaseInsensitive ) )
				fileName.append( ".txt" );
			QFile file( fileName );
			if ( !file.open( QFile::WriteOnly | QFile::Text ) )
			{
				return false;
				break;
			}
			if ( cbFilesAddToProject->isChecked() )
			{
				project = UIMain::self()->currentProject();
				project->addStringListValue( "TEXTS", platform, fileName );
			}
			UIMain::self()->createWindow( fileName, pText, project );
			break;
		}
	case 1:
		{
			// Main File
			fileName = leFilesLocation->text();
			if ( !fileName.isEmpty() && !fileName.endsWith( "/" ) )
				fileName.append( "/" );
			if ( !fileName.isEmpty() )
			{
				QDir d( fileName );
				if ( !d.mkpath( d.path() ) )
					return false;
			}
			fileName.append( leFilesFileName->text() );
			suffix = QFileInfo( fileName ).suffix();
			if ( !QMakeProject::sourceExtension.contains( suffix, Qt::CaseInsensitive ) )
				fileName.append( ".cpp" );
			if ( QFile::exists( fileName ) )
				return false;
			QFile templateFile( ":/Templates/Templates/main_source.txt" );
			if ( !templateFile.open( QFile::ReadOnly | QFile::Text ) )
				return false;
			// Only ask if project template is an application
			if (
				UIMain::self()->currentProject()->stringValue( "TEMPLATE" ).contains( "app", Qt::CaseInsensitive ) &&
				!UIMain::self()->currentProject()->stringValue( "CONFIG" ).contains( "console", Qt::CaseInsensitive )
			   )
			{
				ok = false;
				while ( !ok || include.isEmpty() )
					include = QInputDialog::getItem( this, tr( "Choose a file..." ), tr( "Choose the file to include,\nit will be used to create the default application widget" ), UIMain::self()->currentProject()->stringListValue( "HEADERS" ), 0, true, &ok );
			}
			QFile mainFile( fileName );
			if ( !mainFile.open( QFile::WriteOnly | QFile::Text ) )
				return false;
			if ( include.isEmpty() )
			{
				// Remove reference to widget application
				QByteArray contents( templateFile.readAll() );
				contents.replace( "QApplication", "QCoreApplication" );
				contents.replace( "#include \"INCLUDE\"\n//\n", "" );
				contents.replace( "\tCLASSNAME w;\n", "" );
				contents.replace( "\tw.show();\n", "" );
				contents.replace( "\tapp.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );\n", "" );
				mainFile.write( contents );
			}
			else
				mainFile.write( templateFile.readAll().replace( "INCLUDE", include.toAscii() ).replace( "CLASSNAME", QFileInfo( include ).baseName().toAscii() ) );
			templateFile.close();
			mainFile.close();
			if ( cbFilesAddToProject->isChecked() )
			{
				project = UIMain::self()->currentProject();
				project->addStringListValue( "SOURCES", platform, fileName );
			}
			UIMain::self()->createWindow( fileName, pSource, project );
			break;
		}
	case 2:
		{
			// Header File
			fileName = leFilesLocation->text();
			if ( !fileName.isEmpty() && !fileName.endsWith( "/" ) )
				fileName.append( "/" );
			if ( !fileName.isEmpty() )
			{
				QDir d( fileName );
				if ( !d.mkpath( d.path() ) )
					return false;
			}
			fileName.append( leFilesFileName->text() );
			suffix = QFileInfo( fileName ).suffix();
			if ( !QMakeProject::headerExtension.contains( suffix, Qt::CaseInsensitive ) )
				fileName.append( ".h" );
			if ( QFile::exists( fileName ) )
				return false;
			QFile file( fileName );
			if ( !file.open( QFile::WriteOnly | QFile::Text ) )
			{
				return false;
				break;
			}
			if ( cbFilesAddToProject->isChecked() )
			{
				project = UIMain::self()->currentProject();
				project->addStringListValue( "HEADERS", platform, fileName );
			}
			UIMain::self()->createWindow( fileName, pHeader, project );
			break;
		}
	case 3:
		{
			// Source File
			fileName = leFilesLocation->text();
			if ( !fileName.isEmpty() && !fileName.endsWith( "/" ) )
				fileName.append( "/" );
			if ( !fileName.isEmpty() )
			{
				QDir d( fileName );
				if ( !d.mkpath( d.path() ) )
					return false;
			}
			fileName.append( leFilesFileName->text() );
			suffix = QFileInfo( fileName ).suffix();
			if ( !QMakeProject::sourceExtension.contains( suffix, Qt::CaseInsensitive ) )
				fileName.append( ".cpp" );
			if ( QFile::exists( fileName ) )
				return false;
			QFile file( fileName );
			if ( !file.open( QFile::WriteOnly | QFile::Text ) )
			{
				return false;
				break;
			}
			if ( cbFilesAddToProject->isChecked() )
			{
				project = UIMain::self()->currentProject();
				project->addStringListValue( "SOURCES", platform, fileName );
			}
			UIMain::self()->createWindow( fileName, pSource, project );
			break;
		}
	case 4:
		{
			// Class
			QString baseName = QFileInfo( leFilesFileName->text() ).baseName();
			fileName = leFilesLocation->text();
			if ( !fileName.isEmpty() && !fileName.endsWith( "/" ) )
				fileName.append( "/" );
			if ( !fileName.isEmpty() )
			{
				QDir d( fileName );
				if ( !d.mkpath( d.path() ) )
					return false;
			}
			fileName.append( baseName );
			// Test if files exists
			if ( QFile::exists( QString( "%1.h" ).arg( fileName ) ) || QFile::exists( QString( "%1.cpp" ).arg( fileName ) ) )
				return false;
			// BEGIN
			if ( QMessageBox::question( this, _QUESTION, tr( "Must this class herits from something ?" ), tr( "Yes" ), tr( "No" ) ) == 0 )
			{
				// Templates: objectClass_header.txt & objectClass_source.txt
				// Header
				QFile templateFile( ":/Templates/Templates/objectClass_header.txt" );
				if ( !templateFile.open( QFile::ReadOnly | QFile::Text ) )
					return false;
				ok = false;
				while ( !ok || include.isEmpty() )
					include = QInputDialog::getItem( this, tr( "Choose a file..." ), tr( "Choose the file to herits from,\nyou can directly enter a Qt Class :\( ie: QWidget or qwidget.h )" ), UIMain::self()->currentProject()->stringListValue( "HEADERS" ), 0, true, &ok );
				buffer = templateFile.readAll();
				buffer.replace( "FILENAME", QString( "%1_h" ).arg( baseName ).toUpper() );
				buffer.replace( "INCLUDE", include );
				buffer.replace( "CLASSNAME", baseName );
				buffer.replace( "HERITS", QFileInfo( include ).baseName() );
				QFile file( QString( "%1.h" ).arg( fileName ) );
				if ( !file.open( QFile::WriteOnly | QFile::Text ) )
					return false;
				file.write( buffer.toAscii() );
				templateFile.close();
				file.close();
				// Source
				templateFile.setFileName( ":/Templates/Templates/objectClass_source.txt" );
				if ( !templateFile.open( QFile::ReadOnly | QFile::Text ) )
					return false;
				buffer = templateFile.readAll();
				buffer.replace( "FILENAME", QString( "%1.h" ).arg( baseName ) );
				buffer.replace( "CLASSNAME", baseName );
				buffer.replace( "HERITS", QFileInfo( include ).baseName() );
				file.setFileName( QString( "%1.cpp" ).arg( fileName ) );
				if ( !file.open( QFile::WriteOnly | QFile::Text ) )
					return false;
				file.write( buffer.toAscii() );
				templateFile.close();
				file.close();
			}
			else
			{
				// Templates: emptyClass_header.txt & emptyClass_source.txt
				// Header
				QFile templateFile( ":/Templates/Templates/emptyClass_header.txt" );
				if ( !templateFile.open( QFile::ReadOnly | QFile::Text ) )
					return false;
				buffer = templateFile.readAll();
				buffer.replace( "FILENAME", QString( "%1_h" ).arg( baseName ).toUpper() );
				buffer.replace( "CLASSNAME", baseName );
				QFile file( QString( "%1.h" ).arg( fileName ) );
				if ( !file.open( QFile::WriteOnly | QFile::Text ) )
					return false;
				file.write( buffer.toAscii() );
				templateFile.close();
				file.close();
				// Source
				templateFile.setFileName( ":/Templates/Templates/emptyClass_source.txt" );
				if ( !templateFile.open( QFile::ReadOnly | QFile::Text ) )
					return false;
				buffer = templateFile.readAll();
				buffer.replace( "FILENAME", QString( "%1.h" ).arg( baseName ) );
				buffer.replace( "CLASSNAME", baseName );
				file.setFileName( QString( "%1.cpp" ).arg( fileName ) );
				if ( !file.open( QFile::WriteOnly | QFile::Text ) )
					return false;
				file.write( buffer.toAscii() );
				templateFile.close();
				file.close();
			}
			// END
			if ( cbFilesAddToProject->isChecked() )
			{
				project = UIMain::self()->currentProject();
				project->addStringListValue( "HEADERS", platform, QString( "%1.h" ).arg( fileName ) );
				project->addStringListValue( "SOURCES", platform, QString( "%1.cpp" ).arg( fileName ) );
			}
			UIMain::self()->createWindow( fileName.append( ".h" ), pHeader, project );
			break;
		}
	case 5:
		{
			// PlugIn Class
			QString baseName = QFileInfo( leFilesFileName->text() ).baseName();
			fileName = leFilesLocation->text();
			if ( !fileName.isEmpty() && !fileName.endsWith( "/" ) )
				fileName.append( "/" );
			if ( !fileName.isEmpty() )
			{
				QDir d( fileName );
				if ( !d.mkpath( d.path() ) )
					return false;
			}
			fileName.append( baseName );
			// Test if files exists
			if ( QFile::exists( QString( "%1.h" ).arg( fileName ) ) || QFile::exists( QString( "%1.cpp" ).arg( fileName ) ) )
				return false;
			//
			// Templates: pluginClass_header.txt & pluginClass_source.txt
			// Header
			QFile templateFile( ":/Templates/Templates/pluginClass_header.txt" );
			if ( !templateFile.open( QFile::ReadOnly | QFile::Text ) )
				return false;
			buffer = templateFile.readAll();
			buffer.replace( "FILENAME", QString( "%1_h" ).arg( baseName ).toUpper() );
			buffer.replace( "CLASSNAME", baseName );
			QFile file( QString( "%1.h" ).arg( fileName ) );
			if ( !file.open( QFile::WriteOnly | QFile::Text ) )
				return false;
			file.write( buffer.toAscii() );
			templateFile.close();
			file.close();
			// Source
			templateFile.setFileName( ":/Templates/Templates/pluginClass_source.txt" );
			if ( !templateFile.open( QFile::ReadOnly | QFile::Text ) )
				return false;
			ok = false;
			while ( !ok || include.isEmpty() )
				include = QInputDialog::getItem( this, tr( "Choose a file..." ), tr( "Choose the plugin file header to use :" ), UIMain::self()->currentProject()->stringListValue( "HEADERS" ), 0, true, &ok );
			buffer = templateFile.readAll();
			buffer.replace( "INCLUDEPLUGIN", include );
			buffer.replace( "CLASSNAMEPLUGIN", QFileInfo( include ).baseName() );
			buffer.replace( "FILENAME", QString( "%1.h" ).arg( baseName ) );
			buffer.replace( "CLASSNAME", baseName );
			file.setFileName( QString( "%1.cpp" ).arg( fileName ) );
			if ( !file.open( QFile::WriteOnly | QFile::Text ) )
				return false;
			file.write( buffer.toAscii() );
			templateFile.close();
			file.close();
			// END
			if ( cbFilesAddToProject->isChecked() )
			{
				project = UIMain::self()->currentProject();
				project->addStringListValue( "HEADERS", platform, QString( "%1.h" ).arg( fileName ) );
				project->addStringListValue( "SOURCES", platform, QString( "%1.cpp" ).arg( fileName ) );
			}
			UIMain::self()->createWindow( fileName.append( ".h" ), pHeader, project );
			break;
		}
	case 6:
		{
			// Collection PlugIn Class
			fileName = leFilesLocation->text();
			if (!fileName.isEmpty() && !fileName.endsWith( "/" ) )
				fileName.append( "/" );
			if ( !fileName.isEmpty() )
			{
				QDir d( fileName );
				if ( !d.mkpath( d.path() ) )
					return false;
			}
			fileName.append( leFilesFileName->text() );
			suffix = QFileInfo( fileName ).suffix();
			if ( !QMakeProject::sourceExtension.contains( suffix, Qt::CaseInsensitive ) )
				fileName.append( ".cpp" );
			if ( QFile::exists( fileName ) )
				return false;
			QFile templateFile( ":/Templates/Templates/collectionPluginClass_source.txt" );
			if ( !templateFile.open( QFile::ReadOnly | QFile::Text ) )
				return false;
			QFile file( fileName );
			if ( !file.open( QFile::WriteOnly | QFile::Text ) )
				return false;
			file.write( templateFile.readAll().replace( "CLASSNAME", QFileInfo( fileName ).baseName().toAscii() ) );
			templateFile.close();
			file.close();
			if ( cbFilesAddToProject->isChecked() )
			{
				project = UIMain::self()->currentProject();
				project->addStringListValue( "SOURCES", platform, fileName );
			}
			UIMain::self()->createWindow( fileName, pSource, project );
			break;
		}
	}
	return true;
}
//
bool UINewFiles::templateAccepted()
{
	return false;
}
