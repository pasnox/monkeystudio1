/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 30 janvier 2006 - 17h47
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UIAbout.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "UIAbout.h"
#include "main.h"
#include "Tools.h"
//
#include <QFile>
//
QPointer<UIAbout> UIAbout::_self = 0L;
//
UIAbout* UIAbout::self( QWidget* parent )
{
	if ( !_self )
		_self = new UIAbout( parent );
	return _self;
}
//
UIAbout::UIAbout( QWidget* parent )
	: QDialog( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
	//
	lInformations->setText( QString( "%1 v%2\n%3" ).arg( _PROGRAM_NAME, _PROGRAM_VERSION, _COPYRIGHTS ) );
	//setFixedSize( size() );
	QFile file;
	file.setFileName( ":/About/About/authors.html" );
	file.open( QFile::ReadOnly | QFile::Text );
	tbAuthors->setHtml( file.readAll() );
	file.close();
	file.setFileName( ":/About/About/testers.html" );
	file.open( QFile::ReadOnly | QFile::Text );
	tbTesters->setHtml( file.readAll() );
	file.close();
	file.setFileName( ":/About/About/greetings.html" );
	file.open( QFile::ReadOnly | QFile::Text );
	tbGreetings->setHtml( file.readAll() );
	file.close();
	//
	connect( tbAuthors, SIGNAL( anchorClicked( const QUrl& ) ), this, SLOT( anchorClicked( const QUrl& ) ) );
	connect( tbTesters, SIGNAL( anchorClicked( const QUrl& ) ), this, SLOT( anchorClicked( const QUrl& ) ) );
	connect( tbGreetings, SIGNAL( anchorClicked( const QUrl& ) ), this, SLOT( anchorClicked( const QUrl& ) ) );
}
//
void UIAbout::anchorClicked( const QUrl& url )
{
	QTextBrowser* browser = qobject_cast<QTextBrowser*>( sender() );
	browser->setHtml( browser->toHtml() );
	Tools::openUrl( url );
}
