/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 09 janvier 2006 - 13h29
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : MDIClass.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef MDICLASS_H
#define MDICLASS_H
//
#include "MDIChild.h"
//
class QWorkspace;
class QAction;
class QActionGroup;
//
class MDIClass : public MDIChild
{
	Q_OBJECT
	//
public:
	MDIClass( QWidget* = 0 );
	//
	bool isModified();
	QString currentFilePath();
	//
	QWorkspace* wSpace;
	TextEditor* teHeader;
	TextEditor* teSource;
	QActionGroup* aGroup;
	QAction* actionForm;
	QAction* actionHeader;
	QAction* actionSource;
	QString mFormFileName;
	//
protected:
	void resizeEvent( QResizeEvent* );
	virtual void closeEvent( QCloseEvent* );
	//
public slots:
	void on_aGroup_triggered( QAction* );
	void on_wSpace_windowActivated( QWidget* );
	//
	void setEditorFont( const QFont& );
	void save( bool = false );
	void beforeCompletionShow();
	//
	void subClass(const QString& );
	//
};
//
#endif // MDICLASS_H
