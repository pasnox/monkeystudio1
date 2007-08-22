/********************************************************************************************************
 * PROGRAM      : demo
 * DATE - TIME  : mercredi 03 mai 2006 - 21h17
 * AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
 * FILENAME     : UITextEditorDemo.cpp
 * LICENSE      : GPL
 * COMMENTARY   : TextEditor Demo
 ********************************************************************************************************/
#include "UITextEditorDemo.h"
//
#include <QtGui>
//
QPointer<UITextEditorDemo> UITextEditorDemo::_self = 0L;
//
UITextEditorDemo* UITextEditorDemo::self( QWidget* parent )
{
	if ( !_self )
		_self = new UITextEditorDemo( parent );
	return _self;
}
//
UITextEditorDemo::UITextEditorDemo( QWidget* parent )
	: QMainWindow( parent )
{
	setupUi( this );
	//
	sBar->addPermanentWidget( lOverwriteMode );
	sBar->addPermanentWidget( lCursorPosition );
	sBar->addPermanentWidget( lIndentMethod );
	//
	lOverwriteMode->setText( "INSERT" );
	if ( teEditor->overwriteMode() )
		lOverwriteMode->setText( "OVERWRITE" );
}
//
void UITextEditorDemo::on_teEditor_overwriteModeChanged( bool b )
{
	lOverwriteMode->setText( b ? "OVERWRITE" : "INSERT" );
}
//
void UITextEditorDemo::on_teEditor_cursorPositionChanged( int x, int y )
{
	lCursorPosition->setText( QString( "Colonne: %1, Ligne: %2" ).arg( x ).arg( y ) );
}
//
void UITextEditorDemo::on_actionOpen_triggered()
{
	if ( !teEditor->filePath().isEmpty() )
		teEditor->openFile();
}
//
void UITextEditorDemo::on_actionClose_triggered()
{
	if ( teEditor->fileOpen() )
		teEditor->closeFile();
}
//
void UITextEditorDemo::on_actionQuit_triggered()
{
	close();
}
//
void UITextEditorDemo::on_actionSetIndentMethod_triggered()
{
	bool ok;
	int i = QInputDialog::getInteger( this, "IndentMethod", "Indent Method :", (int)teEditor->indentMethod(), 0, 2, 1, &ok );
	if ( ok )
		teEditor->setIndentMethod( (TextEditor::IndentMethod)i );
	lIndentMethod->setText( QString( "Indent: %1" ).arg( i ) );
}
//
void UITextEditorDemo::on_actionSetHighlightCurrentLine_triggered()
{
	if ( QMessageBox::question( this, "HighlightCurrentLine", "Activate 'Highlight Current Line' ?", QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
		teEditor->setHighlightCurrentLine( true );
	else
		teEditor->setHighlightCurrentLine( false );
}
//
void UITextEditorDemo::on_actionSetCurrentLineColor_triggered()
{
	QColor c = QColorDialog::getColor( teEditor->currentLineColor(), this );
	if ( c.isValid() )
		teEditor->setCurrentLineColor( c );
}
//
void UITextEditorDemo::on_actionSetLimitLine_triggered()
{
	bool ok;
	int i = QInputDialog::getInteger( this, "LimitLine", "Limit Line :", teEditor->limitLine(), 0, 800, 1, &ok );
	if ( ok )
		teEditor->setLimitLine( i );
}
//
void UITextEditorDemo::on_actionSetLimitLineColor_triggered()
{
	QColor c = QColorDialog::getColor( teEditor->limitLineColor(), this );
	if ( c.isValid() )
		teEditor->setLimitLineColor( c );
}
//
void UITextEditorDemo::on_actionSetTabSize_triggered()
{
	bool ok;
	int i = QInputDialog::getInteger( this, "TabSize", "Tab Size :", teEditor->tabSize(), 0, 100, 1, &ok );
	if ( ok )
		teEditor->setTabSize( i );
}
//
void UITextEditorDemo::on_actionSetBackupFile_triggered()
{
	if ( QMessageBox::question( this, "BackupFile", "Activate 'Backup File' ?", QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
		teEditor->setBackupFile( true );
	else
		teEditor->setBackupFile( false );
}
//
void UITextEditorDemo::on_actionSetFilePath_triggered()
{
	QString s = QFileDialog::getOpenFileName( this, "Choose a file" );
	if ( !s.isNull() )
		teEditor->setFilePath( s );
}
//
void UITextEditorDemo::on_actionSetScrollLines_triggered()
{
	bool ok;
	int i = QInputDialog::getInteger( this, "ScrollLines", "Scroll Lines :", teEditor->scrollLines(), 0, 100, 1, &ok );
	if ( ok )
		teEditor->setScrollLines( i );
}
//
void UITextEditorDemo::on_actionSetSelectionColor_triggered()
{
	QColor c = QColorDialog::getColor( teEditor->selectionColor(), this );
	if ( c.isValid() )
		teEditor->setSelectionColor( c );
}
//
void UITextEditorDemo::on_actionSetBackgroundColor_triggered()
{
	QColor c = QColorDialog::getColor( teEditor->backgroundColor(), this );
	if ( c.isValid() )
		teEditor->setBackgroundColor( c );
}
//
void UITextEditorDemo::on_actionSetDefaultComponents_triggered()
{
	if ( QMessageBox::question( this, "DefaultComponents", "Activate 'Default Components' ?", QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
		teEditor->setDefaultComponents( true );
	else
		teEditor->setDefaultComponents( false );
}
