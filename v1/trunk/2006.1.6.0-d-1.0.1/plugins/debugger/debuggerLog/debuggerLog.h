#ifndef plug_H
#define plug_H

#include <QTextEdit>
#include <QPointer>
#include <QWidget>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QIcon>

#include "interface_plugin.h"

class QTextEditEx: public QTextEdit
{
	Q_OBJECT
public :

signals:
	void signalPluginMouse(QMouseEvent *);

public slots :

	void mousePressEvent ( QMouseEvent * event ) ;

};


class LogPlugin :  public QObject, public UIdebugger_Plugin
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
	struct UIdebugger_struct debugger_struct_out;

	QTextEditEx *teGdbLog;
	QFrame *frLog;
	QBoxLayout *vBoxLayoutLog;;
	QBoxLayout *hBoxLayoutLog;
	QLineEdit *leLog;
	QLabel *laLog;

private slots:
	void onUserCommand();

signals : 
	void signalPluginAction(UIdebugger_Plugin * ,struct UIdebugger_struct *debugger_struct);
	void signalPluginMouse(QMouseEvent *);
};

#endif
