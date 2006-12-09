/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 30 janvier 2006 - 17h47
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UIToolsEdit.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef UITOOLSEDIT_H
#define UITOOLSEDIT_H
//
#include "ui_UIToolsEdit.h"
//
#include <QPointer>
//
class UIToolsEdit : public QDialog, public Ui::UIToolsEdit
{
	Q_OBJECT
	//
public:
	static UIToolsEdit* self( QWidget* = 0 );
	struct Tool
	{
		QString Caption;
		QString iconFilePath;
		QString binFilePath;
	};
	//
private:
	UIToolsEdit( QWidget* = 0 );
	static QPointer<UIToolsEdit> _self;
	//
public slots:
	void on_lwTools_itemActivated( QListWidgetItem* );
	void on_pbNew_clicked();
	void on_pbDelete_clicked();
	void on_pbUp_clicked();
	void on_pbDown_clicked();
	void on_tbHelp_clicked();
	void on_leCaption_editingFinished();
	void on_tbIcon_clicked();
	void on_lePath_editingFinished();
	void accept();
	//
};
//
#endif // UITOOLSEDIT_H
