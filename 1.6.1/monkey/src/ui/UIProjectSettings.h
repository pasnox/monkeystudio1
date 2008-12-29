/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : vendredi 27 janvier 2006 - 02h22
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UIProjectSettings.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef UIPROJECTSETTINGS_H
#define UIPROJECTSETTINGS_H
//
#include "ui_UIProjectSettings.h"
#include "QMakeProject.h"
//
#include <QPointer>
//
class UIProjectSettings : public QDialog, public Ui::UIProjectSettings
{
	Q_OBJECT
	//
public:
	static UIProjectSettings* self( QMakeProject*, QWidget* );
	//
private:
	UIProjectSettings( QMakeProject*, QWidget* );
	static QPointer<UIProjectSettings> _self;
	QStringList parseString( const QString& );
	QString getText( QWidget*, const QString&, const QString&, bool* = 0, const QString& = QString::null );
	QString getExistingDirectory( QWidget*, const QString&, const QString& );
	QString getOpenFileName( QWidget*, const QString&, const QString&, const QString& );
	HashStringValues stringValues;
	QMakeProject* mProject;
	//
public slots:
	void on_cbSystem_currentIndexChanged( const QString& );
	void on_cbSystem_highlighted( int );
	void on_tbDestination_clicked();
	void on_cbSetting_currentIndexChanged( const QString& );
	void on_cbSetting_highlighted( int );
	void on_pbAdd_clicked();
	void on_pbModify_clicked();
	void on_pbDelete_clicked();
	void on_pbClear_clicked();
	void accept();
};
//
#endif // UIPROJECTSETTINGS_H
