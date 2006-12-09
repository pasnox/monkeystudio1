/************************************************************************/
// plugin for UIdebugger by Xiantia
// this plugin show log gdb
// it can send user command to gdb
/************************************************************************/

#include <QtPlugin>

#include "debuggerLog.h"

//****************************************************************
// Plugin Interface


// some infos  plugins
QStringList qslPluginsInfos = QStringList() << "View log gdb for UIDebugger" << "by Xiantia" << "Version 0.0.1";
QString qsPluginMenuName = "View Gdb log";
QString qsPluginName = "Log_plugin";

#define PLUGIN_CHANGE		PLUGIN_BASE_USER

// initialise your widget and return pointer to your widget view
// here is QFrame
// call only one when parent load plug 
QPointer<QWidget> LogPlugin::pluginGetWidget()
{
	frLog = new QFrame(0); // main contener
	vBoxLayoutLog =  new QVBoxLayout(frLog);
	hBoxLayoutLog= new QHBoxLayout(0);
	hBoxLayoutLog->setMargin(0);
	vBoxLayoutLog->setMargin(0);

	leLog = new QLineEdit(0);
	laLog = new QLabel(tr("Command line :"));
	
	teGdbLog = new QTextEditEx();
	teGdbLog->setReadOnly(true);

	// label and lineEdit in layout
	hBoxLayoutLog->addWidget(laLog);
	hBoxLayoutLog->addWidget(leLog);
	// layout and log in layout
	vBoxLayoutLog->addWidget(teGdbLog);
	vBoxLayoutLog->addLayout(hBoxLayoutLog);

	// if mouse clicked
	connect(teGdbLog,SIGNAL(signalPluginMouse(QMouseEvent *)),SIGNAL(signalPluginMouse(QMouseEvent *)));
	connect(leLog,SIGNAL(returnPressed ()), SLOT(onUserCommand()));
	
	bGdbStarted = false;
	return frLog;
}


// call when progect or new project is open
void LogPlugin::pluginInitWidget()
{
	teGdbLog->clear();
	teGdbLog->setTextColor(Qt::blue);
	teGdbLog->append("*** Gdb console ***\n");
}


// return name who is in Menu contextuel in UIdebugger
QString LogPlugin::pluginMenuName()
{
	return qsPluginMenuName;
}

// return some info of this plugin
QStringList LogPlugin::pluginInfos()
{
	return qslPluginsInfos;
}

// return Id name of plugin
QString LogPlugin::pluginName()
{
	return qsPluginName;
}

// return a icon
QIcon LogPlugin::pluginMenuIcon()
{
	// use resource of Monkey
	return  QIcon( ":/Icons/Icons/tabconsole.png" );
}

void LogPlugin::pluginGdbStarted(bool a)
{
	bGdbStarted = a; 
	leLog->setEnabled(bGdbStarted);
	if(bGdbStarted) teGdbLog->clear();
}


// call by parent automatique
int LogPlugin::pluginDataFromGdb(struct UIdebugger_struct * debugger_struct)
{
	switch(debugger_struct->iPluginCommand)
	{
		case PLUGIN_CHANGE :
			return PLUGIN_NOTIFY;
		default :
			teGdbLog->setTextColor(Qt::black);
			if(!debugger_struct->qsDataFromGdb.isEmpty())
				teGdbLog->append("Gdb > " + debugger_struct->qsDataFromGdb);	
	}
	return PLUGIN_TERMINED;
}


//*********************************************************************
// member fonction

// subclassing 

void QTextEditEx::mousePressEvent(QMouseEvent *e)
{
	if( e->button()  == Qt::RightButton)
		// for open menu popu
		emit signalPluginMouse(e);
	else 
		// for change value in QTableWidget
		QTextEdit::mousePressEvent(e);
}

// end Interface plug

//*************************************************************

// gestion du plugin
void LogPlugin::onUserCommand()
{
	teGdbLog->setTextColor(Qt::blue);
	teGdbLog->append(tr("User command : "));
	teGdbLog->setTextColor(Qt::black);
	teGdbLog->append("-> " + leLog->text() );

	debugger_struct_out.qsCurrentCommand.clear();
	debugger_struct_out.qsCurrentCommand = leLog->text()  ;//+ "\n";
	debugger_struct_out.iCurrentCommandType = PLUGIN_TO_GDB;
	debugger_struct_out.iPluginCommand = PLUGIN_CHANGE;

	if(bGdbStarted) emit signalPluginAction(this, &debugger_struct_out);			
	leLog->clear();
}


// export class
Q_EXPORT_PLUGIN2(libpnp_plugin , LogPlugin)
