/*============================================
autor : xiantia
date : 02/09/06


class name UIdebuger


public slot:
	
	// enable /desable le menu contextuel start/stop
	debugerEnable(true); // si projet ouvert
	debugerEnable(false); // a la fermeture du debuger

	// ajoute ou supprime un breakpoint dans le debuger
	void debugerToggleBreakpoint(QString Filname, int atLine);

	// memorise le nom du programe executable
	debugerSetProgName(QString ProgName); // remplace debugerStart()

signal	
	debugerSignalAtLine(QString file, int numLine);
	debugerSignalStoped();
	debugerSignalStarted();
*/




//#ifndef UIDEBUGER_H
//#define UIDEBUGER_H
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
	plugin_manager(){
		debuggerStartUp = false;
	}

	void init(){
		qpPluginSender = NULL;
		iCurrentPlugin=0;
		plugin_struct_out.iPluginCommand =   -1;
		plugin_struct_out.iCurrentCommand =   -1;
	}
	
	void loadPlugins(QObject * ,QMenu * , QStackedWidget *);
	void copyStruct(UIdebugger_struct *);
	void addPlugin(UIdebugger_Plugin *plugin, QWidget *widget);
	void execute(QString st);
	
	void setCurrentCommand(int i){
		plugin_struct_out.iCurrentCommand = i;
	}

	void setPluginSender(UIdebugger_Plugin *ps){
		qpPluginSender = ps;
	}

	UIdebugger_Plugin * plugin(int i){
		return qlPlugin.at(i).plugin_Pointer;
	}

	void pluginNotify(bool a);
	int contains(QWidget *);

	bool debuggerStartUp;
	
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

class UIdebuger : public QFrame, public Ui::UIdebuger
{
	Q_OBJECT
	//
public :
	static UIdebuger* self( QWidget* = 0 );

	// enable / desable debuger menu contextuel start/stop
	void debugerEnable(bool enable);
	// enregistre le chemin de l'executable
	void debugerSetProgName(QString progName);
	// toggle breakpoint
	void debugerToggleBreakpoint(QString, int);

protected:
	void closeEvent( QCloseEvent* );

private:
	UIdebuger( QWidget* = 0 );
	static QPointer<UIdebuger> _self;

	// PLUGIN
	class plugin_manager pluginManager;

	QTimer pluginWatchDog;
	//
	// start / stop debugger
	void debugerStart();
	void debugerStop();

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
	QAction * qActionDebugerStart;
	QAction * qActionDebugerStop;
	QAction * qActionDebugerArgs;
	QAction * qActionPluginSetting;
	void debugerSetMenuContext();

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
	void on_qActionDebugerStart_triggered();
	void on_qActionDebugerStop_triggered();
	void on_qActionDebugerArgs_triggered();
	void on_qActionPluginSetting_triggered();

	// action du debugger par defaut
	void ondebugerNextStepOver();
	void ondebugerNextStepInto();
	void ondebugerContinue();

	// message en provenance de gdb
	void ondebugerMsgFrom(QString st);

	// watch dog des plugin
	void onPluginWatchDog();

signals:
	// le debugger est sur un breakpoint, donc notify le 
	void debugerSignalAtLine(QString, int);
	// le debugger est arreté
	void debugerSignalStoped();
	// le debugger est lancé
	void debugerSignalStarted();
};
//
//#endif // UIDEBUGER_H
