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

	exemple:
	debugerToggleBreakpoint("main.cpp", 18); // ajoute un breakpoint a la ligne18
	debugerToggleBreakpoint("main.cpp", 18); // supprime le breakpoint de la ligne 18  puisqu'il existé.

	// memorise le nom du programe executable
	debugerSetProgName(QString ProgName); // remplace debugerStart()
 	
	// arrete le debugeur
	debugerStop();

	// retrouver tout les breakpoint d'un fichier
	QList debugerGetBreakpoint(QString fileName);


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
//#include "src/debuger/debuger_plugins.h"
#include "src/debuger/interface_plugin.h"

//
#include <QTimer>
#include <QPointer>
#include <QFile>
#include <QBoxLayout>
#include <QStackedWidget>
//#include <QTreeWidgetItem>
//#include <QTableWidgetItem> 
#include <QMouseEvent>
//#include <QLineEdit>
//#include <QLabel>
#include <QMenu>
#include <QWidget>
#include <QTableWidget>
// for plugin
#include <QDir>
#include <QPluginLoader>
//

/*class QTextEditEx: public QTextEdit
{
	public :
	QTextEditEx(){setReadOnly(true);};

	void mousePressEvent ( QMouseEvent * event ) ;

};
*/
class QTableWidgetEx : public QTableWidget 
{
//	Q_OBJECT
public : 
	QTableWidgetEx(){};

	void mousePressEvent ( QMouseEvent * event ) ;

};


class UIdebuger : public QFrame, public Ui::UIdebuger
{
	Q_OBJECT
	//
public :
	static UIdebuger* self( QWidget* = 0 );
	//
	// enable / desable debuger menu contextuel start/stop
	void debugerEnable(bool enable);
	void debugerSetProgName(QString progName);
	void debugerToggleBreakpoint(QString, int);
//	void debugerInitAll();

	struct sfile
	{
		QString File_name;
		QList <int> Bp_list;
		QList <int> Bp_index;
	};


	QList <int> debugerGetBreakpoint(QString fileName);
	QList <UIdebuger::sfile > debugerGetAllBreakpoint();

public slots:
	void mousePressEvent ( QMouseEvent * event ) ;

protected:
	void closeEvent( QCloseEvent* );
	//
	void resizeEvent( QResizeEvent* );

private:

	// plugin
	void loadPlugins();
	QList <UIdebugger_Plugin *> plugin_list;
	QList <QWidget *> plugin_WidgetList;
	UIdebugger_Plugin *  qpPlugSender;

	void debugerStart();
	void debugerStop();



	// les breakpoints
	void breakpointInit();
	void breakpointInitWidget();
	void breakpointAdd(QString, QString);
	void breakpointSup(QString fileName, QString line);

	void setBreakpointOnStart();
	void setBreakpointIndex(QString st);


	//
	UIdebuger( QWidget* = 0 );
	static QPointer<UIdebuger> _self;

	QString currentCommand;
	QString m_progName;
	QString m_progArgs;

	//  pointeur de gdb	
	GdbDriver *pGdbCommand;

	// list de breakpoint
	int findFile(QString file);
	QList <sfile> BreakpointList;

	QAction * qActionViewThreads;
	
	QAction * qActionDebugerStart;
	QAction * qActionDebugerStop;
	QAction * qActionDebugerArgs;

//	QAction * qActionViewBreakpoints;
//	QAction * qActionViewConditionelBreakpoints;

	void debugerSetMenuContext();
	QMenu *menu;

public slots:

	void onPluginMouse(QMouseEvent *);
	void onPluginAction(UIdebugger_Plugin * ,struct UIdebugger_struct *);	// menu contextuel
	void onPluginMenuAction(QAction * action);

//	void on_qActionViewBreakpoints();
	void on_qActionDebugerStart();
	void on_qActionDebugerStop();
	void on_qActionDebugerArgs();


	void ondebugerNextStepOver();
	void ondebugerNextStepInto();
	void ondebugerContinue();

	void ondebugerMsgFrom(QString st);
private: 
	struct UIdebugger_struct plugin_struct;

signals:
	void debugerSignalAtLine(QString, int);
	void debugerSignalStoped();
	void debugerSignalStarted();
};
//
//#endif // UIDEBUGER_H
