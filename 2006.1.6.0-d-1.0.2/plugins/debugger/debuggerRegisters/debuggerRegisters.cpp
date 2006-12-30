/************************************************************************/
// plugin for UIdebugger by Xiantia
// this plugin show all values of register cpu in QTableWidgetEx derivate QTableWidget
// it can modifi register value
/************************************************************************/

#include <QtPlugin>

#include "debuggerRegisters.h"
#include "settingRegister.h"

//****************************************************************
// Plugin Interface


// some infos  plugins
QStringList qslPluginsInfos = QStringList() << "View Registers for UIDebugger" << "by Xiantia" << "Version 0.5.1";
QString qsPluginMenuName = "View Registers";
QString qsPluginName = "Registers_plugin";

#define GET_REGISTER_VALUE		PLUGIN_BASE_USER 
#define SET_REGISTER_VALUE		PLUGIN_BASE_USER +1

// initialise your widget and return pointer to your widget view
// here is QTableWidgetEx
// call only one when parent load plug 
QPointer<QWidget> RegisterPlugin::pluginGetWidget()
{
	// create widget
	twRegister = new QTableWidgetEx;
	// 4 column
	twRegister->setColumnCount(4);
	// label
	twRegister->setHorizontalHeaderLabels (QStringList() << "Registers" << "values" << "Registers" << "Values");
	// if mouse clicked
	connect(twRegister,SIGNAL(signalPluginMouse(QMouseEvent *)),SIGNAL(signalPluginMouse(QMouseEvent *)));
	connect(twRegister,SIGNAL(itemChanged ( QTableWidgetItem *)), SLOT(registersChanged(QTableWidgetItem*)));
	bGdbStarted = false;
	bNotify = false;
	return twRegister;
}

// call when project or new project is open
void RegisterPlugin::pluginInitWidget()
{
	// del all row in QTableWidget
	for(int i=0; i<twRegister->rowCount(); i++)
		twRegister->removeRow(0);
}


// return name who is in Menu contextuel in UIdebugger
QString RegisterPlugin::pluginMenuName()
{
	return qsPluginMenuName;
}

// return some info of this plugin
QStringList RegisterPlugin::pluginInfos()
{
	return qslPluginsInfos;
}

// return Id name of plugin
QString RegisterPlugin::pluginName()
{
	return qsPluginName;
}

// return a icon
QIcon RegisterPlugin::pluginMenuIcon()
{
	// use resource of Monkey
	return  QIcon( ":/Icons/Icons/completionslots.png" );
}


void RegisterPlugin::pluginGdbStarted(bool a)
{
	bGdbStarted = a;

	if(!bGdbStarted)
	{
		for(int i =0; i <twRegister->rowCount(); i++)
		{
			 if(twRegister->item(i,1) != NULL) twRegister->item(i,1)->setText("$ = ...") ;
			 if(twRegister->item(i,3) != NULL) twRegister->item(i,3)->setText("$ = ...") ;
			
		}
	}
}

QDialog * RegisterPlugin::pluginSetting()
{
	return settingRegister::self();
}

int RegisterPlugin::pluginDataFromGdb(struct UIdebugger_struct * debugger_struct)
{
	// TODO
	// insert here your switch(debugger_struct->iPluginCommand)

	// update register
	switch(debugger_struct->iPluginCommand)
	{
		case GET_REGISTER_VALUE:
		{
			// update QTableWidgetEx view
			if(debugger_struct->qsDataFromGdb.contains("eax"))
			{
				if(registerShowAll(debugger_struct->qsDataFromGdb))
					registerUpdate(debugger_struct->qsDataFromGdb);
				bNotify = false;
				return PLUGIN_TERMINED;
			}
			return PLUGIN_WAIT;
		}

		case SET_REGISTER_VALUE:	// user have change value of register
			return PLUGIN_NOTIFY;	// well notify all plug (and this plug !)

	}

	// TODO
	// insert here your  switch(debugger_struct->iCurrentCommand)
	
	// this plug ask values register when GDB is in breakpoint, next step , ....
	switch(debugger_struct->iCurrentCommand)
	{
		case DEBUGGER_NOTIFY:
			bNotify = true;	// just for change color of text
		case DEBUGGER_ON_BREAKPOINT:
		case DEBUGGER_STEP_OVER:
		case DEBUGGER_STEP_INTO:
		{
			debugger_struct_out.qsCurrentCommand.clear();
			//command send to GDB
			debugger_struct_out.qsCurrentCommand = "info register";//\n";
			// for GDB
			debugger_struct_out.iCurrentCommandType = PLUGIN_TO_GDB;
			// define user
			debugger_struct_out.iPluginCommand = GET_REGISTER_VALUE;
			// send command
			emit signalPluginAction(this, &debugger_struct_out);
			// end return 
			return PLUGIN_SEND_COMMAND;		
		}
	}
	// other plug termined
	return PLUGIN_TERMINED;
}


//*********************************************************************
// member fonction

