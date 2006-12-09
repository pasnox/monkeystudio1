/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 30 janvier 2006 - 17h48
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : Options.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "Options.h"
#include "main.h"
#include "Tools.h"
#include "UIMain.h"
//
#include <QDir>
#include <QMainWindow>
//
QStringList Options::mLanguages = QStringList()
	<< "C"
	<< "Abkhazian"
	<< "Afan"
	<< "Afar"
	<< "Afrikaans"
	<< "Albanian"
	<< "Amharic"
	<< "Arabic"
	<< "Armenian"
	<< "Assamese"
	<< "Aymara"
	<< "Azerbaijani"
	<< "Bashkir"
	<< "Basque"
	<< "Bengali"
	<< "Bhutani"
	<< "Bihari"
	<< "Bislama"
	<< "Breton"
	<< "Bulgarian"
	<< "Burmese"
	<< "Byelorussian"
	<< "Cambodian"
	<< "Catalan"
	<< "Chinese"
	<< "Corsican"
	<< "Croatian"
	<< "Czech"
	<< "Danish"
	<< "Dutch"
	<< "English"
	<< "Esperanto"
	<< "Estonian"
	<< "Faroese"
	<< "FijiLanguage"
	<< "Finnish"
	<< "French"
	<< "Frisian"
	<< "Gaelic"
	<< "Galician"
	<< "Georgian"
	<< "German"
	<< "Greek"
	<< "Greenlandic"
	<< "Guarani"
	<< "Gujarati"
	<< "Hausa"
	<< "Hebrew"
	<< "Hindi"
	<< "Hungarian"
	<< "Icelandic"
	<< "Indonesian"
	<< "Interlingua"
	<< "Interlingue"
	<< "Inuktitut"
	<< "Inupiak"
	<< "Irish"
	<< "Italian"
	<< "Japanese"
	<< "Javanese"
	<< "Kannada"
	<< "Kashmiri"
	<< "Kazakh"
	<< "Kinyarwanda"
	<< "Kirghiz"
	<< "Korean"
	<< "Kurdish"
	<< "Kurundi"
	<< "Laothian"
	<< "Latin"
	<< "Latvian"
	<< "Lingala"
	<< "Lithuanian"
	<< "Macedonian"
	<< "Malagasy"
	<< "Malay"
	<< "Malayalam"
	<< "Maltese"
	<< "Maori"
	<< "Marathi"
	<< "Moldavian"
	<< "Mongolian"
	<< "NauruLanguage"
	<< "Nepali"
	<< "Norwegian"
	<< "Nynorsk"
	<< "Occitan"
	<< "Oriya"
	<< "Pashto"
	<< "Persian"
	<< "Polish"
	<< "Portuguese"
	<< "Punjabi"
	<< "Quechua"
	<< "RhaetoRomance"
	<< "Romanian"
	<< "Russian"
	<< "Samoan"
	<< "Sangho"
	<< "Sanskrit"
	<< "Serbian"
	<< "SerboCroatian"
	<< "Sesotho"
	<< "Setswana"
	<< "Shona"
	<< "Sindhi"
	<< "Singhalese"
	<< "Siswati"
	<< "Slovak"
	<< "Slovenian"
	<< "Somali"
	<< "Spanish"
	<< "Sundanese"
	<< "Swahili"
	<< "Swedish"
	<< "Tagalog"
	<< "Tajik"
	<< "Tamil"
	<< "Tatar"
	<< "Telugu"
	<< "Thai"
	<< "Tibetan"
	<< "Tigrinya"
	<< "TongaLanguage"
	<< "Tsonga"
	<< "Turkish"
	<< "Turkmen"
	<< "Twi"
	<< "Uigur"
	<< "Ukrainian"
	<< "Urdu"
	<< "Uzbek"
	<< "Vietnamese"
	<< "Volapuk"
	<< "Welsh"
	<< "Wolof"
	<< "Xhosa"
	<< "Yiddish"
	<< "Yoruba"
	<< "Zhuang"
	<< "Zulu";
