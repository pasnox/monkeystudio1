/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 09 janvier 2006 - 13h29
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : MDIText.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef MDITEXT_H
#define MDITEXT_H
//
#include "MDIChild.h"
//
class MDIText : public MDIChild
{
	Q_OBJECT
	//
public:
	MDIText( QWidget* parent = 0 );
	//
	bool isModified();
	QString currentFilePath();
	//
	TextEditor* teContents;
	//
protected:
	virtual void closeEvent( QCloseEvent* );
	//
public slots:
	void setEditorFont( const QFont& );
	void save( bool = false );
	//
};
//
#endif // MDITEXT_H
