/********************************************************************************************************
 * PROGRAM      : TextEditor
 * DATE - TIME  : lundi 01 mai 2006 - 14h54
 * AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
 * FILENAME     : ReplaceDialog.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "ReplaceDialog.h"
//
#include <QTextEdit>
#include <QTextCursor>
#include <QLineEdit>
#include <QApplication>
//
QPointer<ReplaceDialog> ReplaceDialog::mSelf = 0L;
//
ReplaceDialog* ReplaceDialog::self( QWidget* parent, Qt::WFlags f )
{
	if ( !mSelf )
		mSelf = new ReplaceDialog( parent, f );
	return mSelf;
}
//
void ReplaceDialog::tasks()
{
	// if not edit to work on, abort
	if ( !self()->workingEdit() )
		return;
	// clear label messages
	self()->lMessage->clear();
	// show it if not visible
	if ( !self()->isVisible() )
		self()->show();
	// set focus to cbSearch
	self()->cbSearch->setFocus();
	// if not empty search, then select it
	if ( !self()->cbSearch->currentText().isEmpty() )
		self()->cbSearch->lineEdit()->selectAll();
}
//
ReplaceDialog::ReplaceDialog( QWidget* parent, Qt::WFlags f )
	: QFrame( parent, f ), mWorkingEdit( 0 )
{
	setupUi( this );
	// Connections
	connect( cbSearch->lineEdit(), SIGNAL( returnPressed() ), this, SLOT( on_pbSearch_clicked() ) );
	connect( cbReplace->lineEdit(), SIGNAL( returnPressed() ), this, SLOT( on_pbReplace_clicked() ) );
}
//
void ReplaceDialog::setWorkingEdit( QTextEdit* e )
{
	if ( mWorkingEdit != e )
		mWorkingEdit = e;
}
//
void ReplaceDialog::setMode( Mode m )
{
	if ( mMode == m )
		return;
	mMode = m;
	emit modeChanged( mMode );
}
//
QTextEdit* ReplaceDialog::workingEdit()
{
	return mWorkingEdit;
}
//
ReplaceDialog::Mode ReplaceDialog::mode()
{
	return mMode;
}
//
bool ReplaceDialog::searchNext()
{
	// if no edit or search is empty abort
	if ( !mWorkingEdit || cbSearch->currentText().isEmpty() )
		return false;
	// clear label messages
	lMessage->clear();
	//
	QTextCursor searchCursor, positionCursor = mWorkingEdit->textCursor();
	QTextDocument::FindFlags findFlags = 0;
	// setting search options
	if ( cbMatchCase->isChecked() )
		findFlags |= QTextDocument::FindCaseSensitively;
	if ( cbWholeWord->isChecked() )
		findFlags |= QTextDocument::FindWholeWords;
	if ( cbSearchBackward->isChecked() )
		findFlags |= QTextDocument::FindBackward;
	// need replace position and decheck for next search
	if ( cbSearchFromStart->isChecked() )
	{
		positionCursor.setPosition( 0 );
		cbSearchFromStart->setChecked( false );
	}
	if ( !cbSearchSelection->isChecked() )
		searchCursor = mWorkingEdit->document()->find( cbSearch->currentText(), positionCursor, findFlags );
	else
	{
		qWarning( "Search in selection : not yet implemented (i don't know how to do it)" );
	}
	// if found
	if ( !searchCursor.isNull() )
	{
		QApplication::setActiveWindow( mWorkingEdit );
		mWorkingEdit->setTextCursor( searchCursor );
		return true;
	}
	// not found
	lMessage->setText( tr( "Text Not Found!" ) );
	return false;
}
//
bool ReplaceDialog::replaceNext()
{
	// if empty repalce string abort
	if ( cbReplace->currentText().isEmpty() )
		return false;
	// if found a match to replace
	if ( searchNext() )
	{
		// repalce text
		QTextCursor textCursor = mWorkingEdit->textCursor();
		textCursor.removeSelectedText();
		textCursor.insertText( cbReplace->currentText() );
		mWorkingEdit->setTextCursor( textCursor );
		return true;
	}
	// no match found
	return false;
}
//
void ReplaceDialog::replaceAll()
{
	int count = 0;
	bool replace = true;
	// while found match, replace
	while ( replace )
	{
		if ( replaceNext() )
			count++;
		else
			replace = false;
	}
	// inform the number of replace if > 0
	if ( count > 0 )
		lMessage->setText( QString( tr( "Replacing done, %1 occurences replace" ) ).arg( count ) );
}
//
void ReplaceDialog::on_pbSearch_clicked()
{
	searchNext();
}
//
void ReplaceDialog::on_pbReplace_clicked()
{
	replaceNext();
}
//
void ReplaceDialog::on_pbReplaceAll_clicked()
{
	replaceAll();
}
