/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 30 janvier 2006 - 17h47
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UIOptions.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef UIOPTIONS_H
#define UIOPTIONS_H
//
#include "ui_UIOptions.h"
//
#include <QPointer>
#include <QHash>
//
class UIOptions : public QDialog, public Ui::UIOptions
{
	Q_OBJECT
	//
public:
	static UIOptions* self( QWidget* = 0 );
	//
private:
	UIOptions( QWidget* = 0 );
	static QPointer<UIOptions> _self;
	//
private slots:
	void loadOptions();
	void saveOptions();
	void on_cbPath_currentIndexChanged( int );
	void on_tbPath_clicked();
	void on_lePath_editingFinished();
	void tbText_clicked();
	void colorize();
	void on_pbDefault_clicked();
	void accept();
	//
};
//
#endif // UIOPTIONS_H
