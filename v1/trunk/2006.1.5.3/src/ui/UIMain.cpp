/********************************************************************************************************
* PROGRAM      : qt4ds
* DATE - TIME  : vendredi 06 janvier 2006 - 01h51
* AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
* FILENAME     : UIMain.cpp
* LICENSE      : GPL
* COMMENTARY   :
********************************************************************************************************/
#include "UIMain.h"
#include "main.h"
#include "Tools.h"
#include "Options.h"
#include "UIOptions.h"
#include "ReplaceDialog.h"
#include "MDIText.h"
#include "MDIClass.h"
#include "UINewFiles.h"
#include "UIAddExistingFiles.h"
#include "UIProjectSettings.h"
#include "UIToolsEdit.h"
#include "UIAbout.h"
//
#include <QCloseEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QActionGroup>
#include <QStyleFactory>
#include <QIcon>
#include <QErrorMessage>
#include <QScrollBar>
#include <QComboBox>
#include <QFontDatabase>
#include <QAssistantClient>
//
QPointer<UIMain> UIMain::_self = 0L;
//
UIMain* UIMain::self( QWidget* parent )
{
	if ( !_self )
		_self = new UIMain( parent );
	return _self;
}
//
UIMain::UIMain( QWidget* parent )
	: QMainWindow( parent ), mProject( 0 ), mInit( false ),
			mProcess( new QProcess( this ) ), cbFont( new QComboBox( this ) ),
			cbFontSize( new QComboBox( this ) ), mManual( new QAssistantClient( Options::qtBin( "assistant" ), this ) ),
			mAssistant( new QAssistantClient( Options::qtBin( "assistant" ), this ) )
{
	mProcess->setObjectName( "mProcess" );
	mProcess->setReadChannelMode( QProcess::MergedChannels );
	cbFont->setObjectName( "cbFont" );
	cbFontSize->setObjectName( "cbFontSize" );
	mAssistant->setObjectName( "mAssistant" );
	mManual->setObjectName( "mManual" );
	setupUi( this );
	// setting argument to manual
	mManual->setArguments( QStringList() << "-profile" << Options::documentationFile( "html/qt4ds-monkey.adp" ) );
	// workspace scrollbars
	workspace->setScrollBarsEnabled( true );
	// Adding Search Replace Dialog inside Messages Box
	twMessagesBox->addTab( ReplaceDialog::self(), QIcon( ":/Icons/Icons/tabsearch.png" ), tr( "Search Replace" ) );
	ReplaceDialog::self()->pbCancel->hide();
	connect( qApp, SIGNAL( focusChanged( QWidget*, QWidget* ) ), this, SLOT( focusChanged( QWidget*, QWidget* ) ) );
	// Adding buttons to status bar
	sbBar->addPermanentWidget( pbWidgetsBox );
	sbBar->addPermanentWidget( pbProjectsBox );
	sbBar->addPermanentWidget( pbMessagesBox );
	//
	QFont myFont;
	if ( !myFont.fromString( Options::self()->value( "Options/Editor/TextFont" ).toString() ) )
		myFont = qApp->font();
	// Font
	fontToolBar->addWidget( cbFont );
	cbFont->setEditable( true );
	QFontDatabase fdb;
	cbFont->addItems( fdb.families() );
	cbFont->setCurrentIndex( cbFont->findText( myFont.family() ) );
	// Font Size
	fontToolBar->addWidget( cbFontSize );
	cbFontSize->setEditable( true );
	foreach ( int size, fdb.standardSizes() )
		cbFontSize->addItem( QString::number( size ) );
	cbFontSize->setCurrentIndex( cbFontSize->findText (QString::number( myFont.pointSize() ) ) );
	// Adding a application background
	//workspace->setBackground( QBrush( QPixmap( ":/Icons/Icons/background.png" ) ) );
	// Setting corners
	setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
	// create action for styles
	agStyles = new QActionGroup( this );
	connect( agStyles, SIGNAL( triggered( QAction* ) ), this, SLOT( agStyles_triggered( QAction* ) ) );
	QAction* action;
	foreach ( QString style, QStyleFactory::keys() )
	{
		action = agStyles->addAction( style );
		action->setCheckable( true );
		if ( Options::self()->value( "Options/Style" ).toString() == style )
			action->setChecked( true );
	}
	menuViewStyle->addActions( agStyles->actions() );
	// init menu
	projectClosed( mProject );
}
//
void UIMain::showEvent( QShowEvent* )
{
	if ( !mInit )
	{
		mInit = true;
		menuDebugger->menuAction()->setVisible( false );
		pbWidgetsBox->setChecked( dwWidgetsBox->isVisible() );
		pbProjectsBox->setChecked( dwProjectsBox->isVisible() );
		pbMessagesBox->setChecked( dwMessagesBox->isVisible() );
		for ( int i = 0; i < MaxRecentFiles; i++ )
		{
			actionRecentlyOpenedFiles[ i ] = new QAction( this );
			actionRecentlyOpenedFiles[ i ]->setVisible( false );
			connect( actionRecentlyOpenedFiles[ i ], SIGNAL( triggered() ), this, SLOT( actionRecentlyOpenedFiles_triggered() ) );
			menuFilesRecentlyOpenedFiles->addAction( actionRecentlyOpenedFiles[ i ] );
		}
		updateRecentFiles();
		for ( int i = 0; i < MaxRecentProjects; i++ )
		{
			actionRecentlyOpenedProjects[ i ] = new QAction( this );
			actionRecentlyOpenedProjects[ i ]->setVisible( false );
			connect(actionRecentlyOpenedProjects[ i ], SIGNAL( triggered() ), this, SLOT( actionRecentlyOpenedProjects_triggered() ) );
			menuFilesRecentlyOpenedProjects->addAction( actionRecentlyOpenedProjects[ i ] );
		}
		updateRecentProjects();
		loadTools();
		Options::restoreState( this );
		showMaximized();
		if ( Options::self()->value( "Options/General/ShowQtWarnings", true ).toBool() )
			QErrorMessage::qtHandler();
		sbBar->showMessage( "Ready.", 5000 );
	}
}
//
void UIMain::closeEvent( QCloseEvent* )
{
	if ( mProject )
		delete mProject;
	QWidgetList widgetList = workspace->windowList();
	foreach ( QWidget* widget, widgetList )
		widget->close();
	delete mProcess;
	Options::saveState( this );
}
//
void UIMain::changeEvent( QEvent* e )
{
	if ( e->type() == QEvent::WindowTitleChange && windowTitle() != _PROGRAM_TITLE )
		setWindowTitle( _PROGRAM_TITLE );
}
//
QMakeProject* UIMain::currentProject()
{
	if ( !mProject ) // No Project
		return 0;
	//
	QTreeWidgetItem* item;
	if ( !( item = twProjectsFiles->currentItem() ) ) // No Selected Project, Return Opened Project
		return mProject;
	//
	if ( item->data( 0, QTreeWidgetItem::UserType +1 ).toInt() != pProject )
		while ( item->parent()->data( 0, QTreeWidgetItem::UserType +1 ).toInt() != pProject )
			item = item->parent();
	return QMakeProject::getProjectByFilePath( item->data( 0, QTreeWidgetItem::UserType +2 ).toString(), mProject );
}
//
QProcess* UIMain::process()
{
	return mProcess;
}
//
bool UIMain::startDetached( const QString& command )
{
	bool retour;
	teConsole->append( "************************************************************" );
	#ifdef Q_WS_WIN
	// This freeze the application under x11 if I already own a qprocess object and that I have already used it, why ?!
	teConsole->append( QString( "Starting detached ( QProcess ): %1" ).arg( command ) );
	retour = QProcess::startDetached( command  );
	#else
	teConsole->append( QString( "Starting detached ( system ): %1 &" ).arg( command ) );
	system( qPrintable( QString( "%1 &" ).arg( command ) ) );
	retour = true;
	#endif
			teConsole->append( "************************************************************" );
	teConsole->append( QString::null );
	return retour;
}
//
void UIMain::start( const QString& command, const QString& workingDirectory )
{
	if ( mProcess->state() != QProcess::NotRunning )
		return;
	teConsole->append( "************************************************************" );
	if( !workingDirectory.isEmpty() )
	{
		mProcess->setWorkingDirectory( workingDirectory );
		teConsole->append( QString( "From: %1" ).arg( workingDirectory ) );
	}
	teConsole->append( QString( "Executing: %1" ).arg( command ) );
	teConsole->append( "************************************************************" );
	teConsole->append( QString::null );
	mProcess->start( command );
}
//
void UIMain::waitProcess()
{
	while ( mProcess->state() != QProcess::NotRunning )
		qApp->processEvents();
}
//
void UIMain::setMenuEnabled( QMenu* menu, bool enabled )
{
	menu->setEnabled( enabled );
	foreach( QAction* action, menu->actions() )
	{
		action->setEnabled( enabled );
		if ( action->menu() )
			setMenuEnabled( action->menu(), enabled );
	}
}
//
void UIMain::on_workspace_windowActivated( QWidget* w )
{
	child_activated( qobject_cast<MDIChild*>( w ) );
}
//
void UIMain::child_activated( MDIChild* child )
{
	actionFileClose->setEnabled( false );
	actionFileSave->setEnabled( false );
	actionFileSaveAll->setEnabled( false );
	actionFileSaveAsTemplate->setEnabled( false );
	if ( child )
	{
		actionFileClose->setEnabled( true );
		actionFileSave->setEnabled( child->isModified() );
		actionFileSaveAll->setEnabled( true );
		actionFileSaveAsTemplate->setEnabled( true );
		// Setting correct tab index
		QString tabFilePath;
		for ( int i = 0; i < tabBar->count(); i++ )
		{
			tabFilePath = tabBar->tabData( i ).toString();
			if ( child->tabFilePath() == tabFilePath )
			{
				if ( tabBar->currentIndex() != i )
					tabBar->setCurrentIndex( i );
				break;
			}
		}
	}
}
//
void UIMain::on_mManual_error( const QString& message )
{
	teConsole->append( QString( "Manual: %1" ).arg( message ) );
}
//
void UIMain::on_mAssistant_error( const QString& message )
{
	teConsole->append( QString( "Assistant: %1" ).arg( message ) );
}
//
void UIMain::on_tabBar_currentChanged( int id )
{
	MDIChild* child;
	QString tabFilePath = tabBar->tabData( id ).toString();
	QWidgetList widgetList = workspace->windowList();
	for ( int i = 0; i < widgetList.count(); i++ )
	{
		child = qobject_cast<MDIChild*>( widgetList[ i ] );
		if ( child->tabFilePath() == tabFilePath )
		{
			if ( workspace->activeWindow() != child )
				workspace->setActiveWindow( child );
			break;
		}
	}
}
//
void UIMain::addTab( MDIChild* child )
{
	QString caption = child->windowTitle(), fileName = child->tabFilePath();
	bool exists = false;
	for ( int i = 0; i < tabBar->count(); i++ )
	{
		if ( tabBar->tabText( i ) == caption && tabBar->tabData( i ).toString() == fileName )
		{
			exists = true;
			break;
		}
	}
	if ( !exists )
	{
		int id = tabBar->addTab( caption );
		tabBar->setTabData( id, fileName );
		if ( child->project() )
			tabBar->setTabToolTip( id, child->project()->filePath() );
		else
			tabBar->setTabToolTip( id, child->tabFilePath() );
		tabBar->setCurrentIndex( id );
		connect( child, SIGNAL( removeTab( MDIChild* ) ), this, SLOT( removeTab( MDIChild* ) ) );
		connect( child, SIGNAL( activated( MDIChild* ) ), this, SLOT( child_activated( MDIChild* ) ) );
		connect( child, SIGNAL( modified( bool ) ), actionFileSave, SLOT( setEnabled( bool ) ) );
		connect( child, SIGNAL( replaceDialogRequested() ), this, SLOT( showReplaceDialog() ) );
		//
		workspace->addWindow( child );
		child->showMaximized();
	}
}
//
void UIMain::removeTab( MDIChild* child )
{
	QString caption = child->windowTitle(), fileName = child->tabFilePath();
	for ( int i = 0; i < tabBar->count(); i++ )
	{
		if ( tabBar->tabText(i) == caption && tabBar->tabData( i ).toString() == fileName )
		{
			tabBar->removeTab( i );
			break;
		}
	}
}
//
void UIMain::on_twProjectsFiles_itemDoubleClicked( QTreeWidgetItem* item, int )
{
	QMakeProject* project = currentProject();
	if ( !project || !item )
		return;
	createWindow( item->data( 0, QTreeWidgetItem::UserType +2 ).toString(), ( pFileType ) item->data( 0, QTreeWidgetItem::UserType +1 ).toInt(), project );
}
//
void UIMain::on_twProjectsFiles_customContextMenuRequested( const QPoint& pos )
{
	QTreeWidgetItem* item = twProjectsFiles->currentItem();
	if ( !item )
		return;
	//
	QMenu menu( this );
	menu.addMenu( menuProject );
	switch ( item->data( 0, QTreeWidgetItem::UserType +1 ).toInt() )
	{
		case pForm:
		case pHeader:
		case pSource:
		case pTranslation:
		case pText:
			menu.addSeparator();
			( menu.addAction( QIcon( ":/Icons/Icons/editremove.png" ), tr( "&Remove" ), this, SLOT( removeFromProject() ), tr( "Delete" ) ) )->setStatusTip( tr( "Remove the selected file from project" ) );
			( menu.addAction( QIcon( ":/Icons/Icons/editdelete.png" ), tr( "Remove and &Delete" ), this, SLOT( deleteFromProject() ), tr( "Shift+Delete" ) ) )->setStatusTip( tr( "Remove The selected file from project and delete it" ) );
			break;
	}
	menu.exec( twProjectsFiles->mapToGlobal( pos ) );
}
//
void UIMain::alert( const QString& message )
{
	QMessageBox::information( this, _INFORMATION, message );
}
//
void UIMain::focusChanged( QWidget* /* old */, QWidget* now )
{
	if ( qobject_cast<QTextEdit*>( now ) && now != teConsole )
		ReplaceDialog::self()->setWorkingEdit( qobject_cast<TextEditor*>( now ) );
}
//
void UIMain::showReplaceDialog()
{
	if ( twMessagesBox->currentWidget() != ReplaceDialog::self() )
		twMessagesBox->setCurrentWidget( ReplaceDialog::self() );
	ReplaceDialog::self()->tasks();
}
//
void UIMain::createWindow( const QString& fileName, pFileType type, QMakeProject* project )
{
	MDIChild* child;
	bool exists = false;
	QWidgetList widgetList = workspace->windowList();
	switch ( type )
	{
		case pForm:
		case pHeader:
		case pSource:
			if ( QFile::exists( fileName ) )
			{
				QString tabFilePath = QFileInfo( fileName ).path().append( "/%1" ).arg( QFileInfo( fileName ).baseName() );
				for ( int i = 0; i < widgetList.count(); i++ )
				{
					child = qobject_cast<MDIChild*>( widgetList.at( i ) );
					if ( child->type() == ctClass && child->tabFilePath() == tabFilePath )
					{
						exists = true;
						MDIClass* mc = qobject_cast<MDIClass*>( child );
						switch ( type )
						{
						case pForm:
							mc->on_wSpace_windowActivated( 0 );
							break;
						case pHeader:
							mc->on_wSpace_windowActivated( mc->teHeader );
							break;
						case pSource:
							mc->on_wSpace_windowActivated( mc->teSource );
							break;
						default:
							break;
						}
						if ( workspace->activeWindow() != child )
							workspace->setActiveWindow( mc );
						break;
					}
				}
				if ( !exists )
				{
					MDIClass* mc = new MDIClass( workspace );
					mc->setEditorFont( QFont( cbFont->currentText(), cbFontSize->currentText().toInt() ) );
					if ( project )
						mc->setProject( project );
					// checking case
					bool fileNameCaseIsLower = false;
					QString tmpString = QFileInfo( fileName ).suffix();
					if ( tmpString == tmpString.toLower() )
						fileNameCaseIsLower = true;
					// setting filenames
					// temporary hack for form files as Qt Designer integration is not done
					tmpString = fileNameCaseIsLower ? ".ui" : ".UI";
					mc->mFormFileName = tabFilePath + tmpString;
					//
					tmpString = fileNameCaseIsLower ? ".h" : ".H";
					mc->teHeader->setFilePath( tabFilePath + tmpString );
					mc->teHeader->openFile();
					//
					tmpString = fileNameCaseIsLower ? ".cpp" : ".CPP";
					mc->teSource->setFilePath( tabFilePath + tmpString );
					mc->teSource->openFile();
					//
					mc->setTabFilePath( tabFilePath );
					addTab( mc );
					tmpString = QFileInfo( fileName ).suffix();
					if ( QMakeProject::headerExtension.contains( tmpString ) )
						mc->on_wSpace_windowActivated( mc->teHeader );
					else if ( QMakeProject::sourceExtension.contains( tmpString ) )
						mc->on_wSpace_windowActivated( mc->teSource );
					else
						mc->on_wSpace_windowActivated( 0 );
				}
			}
			break;
		case pTranslation:
			if ( mProcess->state() == QProcess::NotRunning )
			{
				QMakeProject* project = currentProject();
				if ( Options::self()->value( "Options/General/SaveBeforeBuilding", true ).toBool() )
					project->save();
				start( project->lupdate(), project->absolutePath() );
				waitProcess();
				startDetached( Options::qtBin( "linguist" ).append( Tools::quoted( fileName ).prepend( " " ) ) );
			}
			break;
		case pFormItem:
		case pHeaderItem:
		case pSourceItem:
		case pTranslationItem:
		case pTextItem:
			break;
		default:
			for ( int i = 0; i < widgetList.count(); i++ )
			{
				child = qobject_cast<MDIChild*>( widgetList.at( i ) );
				if ( child->type() == ctText && child->tabFilePath() == fileName )
				{
					exists = true;
					if ( workspace->activeWindow() != child )
						workspace->setActiveWindow( child );
					break;
				}
			}
			if ( !exists )
			{
				MDIText* mt = new MDIText( workspace );
				mt->setEditorFont( QFont( cbFont->currentText(), cbFontSize->currentText().toInt() ) );
				if ( project )
					mt->setProject( project );
				mt->teContents->setFilePath( fileName );
				mt->teContents->openFile();
				mt->setTabFilePath( fileName );
				addTab( mt );
			}
			break;
		}
}
// Font & Size
void UIMain::on_cbFont_activated( const QString& font )
{
	QWidgetList widgetList = workspace->windowList();
	foreach ( QWidget* child, widgetList )
		qobject_cast<MDIChild*>( child )->setEditorFont( QFont( font, cbFontSize->currentText().toInt() ) );
}
//
void UIMain::on_cbFontSize_activated( const QString& size )
{
	QWidgetList widgetList = workspace->windowList();
	foreach ( QWidget* child, widgetList )
		qobject_cast<MDIChild*>( child )->setEditorFont( QFont( cbFont->currentText(), size.toInt() ) );
}
// File
bool UIMain::openFile( const QString& fileName, QMakeProject* project )
{
	if ( !fileName.isEmpty() )
	{
		createWindow( fileName, QMakeProject::getFileTypeFromFilePath( fileName ), project );
		// Recent Files
		if ( project )
			return true;
		QStringList files = Options::self()->value( "Options/General/RecentFiles" ).toStringList();
		files.removeAll( fileName );
		files.prepend( fileName );
		while ( files.size() > MaxRecentFiles )
			files.removeLast();
		Options::self()->setValue( "Options/General/RecentFiles", files );
		updateRecentFiles();
		return true;
	}
	return false;
}
//
void UIMain::saveAllOpenedWindow( bool closeAll, QMakeProject* project )
{
	MDIChild* child;
	QWidgetList widgetList = workspace->windowList();
	for( int i = widgetList.count() -1; i > -1; i-- )
	{
		child = qobject_cast<MDIChild*>( widgetList.at( i ) );
		if ( !project || child->project() == project )
		{
			if ( !closeAll )
				child->save( true );
			else
				child->close();
		}
	}
}
//
void UIMain::updateRecentFiles()
{
	QStringList files = Options::self()->value( "Options/General/RecentFiles" ).toStringList();
	for ( int i = 0; i < MaxRecentFiles; i++ )
	{
		if ( i < files.count() && !files[ i ].simplified().isEmpty() )
		{
			QString text = QString( "&%1 %2" ).arg( i +1 ).arg( QFileInfo( files[ i ] ).fileName() );
			actionRecentlyOpenedFiles[ i ]->setText( text );
			actionRecentlyOpenedFiles[ i ]->setData( files[ i ] );
			actionRecentlyOpenedFiles[ i ]->setStatusTip( files[ i ] );
			actionRecentlyOpenedFiles[i]->setVisible( true );
		}
		else
			actionRecentlyOpenedFiles[ i ]->setVisible( false );
	}
}
//
void UIMain::updateRecentProjects()
{
	QStringList files = Options::self()->value( "Options/General/RecentProjects" ).toStringList();
	for ( int i = 0; i < MaxRecentProjects; i++ )
	{
		if ( i < files.count() && !files[ i ].simplified().isEmpty() )
		{
			QString text = QString( "&%1 %2" ).arg( i +1 ).arg( QFileInfo( files[ i ] ).fileName() );
			actionRecentlyOpenedProjects[ i ]->setText( text );
			actionRecentlyOpenedProjects[ i ]->setData( files[ i ] );
			actionRecentlyOpenedProjects[ i ]->setStatusTip( files[ i ] );
			actionRecentlyOpenedProjects[ i ]->setVisible( true );
		}
		else
			actionRecentlyOpenedProjects[ i ]->setVisible( false );
	}
}
//
void UIMain::on_actionFileOpen_triggered()
{
	QString path = ".";
	QMakeProject* project = currentProject();
	if ( project )
		path = project->absolutePath();
	openFile( QFileDialog::getOpenFileName( this, tr( "Choose a file to open" ), path, QMakeProject::qtExtensions() ) );
}
//
void UIMain::on_actionFileClose_triggered()
{
	workspace->activeWindow()->close();
}
//
void UIMain::on_actionFileSave_triggered()
{
	MDIChild* child = qobject_cast<MDIChild*>( workspace->activeWindow() );
	if ( child )
		child->save( false );
}
//
void UIMain::on_actionFileSaveAll_triggered()
{
	saveAllOpenedWindow();
}
//
void UIMain::on_actionFileSaveAsTemplate_triggered()
{
	QMessageBox::information( this, _INFORMATION, tr( "Not yet implemented" ) );
}
//
void UIMain::on_actionFileQuickPrint_triggered()
{
	TextEditor* e = qobject_cast<TextEditor*>( focusWidget() );
	if ( e )
		e->printDocument();
}
void UIMain::on_actionFilePrint_triggered()
{
	TextEditor* e = qobject_cast<TextEditor*>( focusWidget() );
	if ( e )
		e->printDocument( false );
}
//
void UIMain::on_actionClearRecentsFiles_triggered()
{
	Options::self()->setValue( "Options/General/RecentFiles", QStringList() );
	updateRecentFiles();
}
//
void UIMain::actionRecentlyOpenedFiles_triggered()
{
	QAction* action = qobject_cast<QAction*>( sender() );
	if ( action )
		openFile( action->data().toString() );
}
//
void UIMain::on_actionClearRecentsProjects_triggered()
{
	Options::self()->setValue( "Options/General/RecentProjects", QStringList() );
	updateRecentProjects();
}
//
void UIMain::actionRecentlyOpenedProjects_triggered()
{
	QAction* action = qobject_cast<QAction*>( sender() );
	if ( action )
		openProject( action->data().toString() );
}
//
void UIMain::on_actionFileExit_triggered()
{
	close();
}
// Edit
void UIMain::on_actionEditUndo_triggered()
{
	QTextEdit* editor = qobject_cast<QTextEdit*>( focusWidget() );
	if ( editor )
		editor->document()->undo();
}
//
void UIMain::on_actionEditRedo_triggered()
{
	QTextEdit* editor = qobject_cast<QTextEdit*>( focusWidget() );
	if ( editor )
		editor->document()->redo();
}
//
void UIMain::on_actionEditCut_triggered()
{
	QTextEdit* editor = qobject_cast<QTextEdit*>( focusWidget() );
	if ( editor )
		editor->cut();
}
//
void UIMain::on_actionEditCopy_triggered()
{
	QTextEdit* editor = qobject_cast<QTextEdit*>( focusWidget() );
	if ( editor )
		editor->copy();
}
//
void UIMain::on_actionEditPaste_triggered()
{
	QTextEdit* editor = qobject_cast<QTextEdit*>( focusWidget() );
	if ( editor )
		editor->paste();
}
//
void UIMain::on_actionEditGotoLine_triggered()
{
	TextEditor* e = qobject_cast<TextEditor*>( focusWidget() );
	if ( e )
		e->gotoLineDialog();
}
//
void UIMain::on_actionEditOptions_triggered()
{
	UIOptions::self( this )->exec();
}
// View
void UIMain::agStyles_triggered( QAction* action )
{
	qApp->setStyle( action->text() );
	Options::self()->setValue( "Options/Style", action->text() );
}
// Project
void UIMain::saveProject( QMakeProject* project )
{
	if ( !project )
		return;
	if ( project->isModified() )
		project->save();
	saveAllOpenedWindow( false, project );
}
//
void UIMain::saveAllProject( QMakeProject* firstProject )
{
	if ( !firstProject )
		return;
	saveProject( firstProject );
	foreach ( QMakeProject* project, firstProject->subProjects() )
		saveAllProject( project );
}
//
bool UIMain::openProject( const QString& fileName )
{
	if ( !QFile::exists( fileName ) )
	{
		if ( !fileName.isEmpty() )
			QMessageBox::warning(this, _WARNING, tr( "This project file doesn't exists.\nie: '%1'" ).arg( fileName ) );
		return false;
	}
	else if ( ( mProject && QMessageBox::question( this, _QUESTION, tr( "Close project?" ), tr( "Yes" ), tr( "No" ) ) == 0 ) || !mProject )
	{
		if ( mProject )
			delete mProject;
		mProject = new QMakeProject( fileName, twProjectsFiles );
		// Connect Project Signals
		connect( mProject, SIGNAL( opened( QMakeProject* ) ), this, SLOT( projectOpened( QMakeProject* ) ) );
		connect( mProject, SIGNAL( closed( QMakeProject* ) ), this, SLOT( projectClosed( QMakeProject* ) ) );
		connect( mProject, SIGNAL( modified( QMakeProject*, bool ) ), this, SLOT( projectModified( QMakeProject*, bool ) ) );
		connect( mProject, SIGNAL( openFile( QMakeProject*, const QString& ) ), this, SLOT( projectOpenFile( QMakeProject*, const QString& ) ) );
		if ( mProject->open() )
			return true;
	}
	return false;
}
//
void UIMain::projectOpened( QMakeProject* project )
{
	if ( project == mProject )
	{
		// Update Recent Projects Actions
		QStringList files = Options::self()->value( "Options/General/RecentProjects" ).toStringList();
		files.removeAll( project->filePath() );
		files.prepend( project->filePath() );
		while ( files.size() > MaxRecentProjects )
			files.removeLast();
		Options::self()->setValue( "Options/General/RecentProjects", files );
		updateRecentProjects();
	}
	actionFileQuickPrint->setEnabled( true );
	actionFilePrint->setEnabled( true );
	actionEditUndo->setEnabled( true );
	actionEditRedo->setEnabled( true );
	actionEditCut->setEnabled( true );
	actionEditCopy->setEnabled( true );
	actionEditPaste->setEnabled( true );
	actionEditGotoLine->setEnabled( true );
	actionProjectNew->setEnabled( false );
	menuProjectSave->setEnabled( true );
	actionProjectSaveCurrent->setEnabled( false );
	actionProjectSaveAll->setEnabled( true );
	menuProjectClose->setEnabled( true );
	menuProjectShow->setEnabled( true );
	menuProjectAdd->setEnabled( true );
	actionProjectOptions->setEnabled( true );
	setMenuEnabled( menuBuild, true );
	setMenuEnabled( menuDebugger, true );
}
//
void UIMain::projectClosed( QMakeProject* project )
{
	if ( project )
		saveAllOpenedWindow( true, project );
	if ( project == mProject )
	{
		actionFileQuickPrint->setEnabled( false );
		actionFilePrint->setEnabled( false );
		actionEditUndo->setEnabled( false );
		actionEditRedo->setEnabled( false );
		actionEditCut->setEnabled( false );
		actionEditCopy->setEnabled( false );
		actionEditPaste->setEnabled( false );
		actionEditGotoLine->setEnabled( false );
		actionProjectNew->setEnabled( true );
		menuProjectSave->setEnabled( false );
		actionProjectSaveCurrent->setEnabled( false );
		actionProjectSaveAll->setEnabled( false );
		menuProjectClose->setEnabled( false );
		menuProjectShow->setEnabled( false );
		menuProjectAdd->setEnabled( false );
		actionProjectOptions->setEnabled( false );
		setMenuEnabled( menuBuild, false );
		setMenuEnabled( menuDebugger, false );
	}
}
//
void UIMain::projectModified( QMakeProject* project, bool modified )
{
	if ( project == currentProject() )
		actionProjectSaveCurrent->setEnabled( modified );
}
//
void UIMain::projectOpenFile( QMakeProject* project, const QString& fileName )
{
	openFile( fileName, project );
}
//
bool UIMain::removeFromProject()
{
	QTreeWidgetItem* item;
	if ( !( item = twProjectsFiles->currentItem() ) )
		return false;
	QStringList platforms = QStringList() << "win32" << "unix" << "mac";
	QString platform = item->parent()->text( 0 );
	if ( !platforms.contains( platform ) )
		platform = "(all)";
	switch ( twProjectsFiles->currentItem()->data( 0, QTreeWidgetItem::UserType +1 ).toInt() )
	{
		case pForm:
			currentProject()->removeStringListValue( "FORMS", platform, item->data( 0, QTreeWidgetItem::UserType +2 ).toString() );
			break;
		case pHeader:
			currentProject()->removeStringListValue( "HEADERS", platform, item->data( 0, QTreeWidgetItem::UserType +2 ).toString() );
			break;
		case pSource:
			currentProject()->removeStringListValue( "SOURCES", platform, item->data( 0, QTreeWidgetItem::UserType +2 ).toString() );
			break;
		case pTranslation:
			currentProject()->removeStringListValue( "TRANSLATIONS", platform, item->data( 0, QTreeWidgetItem::UserType +2 ).toString() );
			break;
		case pText:
			currentProject()->removeStringListValue( "TEXTS", platform, item->data( 0, QTreeWidgetItem::UserType +2 ).toString() );
			break;
		}
		return true;
}
//
bool UIMain::deleteFromProject()
{
	QTreeWidgetItem* item;
	if ( !( item = twProjectsFiles->currentItem() ) )
		return false;
	QString fileName = item->data( 0, QTreeWidgetItem::UserType +2 ).toString();
	if ( removeFromProject() )
		return QFile::remove( fileName );
	return false;
}
//
void UIMain::on_actionProjectNew_triggered()
{
	UINewFiles::self( this, UINewFiles::ftProject )->exec();
}
//
void UIMain::on_actionProjectOpen_triggered()
{
	openProject( QFileDialog::getOpenFileName( this, tr( "Choose a Qt Project to open" ), ".", "Qt Project (*.pro *.PRO)" ) );
}
//
void UIMain::on_actionProjectSaveCurrent_triggered()
{
	saveProject( currentProject() );
}
//
void UIMain::on_actionProjectSaveAll_triggered()
{
	saveAllProject( currentProject() );
}
//
void UIMain::on_actionProjectCloseCurrent_triggered()
{
	delete currentProject();
}
//
void UIMain::on_actionProjectCloseAll_triggered()
{
	delete mProject;
}
//
void UIMain::on_actionProjectShowSource_triggered()
{
	QMakeProject* project = currentProject();
	createWindow( project->filePath(), pOther, project );
}
//
void UIMain::on_actionProjectShowToDo_triggered()
{
	QMakeProject* project = currentProject();
	createWindow( project->absolutePath().append( "/ToDo.txt" ), pOther, project );
}
//
void UIMain::on_actionProjectShowChanges_triggered()
{
	QMakeProject* project = currentProject();
	createWindow( project->absolutePath().append( "/Changes.txt" ), pOther, project );
}
//
void UIMain::on_actionProjectAddNewForm_triggered()
{
	startDetached( Options::qtBin( "designer" ) );
}
//
void UIMain::on_actionProjectAddNewFiles_triggered()
{
	UINewFiles::self( this, UINewFiles::ftFile )->exec();
}
//
void UIMain::on_actionProjectAddNewTemplates_triggered()
{
	UINewFiles::self( this, UINewFiles::ftTemplate )->exec();
}
//
void UIMain::on_actionProjectAddExistingFiles_triggered()
{
	UIAddExistingFiles::self( currentProject(), this )->exec();
}
//
void UIMain::on_actionProjectOptions_triggered()
{
	UIProjectSettings::self( currentProject(), this )->exec();
}
// Build
void UIMain::on_mProcess_started()
{
	if ( mProject )
	{
		if ( twMessagesBox->currentWidget() != teConsole )
			twMessagesBox->setCurrentWidget( teConsole );
		setMenuEnabled( menuBuild, false );
		setMenuEnabled( menuDebugger, false );
	}
}
//
void UIMain::on_mProcess_finished( int exitCode, QProcess::ExitStatus exitStatus )
{
	if ( mProject )
	{
		if ( twMessagesBox->currentWidget() != teConsole )
			twMessagesBox->setCurrentWidget( teConsole );
		setMenuEnabled( menuBuild, true );
		setMenuEnabled( menuDebugger, true );
	}
	teConsole->append( "************************************************************" );
	teConsole->append( QString( tr( "Processus terminated, Exit Code : %1, Exit Status : %2" ) ).arg( exitCode ).arg( exitStatus == QProcess::NormalExit ? tr( "Normal" ) : tr( "Crash" ) ) );
	teConsole->append( "************************************************************" );
}
//
void UIMain::on_mProcess_readyRead()
{
	teConsole->setPlainText( teConsole->toPlainText().append( QString::fromLocal8Bit( mProcess->readAll() ) ) );
	teConsole->verticalScrollBar()->setValue( teConsole->verticalScrollBar()->maximum() );
}
//
void UIMain::on_actionBuildBuildCurrent_triggered()
{
	QMakeProject* project = currentProject();
	if ( Options::self()->value( "Options/General/SaveBeforeBuilding", true ).toBool() )
	{
		if ( project != mProject )
			saveProject( project );
		else
			saveAllProject( project );
	}
	teConsole->clear();
	start( project->qmake(), project->absolutePath() );
	waitProcess();
	start( project->make(), project->absolutePath() );
}
//
void UIMain::on_actionBuildBuildAll_triggered()
{
	QMakeProject* project = mProject;
	if ( Options::self()->value( "Options/General/SaveBeforeBuilding", true ).toBool() )
		saveAllProject( project );
	teConsole->clear();
	start( project->qmake(), project->absolutePath() );
	waitProcess();
	start( project->make(), project->absolutePath() );
}
//
void UIMain::on_actionBuildRebuildCurrent_triggered()
{
	QMakeProject* project = currentProject();
	if ( Options::self()->value( "Options/General/SaveBeforeBuilding", true ).toBool() )
	{
		if ( project != mProject )
			saveProject( project );
		else
			saveAllProject( project );
	}
	teConsole->clear();
	start( project->makeClean(), project->absolutePath() );
	waitProcess();
	start( project->qmake(), project->absolutePath() );
	waitProcess();
	start( project->make(), project->absolutePath() );
}
//
void UIMain::on_actionBuildRebuildAll_triggered()
{
	QMakeProject* project = mProject;
	if ( Options::self()->value( "Options/General/SaveBeforeBuilding", true ).toBool() )
		saveAllProject( project );
	teConsole->clear();
	start( project->makeClean(), project->absolutePath() );
	waitProcess();
	start( project->qmake(), project->absolutePath() );
	waitProcess();
	start( project->make(), project->absolutePath() );
}
//
void UIMain::on_actionBuildCleanCurrent_triggered()
{
	QMakeProject* project = currentProject();
	if ( Options::self()->value( "Options/General/SaveBeforeBuilding", true ).toBool() )
	{
		if ( project != mProject )
			saveProject( project );
		else
			saveAllProject( project );
	}
	teConsole->clear();
	start( project->makeClean(), project->absolutePath() );
}
//
void UIMain::on_actionBuildCleanAll_triggered()
{
	QMakeProject* project = mProject;
	if ( Options::self()->value( "Options/General/SaveBeforeBuilding", true ).toBool() )
		saveAllProject( project );
	teConsole->clear();
	start( project->makeClean(), project->absolutePath() );
}
//
void UIMain::on_actionBuildBuildAndExecute_triggered()
{
	on_actionBuildBuildCurrent_triggered();
	on_actionBuildExecute_triggered();
}
//
void UIMain::on_actionBuildExecute_triggered()
{
	QMakeProject* project = currentProject();
	QString command = project->execute();
	bool ok;
	QString args = QInputDialog::getText( this, QString( tr( "Executing %1..." ) ).arg( project->name() ), tr( "Enter command line" ), QLineEdit::Normal, QString::null, &ok );
	if ( ok )
	{
		if ( !args.isEmpty() )
			command.append( args.prepend( " " ) );
		startDetached( command );
	}
}
//
void UIMain::on_actionBuildLupdate_triggered()
{
	QMakeProject* project = currentProject();
	if ( Options::self()->value( "Options/General/SaveBeforeBuilding", true ).toBool() )
		saveProject( project );
	teConsole->clear();
	start( project->lupdate(), project->absolutePath() );
}
//
void UIMain::on_actionBuildLrelease_triggered()
{
	QMakeProject* project = currentProject();
	if ( Options::self()->value( "Options/General/SaveBeforeBuilding", true ).toBool() )
		saveProject( project );
	teConsole->clear();
	start( project->lrelease(), project->absolutePath() );
}
// Tools
void UIMain::loadTools()
{
	QList<QAction*> actions = menuTools->actions();
	for ( int i = 2; i < actions.count(); i++ )
		delete actions[i];
	//
	QAction* action;
	QSettings* settings = Options::self();
	int size = settings->beginReadArray( "Tools" );
	for ( int i = 0; i < size; i++ )
	{
		settings->setArrayIndex( i );
		action = menuTools->addAction( QIcon( settings->value( "Icon" ).toString() ), settings->value( "Caption" ).toString(), this, SLOT( actionTools_triggered() ) );
		action->setData( settings->value( "Command" ).toString() );
	}
	settings->endArray();
	delete settings;
}
//
void UIMain::on_actionToolsEdit_triggered()
{
	if ( UIToolsEdit::self( this )->exec() )
		loadTools();
}
//
void UIMain::actionTools_triggered()
{
	QAction* action = qobject_cast<QAction*>( sender() );
	if ( !action )
		return;
	QString command = action->data().toString();
	if ( command.contains( "%n", Qt::CaseInsensitive ) )
		command.replace( "%n", QDir::convertSeparators( currentProject()->name() ), Qt::CaseInsensitive );
	if ( command.contains( "%p", Qt::CaseInsensitive ) )
		command.replace( "%p", QDir::convertSeparators( currentProject()->absolutePath() ), Qt::CaseInsensitive );
	if ( command.contains( "%f", Qt::CaseInsensitive ) )
		command.replace( "%f", QDir::convertSeparators( currentProject()->filePath() ), Qt::CaseInsensitive );
	if ( twProjectsFiles->currentItem() )
	{
		QTreeWidgetItem* item = twProjectsFiles->currentItem();
		switch ( item->data( 0, QTreeWidgetItem::UserType +1 ).toInt() )
		{
			case pForm:
			case pHeader:
			case pSource:
			case pTranslation:
			case pResource:
			case pOther:
				if ( command.contains( "%d", Qt::CaseInsensitive ) )
						command.replace( "%d", QDir::convertSeparators( QFileInfo( item->data( 0, QTreeWidgetItem::UserType +2 ).toString() ).absolutePath() ), Qt::CaseInsensitive );
				if ( command.contains( "%g", Qt::CaseInsensitive ) )
						command.replace( "%g", QDir::convertSeparators( item->data( 0, QTreeWidgetItem::UserType +2 ).toString() ), Qt::CaseInsensitive );
				break;
			default:
				break;
			}
	}
	startDetached( command );
}
// Help
void UIMain::on_actionHelpManual_triggered()
{
	if ( !mManual->isOpen() )
		mManual->openAssistant();
	
	
}
void UIMain::on_actionHelpQtAssistant_triggered()
{
	if ( !mAssistant->isOpen() )
		mAssistant->openAssistant();
}
//
void UIMain::on_actionHelpAbout_triggered()
{
	UIAbout::self( this )->exec();
}
void UIMain::on_actionHelpAboutQt_triggered()
{
	qApp->aboutQt();
}
