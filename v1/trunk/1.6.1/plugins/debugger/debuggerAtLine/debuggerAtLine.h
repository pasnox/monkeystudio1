#ifndef plug_H
#define plug_H

#include <QPointer>
#include <QWidget>
#include <QMouseEvent>
#include <QIcon>

#include "interface_plugin.h"



class DebuggerAt :  public QObject, public UIdebugger_Plugin
{
	Q_OBJECT
	Q_INTERFACES(UIdebugger_Plugin)
 
public:
	QPointer <QWidget> pluginGetWidget();
	void pluginInitWidget();	

	QString pluginMenuName();
	QIcon pluginMenuIcon();
	QString pluginName();
	QStringList pluginInfos();
	QDialog * pluginSetting();
 
	int  pluginDataFromGdb(struct UIdebugger_struct * debugger_struct);
	void pluginGdbStarted(bool);
private :
	struct UIdebugger_struct debugger_struct_out;

signals : 
	void signalPluginAction(UIdebugger_Plugin* , struct UIdebugger_struct *debugger_struct);
	void signalPluginMouse(QMouseEvent *);
};

#endif
