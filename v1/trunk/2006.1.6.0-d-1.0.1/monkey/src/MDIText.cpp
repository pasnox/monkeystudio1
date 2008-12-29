/********************************************************************************************************
		* PROGRAM      : qt4ds
		* DATE - TIME  : lundi 09 janvier 2006 - 13h30
		* AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
			* FILENAME     : MDIText.cpp
			* LICENSE      : GPL
			* COMMENTARY   :
********************************************************************************************************/
#include "MDIText.h"
//
#include <QHBoxLayout>
#include <QTextDocument>
#include <QIcon>
#include <QMessageBox>
#include <QFile>
//
MDIText::MDIText( QWidget* parent )
	: MDIChild( parent )
{
	setObjectName( "MDIText" );
	setType( ctText );
	setWindowIcon( QIcon::QIcon( ":/Icons/Icons/projectshowfile.png" ) );
	//
	QHBoxLayout* hboxLayout = new QHBoxLayout;
	hboxLayout->setObjectName( "hboxLayout" );
	hboxLayout->setSpacing( 0 );
	hboxLayout->setMargin( 0 );
	//
	teContents = new TextEditor( this );
	teContents->setObjectName( "teContents" );
	teContents->setFrameShape( QFrame::NoFrame );
	teContents->setFrameShadow( QFrame::Plain );
	teContents->setMidLineWidth( 1 );
	teContents->setDefaultComponents( true );
	teContents->setSyntaxHighlighter( 0 );
	teContents->setCompletion( 0 );
	setSettings( teContents );
	connect( teContents, SIGNAL( replaceDialogRequested() ), this, SIGNAL( replaceDialogRequested() ) );
	connect( teContents, SIGNAL( fileOpen( bool ) ), this, SLOT( fileOpened( bool ) ) );
	connect( teContents->document(), SIGNAL( modificationChanged( bool ) ), this, SIGNAL( modified( bool ) ) );
	//
	hboxLayout->addWidget( teContents );
	setLayout( hboxLayout );
	
	connect(teContents,SIGNAL(signalBreakpointAt(QString,int)), this, SIGNAL(signalBreakpointAt(QString,int)));
	
}
//
void MDIText::closeEvent( QCloseEvent* e )
{
	teContents->closeFile();
	MDIChild::closeEvent( e );
}
//
bool MDIText::isModified()
{
	return teContents->document()->isModified();
}
//
QString MDIText::currentFilePath()
{
	return teContents->filePath();
}
//
void MDIText::setEditorFont( const QFont& font )
{
	teContents->setFont( font );
}
//
void MDIText::save( bool )
{
	if ( !teContents->saveFile() )
		QMessageBox::information( this, _WARNING, QString( tr( "Can't save the file [%1]" ) ).arg( teContents->filePath() ) );
}
