/********************************************************************************************************
 * PROGRAM      : TextEditor
 * DATE - TIME  : lundi 01 mai 2006 - 14h54
 * AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
 * FILENAME     : ReplaceDialog.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef REPLACEDIALOG_H
#define REPLACEDIALOG_H
//
#include "ui_ReplaceDialog.h"
//
#include <QPointer>
#include <QDesignerExportWidget>
//
class QTextEdit;
//
#ifndef TEXTEDITORPLUGIN
#undef QDESIGNER_WIDGET_EXPORT
#define QDESIGNER_WIDGET_EXPORT
#endif
//
class QDESIGNER_WIDGET_EXPORT ReplaceDialog : public QFrame, public Ui::ReplaceDialog
{
	Q_OBJECT
	//
public:
	static ReplaceDialog* self( QWidget* = 0, Qt::WFlags = Qt::Tool );
	ReplaceDialog( QWidget* = 0, Qt::WFlags = 0 );
	//
	QTextEdit* workingEdit();
	enum Mode { mNormal = -1, mPopup, mDocked };
	Mode mode();
	bool searchNext();
	bool replaceNext();
	//
public slots:
	static void tasks();
	void setWorkingEdit( QTextEdit* );
	void setMode( Mode );
	void replaceAll();
	void on_pbSearch_clicked();
	void on_pbReplace_clicked();
	void on_pbReplaceAll_clicked();
	//
private:
	static QPointer<ReplaceDialog> mSelf;
	QPointer<QTextEdit> mWorkingEdit;
	Mode mMode;
	//
signals:
	void modeChanged( Mode );
	//
};
//
#endif // REPLACEDIALOG_H
