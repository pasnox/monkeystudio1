/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 09 janvier 2006 - 14h20
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UINewFiles.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef UINEWFILES_H
#define UINEWFILES_H
//
#include "ui_UINewFiles.h"
//
#include <QPointer>
//
class UINewFiles : public QDialog, public Ui::UINewFiles
{
	Q_OBJECT
	//
public:
	enum UINewFilesType
	{
		ftProject = 0,
		ftFile,
		ftTemplate
	};
	static UINewFiles* self( QWidget*, UINewFilesType = ftProject );
	//
private:
	UINewFiles( QWidget* = 0, UINewFilesType = ftProject );
	static QPointer<UINewFiles> _self;
	//
private slots:
	void tbBrowse_clicked();
	void accept();
	bool projectAccepted();
	bool fileAccepted();
	bool templateAccepted();
	//
};
//
#endif // UINEWFILES_H
