/*

author : Xiantia
date : 02/09/06

class name UIdebugger
*/
	
	/* 
	A pour effet de griser les menu start et stop du menu contextuel.
	De stopper le debugger si il etait en fonction a la fermeture du projet
	De notify a tout les plugin que le projet est ouvert / fermé 
	*/
	
	//	debuggerProjetOpened(true / false);

	/* 
	Permet d'ajouter / supprimer un  breakpoint dans le debugger
	*/

	//	void debuggerToggleBreakpoint(QString Filname, int atLine);

	/*
	Donne au debugger le nom du programme
	*/

	//	debugerSetProgName(QString ProgName); // remplace debugerStart()

	/* 
	signal emit
	*/

	//	debuggerSignalAtLine(QString file, int numLine);
	//	debuggerSignalStoped();
	//	debuggerSignalStarted();


#ifndef UIDEBUGER_H
#define UIDEBUGER_H
//
#include "ui_UIdebuger.h"
#include "src/debuger/gdbDriver.h"
#include "src/debuger/interface_plugin.h"

//
#include <QTimer>
#include <QPointer>
#include <QFile>
#include <QBoxLayout>
#include <QStackedWidget>
#include <QMouseEvent>
#include <QMenu>
#include <QWidget>
#include <QTableWidget>

// for plugin
#include <QDir>
#include <QPluginLoader>

extern void printQstring(QString str);


// ============= CLASS PLUGIN_MANAGER ============

class plugin_manager : public QObject
{
	Q_OBJECT
public:
	plugin_manager()
	{
//		debuggerStartUp = false;
	}

	void init()
	{
		qpPluginSender = NULL;
		iCurrentPlugin=0;
		plugin_struct_out.iPluginCommand =   -1;
		plugin_struct_out.iCurrentCommand =   -1;
	}
	
	void loadPlugins(QObject * ,QMenu * , QStackedWidget *);
	void copyStruct(UIdebugger_struct *);
	void addPlugin(UIdebugger_Plugin *plugin, QWidget *widget);
	void execute(QString st);
	
	void setCurrentCommand(int i)
	{
		plugin_struct_out.iCurrentCommand = i;
	}

	void setPluginSender(UIdebugger_Plugin *ps)
	{
		qpPluginSender = ps;
	}

	UIdebugger_Plugin * plugin(int i)
	{
		return qlPlugin.at(i).plugin_Pointer;
	}

	void pluginNotifyGdbStarted(bool a);
	void pluginNotifyProjetOpened();

	int contains(QWidget *);

//	bool debuggerStartUp;
	
private:
	// index du plugin courant
	int iCurrentPlugin;
	UIdebugger_Plugin *  qpPluginSender;

	struct UIdebugger_struct plugin_struct_out;
	
	struct plugin_list
	{
		// list des pointer sur les plug
		UIdebugger_Plugin *plugin_Pointer;
		// list des Widgets qui seront dans le stacked
		QWidget * plugin_Widget;
	};

	QList <plugin_list> qlPlugin;
};


// =============== CLASS UI_DEBUGGER =============

class UIdebugger : public QFrame, public Ui::UIdebuger
{
	Q_OBJECT
	//
public :
	static UIdebugger* self( QWidget* = 0 );
	//
	// enable / desable debuger menu contextuel start/stop
	void debuggerProjetOpened(bool open);
	//
	// enregistre le chemin de l'executable
	void debuggerSetProgName(QString progName);
	//
	// toggle breakpoint
	void debuggerToggleBreakpoint(QString, int);


protected:
	void closeEvent( QCloseEvent* );

private:
	UIdebugger( QWidget* = 0 );
	static QPointer<UIdebugger> _self;

	// PLUGIN
	class plugin_manager pluginDebuggerManager;
	//
	// start / stop debugger
	void debuggerStart();
	void debuggerStop();

	// BREAKPOINT
	struct sfile
	{
		QString File_name;
		QList <int> Bp_list;
		QList <int> Bp_index;
	};

	// list de breakpoint
	int findFile(QString file);
	QList <sfile> BreakpointList;

	void breakpointInit();
	void setBreakpointOnStart();
	void setBreakpointIndex(QString st);
	
	// MENU
	QMenu *menu;
	QAction * qActionDebuggerStart;
	QAction * qActionDebuggerStop;
	QAction * qActionDebuggerArgs;
	QAction * qActionPluginSetting;
	void debuggerSetMenuContext();

	QString currentCommand;
	QString m_progName;		// nom du programme a lancer
	QString m_progArgs;		// list des argument du programme

	//  pointeur de gdb	
	GdbDriver *pGdbCommand;

public slots:
	void mousePressEvent ( QMouseEvent * event ) ;
	// on click sur un plugin -> ouverture du menu contextuel
	void onPluginMouse(QMouseEvent *);
	// un plugin demande des infos a gdb 
	void onPluginAction(UIdebugger_Plugin * ,struct UIdebugger_struct *);
	// on a selectionné un plug a afficher dans le stacked
	void onPluginMenuAction(QAction * action);
	
	// action du menu par defaut
	void on_qActionDebuggerStart_triggered();
	void on_qActionDebuggerStop_triggered();
	void on_qActionDebuggerArgs_triggered();
	void on_qActionPluginSetting_triggered();

	// action du debugger par defaut
	void ondebuggerNextStepOver();
	void ondebuggerNextStepInto();
	void ondebuggerContinue();

	// message en provenance de gdb
	void onDataFromGdb(QString st);

	// watch dog des plugin
	void onPluginWatchDog();

signals:
	// le debugger est sur un breakpoint, donc notify le 
	void debuggerSignalAtLine(QString, int);
	// le debugger est arreté
	void debuggerSignalStoped();
	// le debugger est lancé
	void debuggerSignalStarted();
};
//
#endif // UIDEBUGER_H
