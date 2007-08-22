/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : vendredi 13 janvier 2006 - 23h44
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UIAddExistingFiles.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "UIAddExistingFiles.h"
#include "QMakeProject.h"
//
#include <QGroupBox>
#include <QLayout>
#include <QComboBox>
//
QPointer<UIAddExistingFiles> UIAddExistingFiles::_self = 0L;
//
UIAddExistingFiles* UIAddExistingFiles::self( QMakeProject* project, QWidget* parent )
{
	if ( !_self )
		_self = new UIAddExistingFiles( project, parent, tr( "Choose files to add to the project" ), project->absolutePath(), QMakeProject::qtExtensions() );
	return _self;
}
//
UIAddExistingFiles::UIAddExistingFiles( QMakeProject* project, QWidget* parent, const QString& caption, const QString& dir, const QString& filter )
	: QFileDialog( parent, caption, dir ), projectParent( project )
{
	setAttribute( Qt::WA_DeleteOnClose );
	gb = new QGroupBox;
	gb->setTitle( tr( "In project..." ) );
	hbl = new QHBoxLayout( gb );
	cbProjects = new QComboBox;
	cbPlatforms = new QComboBox;
	hbl->addWidget( cbProjects );
	hbl->addWidget( cbPlatforms );
	qobject_cast<QGridLayout*>( layout() )->addWidget( gb, 4, 0, 1, 6 );
	setFileMode( QFileDialog::ExistingFiles );
	while ( projectParent->parent() )
		projectParent = qobject_cast<QMakeProject*>( projectParent->parent() );
	cbProjects->addItems( QMakeProject::getProjectNames( projectParent, "subdirs" ) );
	cbProjects->setCurrentIndex( cbProjects->findText( project->name() ) );
	if ( cbProjects->currentIndex() == -1 )
		cbProjects->setCurrentIndex( 0 );
	cbPlatforms->addItems( QStringList() << "(all)" << "win32" << "unix" << "mac" );
	setFilters( filter.split( ";;" ) );
}
//
void UIAddExistingFiles::accept()
{
	QStringList fileNames = selectedFiles();
	if ( fileNames.isEmpty() )
		return;
	QString suffix;
	QMakeProject* project = QMakeProject::getProjectByName( cbProjects->currentText(), projectParent );
	foreach ( QString file, fileNames )
	{
		switch ( project->getFileTypeFromFilePath( file ) )
		{
		case pForm:
			project->addStringListValue( "FORMS", cbPlatforms->currentText(), file );
			break;
		case pHeader:
			project->addStringListValue( "HEADERS", cbPlatforms->currentText(), file );
			break;
		case pSource:
			project->addStringListValue( "SOURCES", cbPlatforms->currentText(), file );
			break;
/*
		case pTranslation:
			project->addStringListValue( "TRANSLATIONS", cbPlatforms->currentText(), file );
			break;
*/
		case pText:
			project->addStringListValue( "TEXTS", cbPlatforms->currentText(), file );
			break;
		default:
			break;
		}
	}
	QDialog::accept();
}
