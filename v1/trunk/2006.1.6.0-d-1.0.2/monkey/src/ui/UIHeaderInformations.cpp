/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 26 décembre 2005 - 12h58
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UIHeaderInformations.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "UIHeaderInformations.h"
#include "QMakeProject.h"
#include "Options.h"
#include "main.h"
//
#include <QFile>
#include <QFileInfo>
//
QPointer<UIHeaderInformations> UIHeaderInformations::_self = 0L;
//
UIHeaderInformations* UIHeaderInformations::self( QMakeProject* project, const QString& fileName, QWidget* parent )
{
	if ( !_self )
		_self = new UIHeaderInformations( project, fileName, parent );
	else
	{
		_self->mProject = project;
		_self->mFileName = fileName;
		_self->leProjectName->setText( _self->mProject->name() );
		_self->dteDateTime->setDateTime( QDateTime::currentDateTime() );
	}
	return _self;
}
//
UIHeaderInformations::UIHeaderInformations( QMakeProject* project, const QString& fileName, QWidget* parent )
	: QDialog( parent ), mProject( project ), mFileName( fileName )
{
	setupUi( this );
	//
	leProjectName->setText( mProject->name() );
	dteDateTime->setDateTime( QDateTime::currentDateTime() );
	leAuthor->setText( mProject->stringValue( "AUTHOR" ) );
	leEmail->setText( mProject->stringValue( "EMAIL" ) );
	cbLicense->setEditText( mProject->stringValue( "LICENSE" ) );
}
//
const QString UIHeaderInformations::getInformations()
{
	if ( exec() && !cbDisable->isChecked() )
	{
		QFile file( ":/Templates/Templates/headerInformations.txt" );
		if ( !file.open( QFile::ReadOnly ) )
		{
			QMessageBox::critical( 0, _CRITICAL, tr( "Can't open header informations templates." ) );
			return QString::null;
		}
		//
		QString content = file.readAll();
		content.replace( "$PROJECTNAME$", leProjectName->text(), Qt::CaseInsensitive );
		content.replace( "$DATE$", dteDateTime->date().toString( "dddd dd MMMM yyyy" ), Qt::CaseInsensitive );
		content.replace( "$TIME$", dteDateTime->time().toString( "hh'h'mm" ), Qt::CaseInsensitive );
		content.replace( "$AUTHORNAME$", leAuthor->text(), Qt::CaseInsensitive );
		content.replace( "$AUTHOREMAIL$", leEmail->text(), Qt::CaseInsensitive );
		content.replace( "$FILENAME$", QFileInfo( mFileName ).fileName(), Qt::CaseInsensitive );
		content.replace( "$LICENSE$", cbLicense->currentText(), Qt::CaseInsensitive );
		content.replace( "$COMMENTARY$", teCommentary->toPlainText(), Qt::CaseInsensitive );
		mProject->setStringValues( "AUTHOR", "(all)", leAuthor->text() );
		mProject->setStringValues( "EMAIL", "(all)", leEmail->text() );
		mProject->setStringValues( "LICENSE", "(all)", cbLicense->currentText() );
		return content;
	}
	if ( cbDisable->isChecked() )
		Options::self()->setValue( "Options/General/ShowHeaderInformations", false );
	return QString::null;
}
