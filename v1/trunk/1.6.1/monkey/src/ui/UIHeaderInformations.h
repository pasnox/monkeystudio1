/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 26 décembre 2005 - 12h51
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UIHeaderInformations.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef UIHEADERINFORMATIONS_H
#define UIHEADERINFORMATIONS_H
//
#include "ui_UIHeaderInformations.h"
//
#include <QPointer>
//
class QMakeProject;
//
class UIHeaderInformations : public QDialog, public Ui::UIHeaderInformations
{
	Q_OBJECT
	//
public:
	static UIHeaderInformations* self( QMakeProject*, const QString&, QWidget* = 0 );
	const QString getInformations();
	QMakeProject* mProject;
	QString mFileName;
	//
private:
	UIHeaderInformations( QMakeProject*, const QString&, QWidget* = 0 );
	static QPointer<UIHeaderInformations> _self;
	//
};
//
#endif // UIHEADERINFORMATIONS_H
