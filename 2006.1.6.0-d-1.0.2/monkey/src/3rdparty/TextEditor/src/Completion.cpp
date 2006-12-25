/********************************************************************************************************
 * PROGRAM      : TextEditor
 * DATE - TIME  : lundi 01 mai 2006 - 12h02
 * AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
 * FILENAME     : Completion.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "Completion.h"
#include "TextEditor.h"
#include "QMakeProject.h"
#include "InitCompletion.h"
//
#include <QVariant>
#include <QAction>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QTextCursor>
//
QPointer<Completion> Completion::mSelf = 0L;
//
Completion* Completion::self( QWidget* parent, Qt::WFlags f )
{
	if ( !mSelf )
		mSelf = new Completion( parent, f );
	return mSelf;
}
//
Completion::Completion( QWidget* parent, Qt::WFlags f )
	: QFrame( parent )
{
	// variables
	actionShowVariables = new QAction( this );
	actionShowVariables->setObjectName( "actionShowVariables" );
	actionShowVariables->setCheckable( true );
	actionShowVariables->setChecked( true );
	actionShowVariables->setText( tr( "Show Variables" ) );
	actionShowVariables->setIcon( QIcon( ":/Icons/Icons/completionvariables.png" ) );
	// members
	actionShowMembers = new QAction( this );
	actionShowMembers->setObjectName( "actionShowMembers" );
	actionShowMembers->setCheckable( true );
	actionShowMembers->setChecked( true );
	actionShowMembers->setText( tr( "Show Members" ) );
	actionShowMembers->setIcon( QIcon( ":/Icons/Icons/completionmembers.png" ) );
	// slots
	actionShowSlots = new QAction( this );
	actionShowSlots->setObjectName( "actionShowSlots" );
	actionShowSlots->setCheckable( true );
	actionShowSlots->setChecked( true );
	actionShowSlots->setText( tr( "Show Slots" ) );
	actionShowSlots->setIcon( QIcon( ":/Icons/Icons/completionslots.png" ) );
	// signals
	actionShowSignals = new QAction( this );
	actionShowSignals->setObjectName( "actionShowSignals" );
	actionShowSignals->setCheckable( true );
	actionShowSignals->setChecked( true );
	actionShowSignals->setText( tr( "Show Signals" ) );
	actionShowSignals->setIcon( QIcon( ":/Icons/Icons/completionsignals.png" ) );
	// others
	actionShowOthers = new QAction( this );
	actionShowOthers->setObjectName( "actionShowOthers" );
	actionShowOthers->setCheckable( true );
	actionShowOthers->setChecked( true );
	actionShowOthers->setText( tr( "Show Others" ) );
	actionShowOthers->setIcon( QIcon( ":/Icons/Icons/completionothers.png" ) );
	// frame
	setObjectName( "Completion" );
	setWindowTitle( "Completion" );
	setWindowFlags( f );
	setFrameShape( QFrame::StyledPanel );
	resize( QSize( 400, 150 ).expandedTo( minimumSizeHint() ) );
	// layout
	vboxLayout = new QVBoxLayout( this );
	vboxLayout->setSpacing( 0 );
	vboxLayout->setMargin( 0 );
	vboxLayout->setObjectName( "vboxLayout" );
	// toolbar
	tbViews = new QToolBar( this );
	tbViews->setObjectName( "tbViews" );
	tbViews->setCursor( QCursor( static_cast<Qt::CursorShape>( 13 ) ) );
	tbViews->setOrientation( Qt::Horizontal );
	tbViews->setIconSize( QSize( 16, 16 ) );
	//
	vboxLayout->addWidget( tbViews );
	// add action to toolbar
	tbViews->addAction( actionShowVariables );
	tbViews->addAction( actionShowMembers );
	tbViews->addAction( actionShowSlots );
	tbViews->addAction( actionShowSignals );
	tbViews->addAction( actionShowOthers );
	// listwidget
	lwItems = new QListWidget( this );
	lwItems->setObjectName( "lwItems" );
	QSizePolicy sizePolicy( static_cast<QSizePolicy::Policy>( 7 ), static_cast<QSizePolicy::Policy>( 7 ) );
	sizePolicy.setHorizontalStretch( 0 );
	sizePolicy.setVerticalStretch( 1 );
	sizePolicy.setHeightForWidth( lwItems->sizePolicy().hasHeightForWidth() );
	lwItems->setSizePolicy( sizePolicy );
	lwItems->setAlternatingRowColors( true );
	//
	vboxLayout->addWidget( lwItems );
	// statusbar
	sbInformations = new QStatusBar( this );
	sbInformations->setObjectName( "sbInformations" );
	//
	vboxLayout->addWidget( sbInformations );
	// connections
	QMetaObject::connectSlotsByName( this );
	connect( actionShowVariables, SIGNAL( triggered() ), this, SLOT( refreshView() ) );
	connect( actionShowMembers, SIGNAL( triggered() ), this, SLOT( refreshView() ) );
	connect( actionShowSlots, SIGNAL( triggered() ), this, SLOT( refreshView() ) );
	connect( actionShowSignals, SIGNAL( triggered() ), this, SLOT( refreshView() ) );
	connect( actionShowOthers, SIGNAL( triggered() ), this, SLOT( refreshView() ) );
}
//
void Completion::mouseMoveEvent( QMouseEvent* e )
{
	move( e->globalPos() -mMouse );
	QFrame::mouseMoveEvent ( e );
}
//
void Completion::mousePressEvent ( QMouseEvent * e )
{
	setMouse( e->pos() );
	QFrame::mousePressEvent( e );
}
//
const QPoint Completion::mouse() const
{
	return mMouse;
}
//
void Completion::setMouse( const QPoint & point )
{
	if ( mMouse != point )
		mMouse = point;
}
//
void Completion::refreshView()
{
	QListWidgetItem* item;
	for ( int i = 0; i < lwItems->count(); i++ )
	{
		item = lwItems->item( i );
		if ( item->type() == itVariables )
			lwItems->setItemHidden( item, !actionShowVariables->isChecked() );
		else if ( item->type() == itMembers )
			lwItems->setItemHidden( item, !actionShowMembers->isChecked() );
		else if ( item->type() == itSlots )
			lwItems->setItemHidden( item, !actionShowSlots->isChecked() );
		else if ( item->type() == itSignals )
			lwItems->setItemHidden( item, !actionShowSignals->isChecked() );
		else if ( item->type() == itOthers )
			lwItems->setItemHidden( item, !actionShowOthers->isChecked() );
	}
}
//
void Completion::on_lwItems_itemDoubleClicked( QListWidgetItem* item )
{
	if ( !item )
		return;
	QTextCursor cursor( mEditor->textCursor() );
	int i = cursor.position();
	cursor.movePosition( QTextCursor::StartOfWord );
	cursor.movePosition( QTextCursor::EndOfWord, QTextCursor::KeepAnchor );
	QString s = cursor.selectedText().trimmed();
	cursor.setPosition( i );
	cursor.setPosition( i -s.length(), QTextCursor::KeepAnchor );
	cursor.removeSelectedText();
	cursor.insertText( item->text() );
	mEditor->setTextCursor( cursor );
	close();
}
//
QList<TextEditor*> Completion::editors()
{
	return mEditors;
}
//
void Completion::setEditors( const QList<TextEditor*>& editors )
{
	mEditors = editors;
}
//
void Completion::invokeCompletion( TextEditor* editor, QMakeProject* p )
{
	if ( !isVisible() )
	{
		mEditor = editor;
		setParent( mEditor, windowFlags() );
		QPoint position = mEditor->mapToGlobal( mEditor->cursorRect().topLeft() );
		position.ry() += mEditor->cursorRect().height();
		// Need a fix as when the last line is near the bottom screen coordinate, the window is unvisible
		move( position );
		if ( prepareCompletion( p ) )
		{
			emit beforeCompletionShow();
			show();
		}
	}
}
//
bool Completion::prepareCompletion( QMakeProject* p )
{
	/*
	normally, here you have all needed informations to create the items
	mEditor -> the editor which send teh request ( the item in wich we must add text on double clicked
	mEditors -> the editors on wich we must scan the buffers
	First, clear items,
	Secondly, creates items on parsing,
	Finally, refreshItemsView
	*/
	QListWidgetItem* item;
	 // clear current items
	lwItems->clear();
	// create some test items
	QList<Tag> TagList = p->completion()->startParse( mEditor->toPlainText().left( mEditor->textCursor().position() ) );
	foreach ( Tag t, TagList )
	{
		item = new QListWidgetItem( t.name, lwItems, t.isFunction ? itMembers : itVariables );
	}
	/*
	// Test items
	item = new QListWidgetItem( "X", lwItems, itVariables );
	item = new QListWidgetItem( "text()", lwItems, itMembers );
	item = new QListWidgetItem( "setText()", lwItems, itSlots );
	item = new QListWidgetItem( "textChanged()", lwItems, itSignals );
	item = new QListWidgetItem( "other()", lwItems, itOthers );
	*/
	// refresh View
	refreshView();
	return true;
}