// subclassing 
void QTableWidgetEx::mousePressEvent(QMouseEvent *e)
{
	if( e->button()  == Qt::RightButton)
		// for open menu popu
		emit signalPluginMouse(e);
	else 
		// for change value in QTableWidget
		QTableWidget::mousePressEvent(e);
}


void QTableWidgetEx::resizeEvent(QResizeEvent * event )
{
	event = event ; // no warning
	for(int i=0; i< columnCount(); i++)
		setColumnWidth(i,width()/4);
}


// end Interface plug

//*************************************************************

// gestion du plugin

// on change la valeur d'un registre
void RegisterPlugin::registersChanged(QTableWidgetItem  *id)
{
	int row = twRegister->currentRow();
	int column = twRegister->currentColumn();
	if( column == 1 || column==3) 
	{
		if( ! id->text().contains("$"))
		{
			QString regName = (twRegister->item(row,column-1))->text();
			QString regValue =  (twRegister->item(row,column))->text();
			debugger_struct_out.qsCurrentCommand.clear();
			// when you send "set $eax=9", gdb not answer !!
			// juste after send "info register" for update your widget
			debugger_struct_out.qsCurrentCommand = "set $" + regName +"=" + regValue;// + "\n";
			debugger_struct_out.iCurrentCommandType = PLUGIN_TO_GDB;
			debugger_struct_out.iPluginCommand = SET_REGISTER_VALUE;
			if(bGdbStarted) 
				emit signalPluginAction(this, &debugger_struct_out);			

		}
	}
}


// decompilation de la trame GDB

// retourn le nom du registre contenu dans st
QString RegisterPlugin::registerGetName(QString st)
{
	QStringList list = st.split(" ",QString::SkipEmptyParts);
	return list.at(0);	
}

// retourn la valeur du registre contenu dans st
QString RegisterPlugin::registerGetValue(QString st)
{
QStringList list;

	if(settingRegister::self()->radioDecimal->isChecked ())
		// affiche decimal
		list = st.split("\t", QString::SkipEmptyParts);
	else
	 	// affiche hexa et decimal
		list = st.split(" ", QString::SkipEmptyParts);
	return list.at(1);
}


void RegisterPlugin::registerIsVariableChangeValue(QTableWidgetItem *id ,QString regValue)
{
QString after = id->text();
QString befort = "$ = " + regValue;

		if(after != befort)
			id->setTextColor( Qt::red);
		else 
			if(!bNotify) id->setTextColor(Qt::black);
}


// mis a jours des registres
void RegisterPlugin::registerUpdate(QString st)
{
int currentRow = 0;
int currentColumn = 0;
#define numberMaxRow	9

	QStringList list = st.split("\n",QString::SkipEmptyParts);

	int numberList = list.count();
		
	for(int i=0; i<numberList; i++)
	{
		if((currentRow+1) % numberMaxRow);
		else
		{
			currentRow=0;
			currentColumn+=2;
		}
			

		QString regValue = registerGetValue(list.at(i));
		regValue.remove("\r"); // for windows

		QTableWidgetItem *itemValue = twRegister->item(currentRow,currentColumn+1);
		registerIsVariableChangeValue(itemValue,regValue);
		itemValue->setText("$ = " + regValue );
		twRegister->setItem(currentRow,currentColumn+1, itemValue);
				
		currentRow++;
	}	
}



// decompilation des registres
bool RegisterPlugin::registerShowAll(QString st)
{
int currentRow = 0;
int currentColumn = 0;
#define numberMaxRow	9

	if(twRegister->rowCount()) 
		// les registres sont deja initialis il faut donc les mettres a jours
		return 1;

	QStringList list = st.split("\n",QString::SkipEmptyParts);

	int numberList = list.count();
		
//	registerInsertColumn(twRegister,currentColumn);


	twRegister->setRowCount(numberList/2);

	for(int i=0; i<numberList; i++)
	{
		if((currentRow+1) % numberMaxRow)
		{
		//	int nb = twRegister->rowCount();
			
			if(i<numberMaxRow) 
			{
		//		twRegister->insertRow( i);
				twRegister->setRowHeight( i,20);
			}
		}
		else
		{
			currentRow=0;
			currentColumn+=2;
//			registerInsertColumn(twRegister,currentColumn);
		}
			

		QString regName = registerGetName(list.at(i));
		regName.remove("\r");
		QString regValue = registerGetValue(list.at(i));
		regValue.remove("\r");
//		regValue.replace(QChar('\t'),QChar('['),Qt::CaseSensitive);
			
		QTableWidgetItem *itemName = new QTableWidgetItem("name");
		itemName->setText(regName);
		itemName->setFlags(Qt::ItemIsEnabled);
		twRegister->setItem(currentRow,currentColumn, itemName);
		QTableWidgetItem *itemValue = new QTableWidgetItem("valeur");
		itemValue->setText("$ = " + regValue );
		itemValue->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable);

		twRegister->setItem(currentRow,currentColumn+1, itemValue);
				
		currentRow++;
	}	

	return 0;
}



// export class
Q_EXPORT_PLUGIN2(libpnp_plugin , RegisterPlugin)
