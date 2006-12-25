/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 09 janvier 2006 - 14h30
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UISubclassWizard.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef UISUBCLASSWIZARD_H
#define UISUBCLASSWIZARD_H
//
#include "ui_UISubclassWizard.h"
//
#include <QPointer>
//
class UISubclassWizard : public QDialog, public Ui::UISubclassWizard
{
	Q_OBJECT
	//
public:
	static UISubclassWizard* self( const QString&, QWidget* = 0 );
	//
private:
	UISubclassWizard( const QString&, QWidget* = 0 );
	static QPointer<UISubclassWizard> _self;
	QString mFilePath;
	//
public slots:
	void on_pbBackward_clicked();
	void on_pbForward_clicked();
	void clicked();
	void loadHeader();
	void loadSource();
	void accept();
	//
};
//
#endif // UISUBCLASSWIZARD_H
