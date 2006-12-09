#include <QWidget>
#include <QPointer>

#ifndef _PLUGIN_INTERFACE_
#define  _PLUGIN_INTERFACE_

// mapping DEFINE
// from 1000 to 1499 : define DEBUGGER, no change this !
// from 1500 to 1999 : define PLUGIN, no change this !
// from 2000 to .... : your define

// from debugger do not change this
#define DEBUGGER_BASE_ADDR		1000

#define DEBUGGER_ON_BREAKPOINT		DEBUGGER_BASE_ADDR+1	// when gdb break 
#define DEBUGGER_NEW_BREAKPOINT		DEBUGGER_BASE_ADDR+2	// when user add new breakpoint on editor
#define DEBUGGER_STEP_OVER		DEBUGGER_BASE_ADDR+3	// when gdb execute step over
#define DEBUGGER_STEP_INTO		DEBUGGER_BASE_ADDR+4	// when gdb execute step into
#define DEBUGGER_NOTIFY		DEBUGGER_BASE_ADDR+5


#define PLUGIN_BASE_ADDR		DEBUGGER_BASE_ADDR+500

#define PLUGIN_TERMINED		PLUGIN_BASE_ADDR+1		// when plug have temined 
#define PLUGIN_WAIT			PLUGIN_BASE_ADDR+2		// when plug wait other data from gdb
#define PLUGIN_SEND_COMMAND		PLUGIN_BASE_ADDR+3		// when you want send data to gdb
#define PLUGIN_NOTIFY			PLUGIN_BASE_ADDR+4		// when you want notify other plugin
#define PLUGIN_TO_GDB			PLUGIN_BASE_ADDR+5		// when you want send  data to GDB
#define PLUGIN_TO_PARENT		PLUGIN_BASE_ADDR+6		// when yiou want send data to parent

#define PLUGIN_BASE_USER		PLUGIN_BASE_ADDR+500

/*
#define PLUGIN_ER_TYPE		 "No symbol " \
			<< "Attempt to use a type name as an expression"  \
			<< "A syntax error in expression, near" \
			<< " has no component named" \
			<< " is not a structure or union type." \
			<< "Invalid character" \
			<< " in expression."

			
#define PLUGIN_ER_PRINT	"No symbol " \
			<< "There is no member or method named " \
			<< "Structure has no component named " \
			<< "Cannot access memory at address " \
			<< "A syntax error in expression, near" \
			<< "Attempt to extract a component of a value that is not a structure." \
			<< "cannot subscript something of type" \
			<< "Invalid character" \
			<< " in expression."

#define PLUGIN_ER_ADDR		"Argument to arithmetic operation not a number or boolean." \
			<< "Attempt to take address of value not located in memory." \
			<< "Invalid character" \
			<< " in expression."
*/

#define PLUGIN_ER_GDB			 "No symbol " \
			<< "Attempt to use a type name as an expression"  \
			<< "A syntax error in expression, near" \
			<< " has no component named" \
			<< " is not a structure or union type." \
			<< "There is no member or method named " \
			<< "Structure has no component named " \
			<< "Cannot access memory at address " \
			<< "Attempt to extract a component of a value that is not a structure." \
			<< "cannot subscript something of type" \
			<< "Argument to arithmetic operation not a number or boolean." \
			<< "Attempt to take address of value not located in memory." \
			<< "Invalid character" \
			<< "Can't take address of " \
			<< "too few arguments in function call" \
			<< "should be called only once" \
			<< "No type named"
			

			
struct UIdebugger_struct
{
	QString qsCurrentCommand;	// use when your plug send a command to gdb or parent
	int iCurrentCommandType;	// use when your plug send a command,  PLUGIN_TO_GDB or PLUGIN_TO_PARENT
	int iCurrentCommand;		
	int iPluginCommand;		// your define command for pluginDataFromGdb()
	QString qsDataFromGdb;		// data from gdb
};


class UIdebugger_Plugin  
{
public:
	UIdebugger_Plugin()
	{
//		qslGetEr_Type = QStringList() << PLUGIN_ER_TYPE;
//		qslGetEr_Print = QStringList() << PLUGIN_ER_PRINT;
//		qslGetEr_Adresse = QStringList() << PLUGIN_ER_ADDR;
		qslGetEr_Gdb = QStringList() << PLUGIN_ER_GDB;
	}

	virtual ~UIdebugger_Plugin() {}
 
	virtual QString pluginMenuName() = 0;		// return name of your plugin in menu
    	virtual QIcon pluginMenuIcon()=0;			// return Icon of your plugin in menu
	virtual QString pluginName()=0;			// return name of your plugin
	virtual QStringList pluginInfos()=0;			// return some infos

	virtual QPointer <QWidget > pluginGetWidget()=0;		// return pointer of yout widget contener
	virtual void pluginInitWidget()=0;			// init your widget (clear, add row, ....)

	// when UIdebugger is on breakpoint , next step or other plugin request data,  this function is call automaticly
	// if you want send command to gdb, return :
	//	PLUGIN_TERMINED if your plug is  finished
	//	PLUGIN_WAIT if your plug wait other data from GDB
	//	PLUGIN_SEND_COMMAND if your plug has send command to GDB or PARENT
	//	PLUGIN_NOTIFY replace PLUGIN_TERMINED for notify other update plug 
	virtual int pluginDataFromGdb(struct UIdebugger_struct * debugger_struct)= 0;
	virtual void pluginGdbStarted(bool)=0;


//	QString qsGetErType(QString );
//	QString qsGetErPrint(QString );
//	QString qsGetErAdresse(QString );
	

	QString qsGetErGdb(QString );

private : 
//	QStringList qslGetEr_Type;
//	QStringList qslGetEr_Print;
//	QStringList qslGetEr_Adresse;
	QStringList qslGetEr_Gdb;

signals:
	virtual void signalPluginAction(UIdebugger_Plugin * ,struct UIdebugger_struct *debugger_struct)=0;
	virtual void signalPluginMouse(QMouseEvent *)=0;
};
 


Q_DECLARE_INTERFACE(UIdebugger_Plugin, "plugin.UIdebugger_Plugin/1.0")

#endif
