/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 30 janvier 2006 - 17h47
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UIToolsEdit.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "UIToolsEdit.h"
#include "Options.h"
//
#include <QFileDialog>
#include <QWhatsThis>
//
QPointer<UIToolsEdit> UIToolsEdit::_self = 0L;
//
UIToolsEdit* UIToolsEdit::self( QWidget* parent )
{
	if ( !_self )
		_self = new UIToolsEdit( parent );
	return _self;
}
//
UIToolsEdit::UIToolsEdit( QWidget* parent )
	: QDialog( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
	//
	QListWidgetItem* item;
	QSettings* settings = Options::self();
	int size = settings->beginReadArray( "Tools" );
	for ( int i = 0; i < size; i++ )
	{
		settings->setArrayIndex( i );
		item = new QListWidgetItem( settings->value( "Caption" ).toString(), lwTools );
		item->setData( Qt::UserRole +1, settings->value( "Icon" ).toString() );
		item->setIcon( QIcon( item->data( Qt::UserRole +1 ).toString() ) );
		item->setData( Qt::UserRole +2, settings->value( "Command" ).toString() );
	}
	settings->endArray();
	delete settings;
}
//
void UIToolsEdit::on_lwTools_itemActivated( QListWidgetItem* item )
{
	if ( !item )
		return;
	leCaption->setText( item->text() );
	tbIcon->setIcon( item->icon() );
	lePath->setText( item->data( Qt::UserRole +2 ).toString() );
}
//
void UIToolsEdit::on_pbNew_clicked()
{
	lwTools->addItem( "new Tool" );
}
//
void UIToolsEdit::on_pbDelete_clicked()
{
	if ( lwTools->currentItem() )
		delete lwTools->currentItem();
}
//
void UIToolsEdit::on_pbUp_clicked()
{
	if ( !lwTools->currentItem() )
		return;
	int id = lwTools->currentRow();
	QListWidgetItem* item = lwTools->takeItem( id );
	lwTools->insertItem( id -1, item );
	lwTools->setCurrentRow( id -1 );
}
//
void UIToolsEdit::on_pbDown_clicked()
{
	if ( !lwTools->currentItem() )
		return;
	int id = lwTools->currentRow();
	QListWidgetItem* item = lwTools->takeItem( id );
	lwTools->insertItem( id +1, item );
	lwTools->setCurrentRow( id +1 );
}
//
void UIToolsEdit::on_tbHelp_clicked()
{
	QString message = tr( "<b>Tools Editor</b> give you the possibility to use variables<br><br>"
		"<b>%n</b> : Current project name<br>"
		"<b>%p</b> : Current project path<br>"
		"<b>%f</b> : Current project file path<br>"
		"<b>%d</b> : Selected file path ( In Project Box )<br>"
		"<b>%g</b> : Selected file file path ( In Project Box )" );
	QWhatsThis::showText( tbHelp->mapToGlobal( QPoint( 0, 0 ) ), message );
}
//
void UIToolsEdit::on_leCaption_editingFinished()
{
	QListWidgetItem* item = lwTools->currentItem();
	if ( !item )
		return;
	item->setText( leCaption->text() );
}
//
void UIToolsEdit::on_tbIcon_clicked()
{
	QListWidgetItem* item = lwTools->currentItem();
	if ( !item )
		return;
	QString fileName = QFileDialog::getOpenFileName( this, tr( "Choose an icon for this tool" ), QString::null, tr( "Images (*.png *.xpm *.jpg)" ) );
	if ( fileName.isEmpty() )
		return;
	item->setData( Qt::UserRole +1, fileName );
	tbIcon->setIcon( QIcon( fileName ) );
	item->setIcon( tbIcon->icon() );
}
//
void UIToolsEdit::on_lePath_editingFinished()
{
	QListWidgetItem* item = lwTools->currentItem();
	if ( !item )
		return;
	item->setData( Qt::UserRole +2, lePath->text() );
}
//
void UIToolsEdit::accept()
{
	QSettings* settings = Options::self();
	settings->beginWriteArray( "Tools" );
	for ( int i = 0; i < lwTools->count(); i++ )
	{
		settings->setArrayIndex( i );
		settings->setValue( "Caption", lwTools->item( i )->text() );
		settings->setValue( "Icon", lwTools->item( i )->data( Qt::UserRole +1 ).toString() );
		settings->setValue( "Command", lwTools->item( i )->data( Qt::UserRole +2 ).toString() );
	}
	settings->endArray();
	delete settings;
	QDialog::accept();
}
