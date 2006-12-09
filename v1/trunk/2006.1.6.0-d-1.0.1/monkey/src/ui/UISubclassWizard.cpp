/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 09 janvier 2006 - 14h30
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UISubclassWizard.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "UISubclassWizard.h"
#include "UIMain.h"
#include "QMakeProject.h"
#include "main.h"
//
#include <QFile>
#include <QFileInfo>
#include <QInputDialog>
#include <QMessageBox>
//
QPointer<UISubclassWizard> UISubclassWizard::_self = 0L;
//
UISubclassWizard* UISubclassWizard::self( const QString& filePath, QWidget* parent )
{
	if ( !_self )
		_self = new UISubclassWizard( filePath, parent );
	return _self;
}
//
UISubclassWizard::UISubclassWizard( const QString& filePath, QWidget* parent )
	: QDialog( parent ), mFilePath( filePath )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
}
//
void UISubclassWizard::on_pbBackward_clicked()
{
	swPages->setCurrentIndex( swPages->currentIndex() -1 );
	clicked();
}
//
void UISubclassWizard::on_pbForward_clicked()
{
	swPages->setCurrentIndex( swPages->currentIndex() +1 );
	clicked();
}
//
void UISubclassWizard::clicked()
{
	switch ( swPages->currentIndex() )
	{
	case 0:
		pbBackward->setEnabled( false );
		pbForward->setText( tr ( "Next" ) );
		break;
	case 1:
		pbBackward->setEnabled( true );
		pbForward->setText( tr ( "Next" ) );
		loadHeader();
		break;
	case 2:
		pbBackward->setEnabled( true );
		if ( pbForward->text() == tr( "Next" ) )
		{
			pbForward->setText( tr ( "Finish" ) );
			loadSource();
		}
		else
			accept();
		break;
	}
	lInformations->setText( swPages->currentWidget()->toolTip() );
}
//
void UISubclassWizard::loadHeader()
{
	if ( !teHeader->toPlainText().isEmpty() )
		return;
	QFile file( ":/Templates/Templates/subClass_header.txt" );
	if ( file.open( QFile::ReadOnly | QFile::Text ) )
	{
		QString buffer = file.readAll();
		buffer.replace( "FILENAME", QFileInfo( mFilePath ).baseName().append( "_h" ).toUpper() );
		buffer.replace( "INCLUDE", QFileInfo( mFilePath ).baseName().append( ".h" ).prepend( "ui_" ) );
		buffer.replace( "CLASSNAME", QFileInfo( mFilePath ).baseName() );
		buffer.replace( "HERITS", cbGetParent->currentText() );
		teHeader->setPlainText( buffer );
		file.close();
	}
}
void UISubclassWizard::loadSource()
{
	if ( !teSource->toPlainText().isEmpty() )
		return;
	QFile file( ":/Templates/Templates/subClass_source.txt" );
	if ( file.open( QFile::ReadOnly | QFile::Text ) )
	{
		QString buffer = file.readAll();
		buffer.replace( "INCLUDE", QFileInfo( mFilePath ).baseName().append( ".h" ) );
		buffer.replace( "CLASSNAME", QFileInfo( mFilePath ).baseName() );
		buffer.replace( "HERITS", cbGetParent->currentText() );
		teSource->setPlainText( buffer );
		file.close();
	}
}
//
void UISubclassWizard::accept()
{
	QString path = QFileInfo( mFilePath ).path().append( "/" ).append( QFileInfo( mFilePath ).baseName() );
	QMakeProject* project = UIMain::self()->currentProject();
	//
	QFile header( path.append( ".h" ) );
	if ( header.open( QFile::WriteOnly | QFile::Text ) )
	{
		header.write( teHeader->toPlainText().toAscii() );
		header.close();
		project->addStringListValue( "HEADERS", cbGetWindowManager->currentText(), path );
		UIMain::self()->createWindow( path, pHeader, project );
		QFile source( path.replace( ".h", ".cpp") );
		if ( source.open( QFile::WriteOnly | QFile::Text ) )
		{
			source.write( teSource->toPlainText().toAscii() );
			source.close();
			project->addStringListValue( "SOURCES", cbGetWindowManager->currentText(), path );
			UIMain::self()->createWindow( path, pSource, project );
			QDialog::accept();
		}
		else
		{
			QMessageBox::warning( this, _WARNING, tr( "Unable to create the files." ) );
			QDialog::reject();
		}
	}
	else
	{
		QMessageBox::warning( this, _WARNING, tr( "Unable to create the files." ) );
		QDialog::reject();
	}
}
