#ifndef plug_H
#define plug_H

#include <QTableWidget>
#include <QPointer>
#include <QWidget>
#include <QMouseEvent>


#include "interface_plugin.h"
 
// just for mousePressEvent redirection
class QTableWidgetEx : public QTableWidget
{
	Q_OBJECT
public slots:
	void mousePressEvent(QMouseEvent *e);

protected:
	 void resizeEvent ( QResizeEvent * event ) ;

signals:
	void signalPluginMouse(QMouseEvent *);
};



class RegisterPlugin :  public QObject, public UIdebugger_Plugin
{
	Q_OBJECT
	Q_INTERFACES(UIdebugger_Plugin)
 
public:
	// call when your plugin is loaded
	QPointer <QWidget> pluginGetWidget();
	void pluginInitWidget();	

	QString pluginMenuName();
	QIcon pluginMenuIcon();
	QString pluginName();
	QStringList pluginInfos();
 
	int  pluginDataFromGdb(struct UIdebugger_struct * debugger_struct);
	void pluginGdbStarted(bool);

private :
	bool bGdbStarted;
	bool bNotify;

	struct UIdebugger_struct debugger_struct_out;

	QTableWidgetEx *twRegister;
	void registerUpdate(QString st);
	bool registerShowAll(QString st);
	void registerIsVariableChangeValue(QTableWidgetItem *id ,QString regValue);
	QString registerGetName(QString st);
	QString registerGetValue(QString st);
private slots:
	void registersChanged(QTableWidgetItem  *id);

signals : 
	void signalPluginAction(UIdebugger_Plugin * ,struct UIdebugger_struct *debugger_struct);
	void signalPluginMouse(QMouseEvent *);
};

#endif
