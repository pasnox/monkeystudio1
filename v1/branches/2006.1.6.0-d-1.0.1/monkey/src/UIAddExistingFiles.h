/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : vendredi 13 janvier 2006 - 23h44
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UIAddExistingFiles.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef UIADDEXISTINGFILES_H
#define UIADDEXISTINGFILES_H
//
#include <QFileDialog>
#include <QPointer>
//
class QGroupBox;
class QHBoxLayout;
class QComboBox;
//
class QMakeProject;
//
class UIAddExistingFiles : public QFileDialog
{
	Q_OBJECT
	//
public:
	static UIAddExistingFiles* self( QMakeProject*, QWidget* = 0 );
	//
private:
	UIAddExistingFiles( QMakeProject*, QWidget* = 0, const QString& = QString(), const QString& = QString(), const QString& = QString() );
	//
	static QPointer<UIAddExistingFiles> _self;
	QGroupBox* gb;
	QHBoxLayout* hbl;
	QComboBox* cbProjects;
	QComboBox* cbPlatforms;
	QMakeProject* projectParent;
	//
public slots:
	void accept();
	//
};
//
#endif // FORMADDEXISTINGFILES_H
