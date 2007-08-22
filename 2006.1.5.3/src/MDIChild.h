/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 26 décembre 2005 - 12h41
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : MDIChild.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef MDICHILD_H
#define MDICHILD_H
//
#include <QFrame>
//
#include "main.h"
#include "TextEditor.h"
#include "QMakeProject.h"
//
class QMenu;
//
enum childType
{
	ctEmpty = 0,
	ctClass,
	ctText,
	ctAssistant,
	ctCustom
};
//
class MDIChild : public QFrame
{
	Q_OBJECT
	//
public:
	MDIChild( QWidget* = 0 );
	//
	childType type();
	QString tabFilePath() const;
	QMakeProject* project();
	//
	virtual bool isModified();
	virtual void save( bool = false );
	virtual void setEditorFont( const QFont& );
	virtual QString currentFilePath();
	//
private:
	childType mType;
	QString mTabFilePath;
	QMakeProject* mProject;
	//
protected:
	virtual void showEvent( QShowEvent* );
	virtual void closeEvent( QCloseEvent* );
	//
signals:
	void removeTab( MDIChild* );
	void activated( MDIChild* );
	void modified( bool );
	void replaceDialogRequested();
	//
public slots:
	void setSettings( TextEditor* );
	void setType( const childType );
	void setTabFilePath( const QString& );
	void setProject( QMakeProject* );
	void fileOpened( bool );
	//
	virtual void beforeCompletionShow();
};
//
#endif // MDICHILD_H
