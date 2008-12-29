/********************************************************************************************************
 * PROGRAM      : demo
 * DATE - TIME  : mercredi 03 mai 2006 - 21h17
 * AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
 * FILENAME     : UITextEditorDemo.h
 * LICENSE      : GPL
 * COMMENTARY   : TextEditor Demo
 ********************************************************************************************************/
#ifndef UITEXTEDITORDEMO_H
#define UITEXTEDITORDEMO_H
//
#include "ui_UITextEditorDemo.h"
//
#include <QPointer>
//
class UITextEditorDemo : public QMainWindow, public Ui::UITextEditorDemo
{
	Q_OBJECT
	//
public:
	static UITextEditorDemo* self( QWidget* = 0 );
	//
private:
	UITextEditorDemo( QWidget* = 0 );
	static QPointer<UITextEditorDemo> _self;
	//
public slots:
	void on_teEditor_overwriteModeChanged( bool );
	void on_teEditor_cursorPositionChanged( int, int );
	//
	void on_actionOpen_triggered();
	void on_actionClose_triggered();
	void on_actionQuit_triggered();
	void on_actionSetIndentMethod_triggered();
	void on_actionSetHighlightCurrentLine_triggered();
	void on_actionSetCurrentLineColor_triggered();
	void on_actionSetLimitLine_triggered();
	void on_actionSetLimitLineColor_triggered();
	void on_actionSetTabSize_triggered();
	void on_actionSetBackupFile_triggered();
	void on_actionSetFilePath_triggered();
	void on_actionSetScrollLines_triggered();
	void on_actionSetSelectionColor_triggered();
	void on_actionSetBackgroundColor_triggered();
	void on_actionSetDefaultComponents_triggered();
	//
};
//
#endif // UITEXTEDITORDEMO_H
