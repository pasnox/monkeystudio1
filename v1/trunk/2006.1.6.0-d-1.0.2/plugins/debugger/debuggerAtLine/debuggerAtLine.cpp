/************************************************************************/
// plugin for UIdebugger by Xiantia
// this plugin search gdb at line ?
/************************************************************************/

#include <QtPlugin>

#include "debuggerAtLine.h"

//****************************************************************
// Plugin Interface


// some infos  plugins
QStringList qslPluginsInfos = QStringList() << "Search gdb possition" << "by Xiantia" << "Version 0.0.1";
QString qsPluginMenuName = "";	// not in menu :)
QString qsPluginName = "debugger_At";

#define DEBUGGER_AT		PLUGIN_BASE_USER

// initialise your widget and return pointer to your widget view
// call only one when parent load plug 
QPointer<QWidget> DebuggerAt::pluginGetWidget()
{
	return NULL;	// no widget for debugger at line
}


// call when progect or new project is open
void DebuggerAt::pluginInitWidget()
{
	// no init for this plug
}


// return name who is in Menu contextuel in UIdebugger
QString DebuggerAt::pluginMenuName()
{
	return qsPluginMenuName;
}

// return some info of this plugin
QStringList DebuggerAt::pluginInfos()
{
	return qslPluginsInfos;
}

// return Id name of plugin
QString DebuggerAt::pluginName()
{
	return qsPluginName;
}

// return a icon
QIcon DebuggerAt::pluginMenuIcon()
{
	// use resource of Monkey
	return  QIcon();	// no icon in menu, because i don't have a widget to view
}

void DebuggerAt::pluginGdbStarted(bool a)
{
	a=a; // no warnning
}

QDialog * DebuggerAt::pluginSetting()
{
	return NULL;
}

// call by parent automatique
int DebuggerAt::pluginDataFromGdb(struct UIdebugger_struct * debugger_struct)
{

	// TODO 
	// insert here your switch(debugger_struct->iPluginCommand)

	switch(debugger_struct->iPluginCommand)
	{
		case DEBUGGER_AT:
		{
			if(debugger_struct->qsDataFromGdb.contains("at"))
			{
				// decompilation
				QString nameFile;
				QString line;
				QStringList list = debugger_struct->qsDataFromGdb.split("#");

				int indexdeb = list.at(1).lastIndexOf(" at ") + 4;
				int indexfin = list.at(1).lastIndexOf(":") ;
				
				nameFile = list.at(1).mid(indexdeb,indexfin-indexdeb);
				line =  list.at(1).right(list.at(1).length() - indexfin -1);

				// send this to parent , NOT GDB

				debugger_struct_out.iCurrentCommandType = PLUGIN_TO_PARENT;
				debugger_struct_out.qsCurrentCommand.clear();
				debugger_struct_out.qsCurrentCommand = nameFile + ":" + line;

				emit signalPluginAction(this , &debugger_struct_out);
				return PLUGIN_TERMINED; // plug termined
			}
			return PLUGIN_WAIT;
		}
	}

	// TODO 
	// insert here your switch(debugger_struct->iCurrentCommand)
	switch(debugger_struct->iCurrentCommand)
	{
		case DEBUGGER_ON_BREAKPOINT:
		case DEBUGGER_STEP_OVER:
		case DEBUGGER_STEP_INTO:
//		case DEBUGGER_NOTIFY:
		// other plug have change value, but Gdb is at the same line, do not use DEBUGGER_NOTIFY
		// case DEBUGGER_NOTIFY :
		{
			debugger_struct_out.iCurrentCommandType = PLUGIN_TO_GDB;
			debugger_struct_out.qsCurrentCommand.clear();
			debugger_struct_out.qsCurrentCommand = "bt";//\n";
			debugger_struct_out.iPluginCommand = DEBUGGER_AT;
			emit signalPluginAction(this , &debugger_struct_out);

			return PLUGIN_SEND_COMMAND;
		}
	}
	// return by default 
	return PLUGIN_TERMINED;
}

Q_EXPORT_PLUGIN2(libpnp_plugin , DebuggerAt)
