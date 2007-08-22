/********************************************************************************************************
		* PROGRAM      : qt4ds
		* DATE - TIME  : vendredi 06 janvier 2006 - 01h50
		* AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
			* FILENAME     : UIMain.h
			* LICENSE      : GPL
			* COMMENTARY   :
********************************************************************************************************/
#ifndef UIMAIN_H
#define UIMAIN_H
//
#include "ui_UIMain.h"
#include "QMakeProject.h"
#include "MDIChild.h"
//
#include <QPointer>
#include <QProcess>
//
class QComboBox;
class QActionGroup;
class QAssistantClient;
//
class UIMain : public QMainWindow, public Ui::UIMain
{
	Q_OBJECT
	//
public:
	enum { MaxRecentFiles = 15, MaxRecentProjects = 15 };
	static UIMain* self( QWidget* = 0 );
	QMakeProject* currentProject();
	QProcess* process();
	bool startDetached( const QString& );
	void start( const QString&, const QString& = QString::null );
	void waitProcess();
	void setMenuEnabled( QMenu*, bool );
	//
protected:
	void showEvent( QShowEvent* );
	void closeEvent( QCloseEvent* );
	void changeEvent( QEvent* );
	//
private:
	UIMain( QWidget* = 0 );
	static QPointer<UIMain> _self;
	QPointer<QMakeProject> mProject;
	bool mInit;
	QAction* actionRecentlyOpenedFiles[ MaxRecentFiles ];
	QAction* actionRecentlyOpenedProjects[ MaxRecentProjects ];
	QProcess* mProcess;
	QComboBox* cbFont;
	QComboBox* cbFontSize;
	QActionGroup* agStyles;
	QAssistantClient* mManual;
	QAssistantClient* mAssistant;
	//
public slots:
	void on_workspace_windowActivated( QWidget* );
	void child_activated( MDIChild* );
	void on_mManual_error( const QString& );
	void on_mAssistant_error( const QString& );
	// TabBar
	void on_tabBar_currentChanged( int );
	void addTab( MDIChild* );
	void removeTab( MDIChild* );
	//
	void on_twProjectsFiles_customContextMenuRequested( const QPoint& );
	void on_twProjectsFiles_itemDoubleClicked( QTreeWidgetItem*, int );
	//
	void alert( const QString& );
	void focusChanged( QWidget*, QWidget* );
	void showReplaceDialog();
	void createWindow( const QString&, pFileType, QMakeProject* = 0 );
	// Font & Size
	void on_cbFont_activated( const QString& );
	void on_cbFontSize_activated( const QString& );
	// File
	bool openFile( const QString&, QMakeProject* = 0 );
	void saveAllOpenedWindow( bool = false, QMakeProject* = 0 );
	void updateRecentFiles();
	void updateRecentProjects();
	//
	void on_actionFileOpen_triggered();
	void on_actionFileClose_triggered();
	void on_actionFileSave_triggered();
	void on_actionFileSaveAll_triggered();
	void on_actionFileSaveAsTemplate_triggered();
	void on_actionFileQuickPrint_triggered();
	void on_actionFilePrint_triggered();
	void on_actionClearRecentsFiles_triggered();
	void actionRecentlyOpenedFiles_triggered();
	void on_actionClearRecentsProjects_triggered();
	void actionRecentlyOpenedProjects_triggered();
	void on_actionFileExit_triggered();
	// Edit
	void on_actionEditUndo_triggered();
	void on_actionEditRedo_triggered();
	void on_actionEditCut_triggered();
	void on_actionEditCopy_triggered();
	void on_actionEditPaste_triggered();
	void on_actionEditGotoLine_triggered();
	void on_actionEditOptions_triggered();
	// View
	void agStyles_triggered( QAction* );
	// Project
	void saveProject( QMakeProject* );
	void saveAllProject( QMakeProject* );
	bool openProject( const QString& );
	void projectOpened( QMakeProject* );
	void projectClosed( QMakeProject* );
	void projectModified( QMakeProject*, bool );
	void projectOpenFile( QMakeProject*, const QString& );
	bool removeFromProject();
	bool deleteFromProject();
	//
	void on_actionProjectNew_triggered();
	void on_actionProjectOpen_triggered();
	void on_actionProjectSaveCurrent_triggered();
	void on_actionProjectSaveAll_triggered();
	void on_actionProjectCloseCurrent_triggered();
	void on_actionProjectCloseAll_triggered();
	void on_actionProjectShowSource_triggered();
	void on_actionProjectShowToDo_triggered();
	void on_actionProjectShowChanges_triggered();
	void on_actionProjectAddNewForm_triggered();
	void on_actionProjectAddNewFiles_triggered();
	void on_actionProjectAddNewTemplates_triggered();
	void on_actionProjectAddExistingFiles_triggered();
	void on_actionProjectOptions_triggered();
	// Build
	void on_mProcess_started();
	void on_mProcess_finished( int, QProcess::ExitStatus );
	void on_mProcess_readyRead();
	void on_actionBuildBuildCurrent_triggered();
	void on_actionBuildBuildAll_triggered();
	void on_actionBuildRebuildCurrent_triggered();
	void on_actionBuildRebuildAll_triggered();
	void on_actionBuildCleanCurrent_triggered();
	void on_actionBuildCleanAll_triggered();
	void on_actionBuildBuildAndExecute_triggered();
	void on_actionBuildExecute_triggered();
	void on_actionBuildLupdate_triggered();
	void on_actionBuildLrelease_triggered();
	// xiantia, Debugger
	void on_teToggleBreakpoint(QString, int);
	void on_DebugerAtLine(QString fileorfonction, int line);
	// Tools
	void loadTools();
	void on_actionToolsEdit_triggered();
	void actionTools_triggered();
	// Help
	void on_actionHelpManual_triggered();
	void on_actionHelpQtAssistant_triggered();
	void on_actionHelpAbout_triggered();
	void on_actionHelpAboutQt_triggered();
};
//
#endif // UIMAIN_H
