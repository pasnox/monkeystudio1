/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 30 janvier 2006 - 17h47
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UIAbout.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef UIABOUT_H
#define UIABOUT_H
//
#include "ui_UIAbout.h"
//
#include <QPointer>
//
class UIAbout : public QDialog, public Ui::UIAbout
{
	Q_OBJECT
	//
public:
	static UIAbout* self( QWidget* = 0 );
	//
public slots:
	void anchorClicked( const QUrl& );
	//
private:
	UIAbout( QWidget* = 0 );
	static QPointer<UIAbout> _self;
	//
};
//
#endif // UIABOUT_H