//
QPointer<Options> Options::mSelf = 0L;
static const QString mIniFilePath = QString( "%1/.%2/.%2.ini" ).arg( QDir::homePath(), _PROGRAM_NAME );
//
Options* Options::self()
{
	if ( !mSelf )
		mSelf = new Options();
	return mSelf;
}
//
Options::Options()
	: QSettings( mIniFilePath, QSettings::IniFormat )
{}
//
QString Options::mingwBin( const QString& fileName )
{
	if ( !self()->value( "Options/General/MingwPath" ).toString().isEmpty() )
		return Tools::quoted( QString( "%1/bin/%2" ).arg( self()->value( "Options/General/MingwPath" ).toString(), fileName ) );
	return Tools::quoted( fileName );
}
//
QString Options::qtBin( const QString& fileName )
{
	QString filePath;
	if ( fileName.isEmpty() )
		return QString::null;
	else
	{
		filePath = self()->value( "Options/General/QtPath" ).toString().append( "/bin/" ).append( fileName );
#if defined( Q_OS_MAC )
		//some tools, such as "lupdate" are normal files
		//others, such as "assistant" are osx application directories
		if ( !QFile::exists( filePath ) )
		{
			filePath.append( ".app" );
			QDir appDir( filePath );
			if ( appDir.exists() )       
				filePath.append( "/Contents/MacOS/" ).append( fileName );
		}
#endif
		if ( !QFile::exists( filePath ) )
			filePath = fileName;
	}
	return Tools::quoted( filePath );
/*
	if ( !self()->value( "Options/Path/qtPath" ).toString().isEmpty() )
		return Tools::quoted( QString( "%1/bin/%2" ).arg( self()->value( "Options/Path/qtPath" ).toString(), fileName ) );
	return Tools::quoted( fileName );
*/
}
//
QString Options::templateFile( const QString& fileName )
{
	if ( !self()->value( "Options/General/TemplatesPath" ).toString().isEmpty() )
		return Tools::quoted( QString( "%1/%2" ).arg( self()->value( "Options/General/TemplatesPath" ).toString(), fileName ) );
	return Tools::quoted( fileName );
}
//
QString Options::translationFile( const QString& translationName )
{
	if ( !self()->value( "Options/General/TranslationsPath" ).toString().isEmpty() )
		return QString( "%1/%2_%3" ).arg( self()->value( "Options/General/TranslationsPath" ).toString(), "qt4ds", translationName.toLower() );
	return QString( "%1_%2" ).arg( "qt4ds", translationName.toLower() );
}
//
QString Options::documentationFile( const QString& documentName )
{
	if ( !self()->value( "Options/General/DocumentationsPath" ).toString().isEmpty() )
		return QString( "%1/%2" ).arg( self()->value( "Options/General/DocumentationsPath" ).toString(), documentName );
	return documentName;
}
//
void Options::restoreState( QMainWindow* window )
{ window->restoreState( self()->value( "MainWindow/State" ).toByteArray() ); }
//
void Options::saveState(QMainWindow* window)
{ self()->setValue( "MainWindow/State", window->saveState() ); }
//
QStringList Options::languages()
{ return mLanguages; }
//
void Options::setDefaultOptions()
{
	// setting defaults options
	self()->beginGroup( "Options" );
		// First Time Setting
		//self()->setValue( "FirstTimeSetting", true );
		self()->beginGroup( "General" );
			// Project
			self()->setValue( "ReloadLastProject", true );
			self()->setValue( "ReloadLastProjectFiles", true );
			self()->setValue( "SaveBeforeBuilding", true );
			self()->setValue( "ShowHeaderInformations", true );
			self()->setValue( "CreateBackup", false );
			// Path
			self()->setValue( "MingwPath", "" );
			self()->setValue( "QtPath", "" );
			self()->setValue( "TranslationsPath", "translations" );
			self()->setValue( "DocumentationsPath", "documentation" );
			self()->setValue( "TemplatesPath", "templates" );
			self()->setValue( "PdfViewer", "" );
			self()->setValue( "WebBrowser", "" );
			// Others
			self()->setValue( "ShowQtWarnings", true );
			//
			//self()->setValue( "RecentFiles", QStringList() );
			//self()->setValue( "RecentProjects", QStringList() );
		self()->endGroup();
		self()->beginGroup( "Editor" );
			// Editor
			self()->setValue( "TextFont", qApp->font().toString() );
			self()->setValue( "TextForegroundColor", "#000000" );
			self()->setValue( "TextSelectionColor", "#aaaaff" );
			self()->setValue( "TextBackgroundColor", "#ffffff" );
			self()->setValue( "TextTabSpacing", 4 );
			self()->setValue( "TextScrollLines", 10 );
			self()->setValue( "LimitLine", 130 );
			self()->setValue( "LimitLineColor", "#ff0000" );
			self()->setValue( "CurrentLineColor", "#eef6ff" );			
			self()->setValue( "SourceShowingMethod", 0 );
			self()->setValue( "SourceIndentationMethod", 0 );
			self()->setValue( "PrintingColor", false );
			self()->setValue( "PrintingLineNumbers", false );
			self()->setValue( "HighlightCurrentLine", true );
			self()->setValue( "ActivateAutoCompletion", true );
			self()->setValue( "ActivateParentheseMatcher", true );
			// Color Scheme
			QString myFont = qApp->font().toString();
			self()->setValue( "Number", QStringList() << myFont << "" << QColor( Qt::blue ).name() );
			self()->setValue( "String", QStringList() << myFont << "" << QColor( Qt::darkGreen ).name() );
			self()->setValue( "Type", QStringList() << myFont << "" << QColor( Qt::darkMagenta ).name() );
			self()->setValue( "Keyword", QStringList() << myFont << "" << QColor( Qt::darkYellow ).name() );
			self()->setValue( "Label", QStringList() << myFont << "" << QColor( Qt::darkRed ).name() );
			self()->setValue( "Comment", QStringList() << "times, 10, -1, 5, 50, 1, 0, 0, 0, 0" << "" << QColor( Qt::red ).name() );
			self()->setValue( "Preprocessor", QStringList() << myFont << "" << QColor( Qt::darkBlue ).name() );
		self()->endGroup();
	self()->endGroup();
}
