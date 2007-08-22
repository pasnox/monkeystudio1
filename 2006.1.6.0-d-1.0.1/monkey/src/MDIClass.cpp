/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 09 janvier 2006 - 13h29
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : MDIClass.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "MDIClass.h"
#include "UISubclassWizard.h"
#include "Completion.h"
#include "Options.h"
#include "UIMain.h"
#include "Tools.h"
//
#include <QVBoxLayout>
#include <QToolBar>
#include <QActionGroup>
#include <QAction>
#include <QWorkspace>
#include <QTextDocument>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QFileInfo>
//
MDIClass::MDIClass( QWidget* parent )
	: MDIChild( parent )
{
	setObjectName( "MDIClass" );
	setType( ctClass );
	setWindowIcon( QIcon( ":/Icons/Icons/projectshowfile.png" ) );
	//
	QVBoxLayout* vboxLayout = new QVBoxLayout( this );
	vboxLayout->setObjectName( "vboxLayout" );
	vboxLayout->setSpacing( 0 );
	vboxLayout->setMargin( 0 );
	// Toolbar
	QToolBar* tb = new QToolBar( tr( "Files" ), this );
	tb->setFixedHeight( tb->height() );
	tb->setIconSize( QSize(16, 16 ) );
	// Action Group
	aGroup = new QActionGroup( this );
	aGroup->setObjectName( "aGroup" );
	// Actions
	actionForm = new QAction( QIcon( ":/Icons/Icons/form.png" ), tr( "Form" ), this );
	actionForm->setCheckable( true );
	aGroup->addAction( actionForm );
	actionHeader = new QAction( QIcon( ":/Icons/Icons/h.png" ), tr( "Header" ), this );
	actionHeader->setCheckable( true );
	aGroup->addAction( actionHeader );
	actionSource = new QAction( QIcon( ":/Icons/Icons/cpp.png" ), tr( "Source" ), this );
	actionSource->setCheckable( true );
	aGroup->addAction( actionSource );
	tb->addActions( aGroup->actions() );
	vboxLayout->addWidget( tb );
	// Workspace
	wSpace = new QWorkspace( this );
	wSpace->setObjectName( "wSpace" );
	vboxLayout->addWidget( wSpace );
	// Form
	// Header
	teHeader = new TextEditor( this );
	teHeader->setObjectName( "teHeader" );
	teHeader->setFrameShape( QFrame::NoFrame );
	teHeader->setFrameShadow( QFrame::Plain );
	teHeader->setMidLineWidth( 1 );
	teHeader->setDefaultComponents( true );
	teHeader->setWindowIcon( actionHeader->icon() );
	setSettings( teHeader );
	connect( teHeader, SIGNAL( replaceDialogRequested() ), this, SIGNAL( replaceDialogRequested() ) );
	connect( teHeader->completion(), SIGNAL( beforeCompletionShow() ), this, SLOT( beforeCompletionShow() ) );
	connect( teHeader->document(), SIGNAL( modificationChanged( bool ) ), this, SIGNAL( modified( bool ) ) );
	connect( teHeader, SIGNAL( fileOpen( bool ) ), this, SLOT( fileOpened( bool ) ) );
	connect( teHeader, SIGNAL( completionRequested( Completion*, TextEditor* ) ), this, SLOT( completionRequested( Completion*, TextEditor* ) ) );
	wSpace->addWindow( teHeader, Qt::WindowTitleHint );
	// Source
	teSource = new TextEditor( this );
	teSource->setObjectName( "teSource" );
	teSource->setFrameShape( QFrame::NoFrame );
	teSource->setFrameShadow( QFrame::Plain );
	teSource->setMidLineWidth( 1 );
	teSource->setDefaultComponents( true );
	teSource->setWindowIcon( actionSource->icon() );
	setSettings( teSource );
	connect( teSource, SIGNAL( replaceDialogRequested() ), this, SIGNAL( replaceDialogRequested() ) );
	connect( teSource, SIGNAL( beforeCompletionShow() ), this, SLOT( beforeCompletionShow() ) );
	connect( teSource->document(), SIGNAL( modificationChanged( bool ) ), this, SIGNAL( modified( bool ) ) );
	connect( teSource, SIGNAL( fileOpen( bool ) ), this, SLOT( fileOpened( bool ) ) );
	connect( teSource, SIGNAL( completionRequested( Completion*, TextEditor* ) ), this, SLOT( completionRequested( Completion*, TextEditor* ) ) );
	wSpace->addWindow( teSource, Qt::WindowTitleHint );
	// Connections
	QMetaObject::connectSlotsByName( this );
}
//
void MDIClass::resizeEvent( QResizeEvent* )
{
	QSize wSpaceSize = wSpace->geometry().size();
	switch ( Options::self()->value( "Options/Editor/SourceShowingMethod", 0 ).toInt() )
	{
	case smMaximized:
		teHeader->parentWidget()->move( 0, 0 );
		teHeader->parentWidget()->resize( wSpaceSize );
		teSource->parentWidget()->move( 0, 0 );
		teSource->parentWidget()->resize( wSpaceSize );
		break;
	case smVertically:
		teHeader->parentWidget()->move( 0, 0 );
		teHeader->parentWidget()->resize( wSpaceSize.width() / 2, wSpaceSize.height() );
		teSource->parentWidget()->move( teHeader->parentWidget()->width(), 0 );
		teSource->parentWidget()->resize( teHeader->parentWidget()->size() );
		break;
	case smHorizontally:
		teHeader->parentWidget()->move( 0, 0 );
		teHeader->parentWidget()->resize( wSpaceSize.width(), wSpaceSize.height() / 2 );
		teSource->parentWidget()->move( 0, teHeader->parentWidget()->height() );
		teSource->parentWidget()->resize( teHeader->parentWidget()->size() );
		break;
	}
	// setting this, set impossible to resize/move childs as there have no minimize/maximize buttons
	teHeader->parentWidget()->showMaximized();
	teSource->parentWidget()->showMaximized();
}
//
void MDIClass::closeEvent( QCloseEvent* e )
{
	teHeader->closeFile();
	teSource->closeFile();

	// supprime la list des breakpoint en fonction du nom du fichier
	QStringList s = mProject->stringListValue( "DEBUGGER");
	for(int i=0; i<s.count(); i++)
	{
		QStringList l = s.at(i).split(":");
		if(l.at(0) == QFileInfo(teSource->filePath()).fileName())
			mProject->removeStringListValue( "DEBUGGER", "(all)",s.at(i)  );
	}	

	// genere l'eventuel list de breakpoint
	int numberBp = teSource->getAllBreakpoint().count();
	if(numberBp)
	{
		QString lineNumber;
		for(int i=0; i< numberBp; i++)
			lineNumber +=  ":" + QString::number(teSource->getAllBreakpoint().at(i));

		mProject->addStringListValue( "DEBUGGER","(all)",QFileInfo(teSource->filePath()).fileName()+ lineNumber);
	}
	 // ne fonctionne que si on ferme le fiechier avant la fermeture du project
	//BUG
	mProject->setModified( true );
	MDIChild::closeEvent( e );
}
//
void MDIClass::setEditorFont( const QFont& font )
{
	teHeader->setFont( font );
	teSource->setFont( font );
}
//
void MDIClass::on_aGroup_triggered( QAction* action )
{
	if ( action == actionForm )
		wSpace->setActiveWindow( 0 );
	else if ( action == actionHeader )
	{
		if ( wSpace->activeWindow() != teHeader )
			wSpace->setActiveWindow( teHeader );
	}
	else if ( action == actionSource )
	{
		if ( wSpace->activeWindow() != teSource )
			wSpace->setActiveWindow( teSource );
	}
}
//
void MDIClass::on_wSpace_windowActivated( QWidget* widget )
{
	if ( widget )
	{

		if ( widget == teHeader )
		{
			if ( aGroup->checkedAction() != actionHeader )
				actionHeader->trigger();
			if ( !teHeader->fileOpen() )
				teHeader->openFile();
		}
		else if ( widget == teSource )
		{
		if ( aGroup->checkedAction() != actionSource )
				actionSource->trigger();
			if ( !teSource->fileOpen() )
				teSource->openFile();
		}
		setWindowIcon( widget->windowIcon() );
	}
	else if ( wSpace->windowList().count() == 2 )
	{
		// temporary hack as Designer integration is not done
		if ( QFile::exists( mFormFileName ) && !QFile::exists( teHeader->filePath() ) && !QFile::exists( teSource->filePath() )  )
			subClass( mFormFileName );
		if ( QFile::exists( mFormFileName ) )
			UIMain::self()->startDetached( QString( "%1 %2" ).arg( Options::qtBin( "designer" ), Tools::quoted( mFormFileName ) ) );
	}
	emit activated( this );
}
//
void MDIClass::save( bool all )
{
	QString wm;
	if ( ( !teHeader->fileOpen() && teHeader->document()->isModified() ) ||
		( !teSource->fileOpen() && teSource->document()->isModified() ) )
	{
		QStringList wms = QStringList() << "(all)" << "win32" << "unix" << "mac";
		bool ok;
		wm = QInputDialog::getItem( this, tr( "OS..." ), tr( "One or more file(s) are requesting to save them,\nunder witch OS do you want to add them ?" ), wms, 0, false, &ok );
		if ( ok && !wm.isEmpty() )
			wm = wm;
		else
			wm = "(all)";
	}
	//
	if ( !all )
	{
		if ( wSpace->activeWindow() == teHeader )
		{
			if ( !teHeader->saveFile() )
				QMessageBox::information( this, _WARNING, QString( tr( "Can't save the header file\n[%1]" ) ).arg( teHeader->filePath() ) );
			else if ( project() && !wm.isEmpty() )
				project()->addStringListValue( "HEADERS", wm, teHeader->filePath() );
		}
		else if ( wSpace->activeWindow() == teSource )
		{
			if ( !teSource->saveFile() )
				QMessageBox::information( this, _WARNING, QString( tr( "Can't save the source file\n[%1]" ) ).arg( teSource->filePath() ) );
			else if ( project() && !wm.isEmpty() )
				project()->addStringListValue( "SOURCES", wm, teSource->filePath() );
		}
	}
	else
	{
		if ( !teHeader->saveFile() )
			QMessageBox::information( this, _WARNING, QString( tr( "Can't save the header file\n[%1]" ) ).arg( teHeader->filePath() ) );
		else if ( project() && !wm.isEmpty() )
			project()->addStringListValue( "HEADERS", wm, teHeader->filePath() );
		if ( !teSource->saveFile() )
			QMessageBox::information( this, _WARNING, QString( tr( "Can't save the source file\n[%1]" ) ).arg( teSource->filePath() ) );
		else if ( project() && !wm.isEmpty() )
			project()->addStringListValue( "SOURCES", wm, teSource->filePath() );
	}	
}
//
bool MDIClass::isModified()
{
	if ( wSpace->activeWindow() == teHeader )
		return teHeader->document()->isModified();
	else if ( wSpace->activeWindow() == teSource )
		return teSource->document()->isModified();
	return false;
}
//
void MDIClass::subClass( const QString& filePath )
{
	UISubclassWizard::self( filePath, this )->exec();
}
//
QString MDIClass::currentFilePath()
{
	if ( wSpace->activeWindow() == teHeader )
		return teHeader->filePath();
	else if ( wSpace->activeWindow() == teSource ) 
		return teSource->filePath();
	return mFormFileName;
}
//
void MDIClass::beforeCompletionShow()
{
/*
	TextEditor* editor = qobject_cast<TextEditor*>( sender() );
	if ( !editor || !editor->completion() )
		return;
	editor->completion()->setEditors( QList<TextEditor*>() << teHeader << teSource );
*/
}
