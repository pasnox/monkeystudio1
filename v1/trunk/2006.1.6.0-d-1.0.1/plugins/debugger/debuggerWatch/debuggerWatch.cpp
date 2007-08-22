/************************************************************************/
// plugin for UIdebugger by Xiantia
// this plugin view your varaible value
// it can modifit this
/************************************************************************/

#include <QtPlugin>
#include <QMessageBox>

#include "debuggerWatch.h"

//****************************************************************
// Plugin Interface

// some infos  plugins
QStringList qslPluginsInfos = QStringList() << "View variable in watch" << "by Xiantia" << "Version 0.0.1";
QString qsPluginMenuName = "View watch";
QString qsPluginName = "View_Watch";




// initialise your widget and return pointer to your widget view
// call only one when parent load plug 
QPointer<QWidget> Watch::pluginGetWidget()
{
	frContener = new QFrame(0);
	layoutH = new QHBoxLayout(frContener);
	layoutH->setMargin(0);

	// set icon to button
	bAddVariable.setIcon(QIcon( ":/Icons/Icons/editadd.png" ));
	bDelVariable.setIcon(QIcon( ":/Icons/Icons/editremove.png" ));

	layoutV = new QVBoxLayout(0);
	layoutV->addWidget(&bAddVariable);
	layoutV->addWidget(&bDelVariable);

	trWatch = new QTreeWidgetEx;
	trWatch->setColumnCount( 4 );
	trWatch->setAcceptDrops(true);
	trWatch->setDropIndicatorShown(true);

	QStringList columnName;
	columnName << tr("Name") << tr("Type")<< tr("Adresse") << tr("Value");
	trWatch->setHeaderLabels (columnName);

	layoutH->addLayout(layoutV);
	layoutH->addWidget(trWatch);

	// if mouse clicked
	connect(trWatch,SIGNAL(signalPluginMouse(QMouseEvent *)),SIGNAL(signalPluginMouse(QMouseEvent *)));
	connect(trWatch,SIGNAL(itemChanged  ( QTreeWidgetItem *, int )),  SLOT(itemChanged( QTreeWidgetItem *,int )));
	connect(trWatch,SIGNAL( currentItemChanged ( QTreeWidgetItem * , QTreeWidgetItem *  )), SLOT(currentItemChanged ( QTreeWidgetItem * , QTreeWidgetItem *  )));  

	connect(&bAddVariable,SIGNAL(clicked()),SLOT(onAddVariable()));
	connect(&bDelVariable,SIGNAL(clicked()),SLOT(onDelVariable()));
	
	qtwiCurrentVariable=NULL;
	bGdbStarted = false;
	bNotify = false;

	return frContener;
}


// call when progect or new project is open
void Watch::pluginInitWidget()
{
	int count = trWatch->topLevelItemCount ();

	bDelVariable.setEnabled(false);
	for(int i=0; i<count; i++)
		delete trWatch->topLevelItem (0);
}


// return name who is in Menu contextuel in UIdebugger
QString Watch::pluginMenuName()
{
	return qsPluginMenuName;
}

// return some info of this plugin
QStringList Watch::pluginInfos()
{
	return qslPluginsInfos;
}

// return Id name of plugin
QString Watch::pluginName()
{
	return qsPluginName;
}

// return a icon
QIcon Watch::pluginMenuIcon()
{
	// use resource of Monkey
	return  QIcon( ":/Icons/Icons/completionvariables.png" );
}

void Watch::pluginGdbStarted(bool a)
{
	bGdbStarted = a;
	if(!bGdbStarted)
	{
		// efface tout les variables
		int count = trWatch->topLevelItemCount ();
		
		// en faire une fonction recursive
		for(int i = 0; i < count; i++)
		{
			QTreeWidgetItem *id = trWatch->topLevelItem ( i );
			
			id->setTextColor(3, Qt::black);
		
			for(int j=0; j< id->childCount(); j++)
 			{
				id->child(j)->setText(1,"...");
				id->child(j)->setText(2,"...");
				id->child(j)->setText(3,"$ = ...");
			}
			id->setText(1,"...");
			id->setText(2,"...");
			id->setText(3,"$ = ...");
		}
	}
}

// call by parent automatic
int Watch::pluginDataFromGdb(struct UIdebugger_struct * debugger_struct)
{
	// TODO
	// insert here your switch(debugger_struct->iPluginCommand)

	// i have a variable in watch ?
	if(trWatch->topLevelItemCount ())
	{
		// yes
		switch(debugger_struct->iPluginCommand)
		{
			case GET_TYPE: 
				return getType(debugger_struct);

			case GET_ADDR: 
				return getAdresse(debugger_struct);

			case GET_VALUE: 
				return getValue(debugger_struct);

			case SET_VALUE:
				return PLUGIN_NOTIFY;
 
			case GET_TRANSLATE : 
				return getTranslate(debugger_struct);
			
		} // end switch

		// TODO
		// insert here your debugger_struct->iCurrentCommand)

		switch(debugger_struct->iCurrentCommand)
		{
			// this plug is started if gdb is in :
			case DEBUGGER_NOTIFY:
				bNotify = true;
			case DEBUGGER_ON_BREAKPOINT:
			case DEBUGGER_STEP_OVER:
			case DEBUGGER_STEP_INTO:
			{
				qtwiCurrentVariable = getVariableNext(NULL);
				getVariableType(&debugger_struct_out,qtwiCurrentVariable->text(0));
				emit signalPluginAction(this, &debugger_struct_out);
				return PLUGIN_SEND_COMMAND;
			}
		}

	}// end if

	// return default
	return PLUGIN_TERMINED;
}



// sub classing
void QTreeWidgetEx::mousePressEvent(QMouseEvent *e)
{
	if( e->button()  == Qt::RightButton)
		// for open menu popu
		emit signalPluginMouse(e);
	else 
		// for change value in QTableWidget
		QTreeWidget::mousePressEvent(e);
}

void QTreeWidgetEx::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/plain"))
	event->acceptProposedAction();
}

void QTreeWidgetEx::dragMoveEvent(QDragMoveEvent *event)
{
	event = event;
}

void QTreeWidgetEx::dropEvent(QDropEvent *event)
{

	QTreeWidgetItem* id = new QTreeWidgetItem( this );
	id->setFlags( id->flags() | Qt::ItemIsEditable);
	QString tmp = event->mimeData()->text();
	tmp.remove(" ");
	tmp.remove("\t");
	tmp.remove("\n");
	
	id->setText(0,tmp);
	id->setText(1,"...");
	id->setText(2,"...");
	id->setText(3,"$ = ...");
}

// member functions
// Ui
void Watch::onAddVariable()
{
	bDelVariable.setEnabled(true);
	QTreeWidgetItem* id = new QTreeWidgetItem( trWatch );
	id->setFlags( id->flags() | Qt::ItemIsEditable);
	qtwiCurrentVariable=id;
	qtwiCurrentVariable->setText(0,tr("insert here"));

	if(!bGdbStarted)
	{
		qtwiCurrentVariable->setText(1,"...");
		qtwiCurrentVariable->setText(2,"...");
		qtwiCurrentVariable->setText(3,"$ = ...");
	}
}


void Watch::onDelVariable()
{
	QTreeWidgetItem * id = trWatch->currentItem();
	if(id !=0)
		delete id;
	
	// si plus de variable dans le watch alors grise le buton delete
	if( !trWatch->topLevelItemCount ())
		bDelVariable.setEnabled(false);
}


void Watch::itemChanged( QTreeWidgetItem *id, int column )
{
	if (id->parent() != NULL) return; // ne met a jour que les topLevel et non les childs

	if(column== 0 && bGdbStarted)
	{
		// on modif le nom d'une variable	
		if( !id->text(0).isEmpty()) 
		{
			qtwiCurrentVariable = id;
			int countChild = qtwiCurrentVariable->childCount();

		
			for(int i=0; i< countChild; i++)
				delete qtwiCurrentVariable->child(0);

			
			getVariableType(&debugger_struct_out,qtwiCurrentVariable->text(0));
			if(bGdbStarted) emit signalPluginAction(this, &debugger_struct_out);
		}
	}
	else
	{
		// on modifit la valeur d'une variable
		if( column == 3 && bGdbStarted)	
		{
			if(! id->text(3).contains("$") ) // enti remote
			{
				qtwiCurrentVariable = id;

				debugger_struct_out.qsCurrentCommand.clear();
				debugger_struct_out.qsCurrentCommand = "p " + id->text(0) +"=" + id->text(3) ;//+  "\r\n";
				debugger_struct_out.iCurrentCommandType = PLUGIN_TO_GDB;
				debugger_struct_out.iPluginCommand = SET_VALUE;
				
				if(bGdbStarted) emit signalPluginAction(this , &debugger_struct_out);
			}
		}
	}	
}


void Watch::currentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous ) 
{
	if(current != NULL)
	{
		if (current->parent() != NULL)
		 	bDelVariable.setEnabled(false);
		else
			bDelVariable.setEnabled(true);
	}
	// no warning
	previous = previous;
}


// end UI


void Watch::itemDoubleClicked ( QTreeWidgetItem * item, int column )
{
}


// other member function

QTreeWidgetItem * Watch::getVariableNext(QTreeWidgetItem * variableName)
{
	if(variableName==NULL)
	{
		// on commence par la premiere variable du watch
		return trWatch->topLevelItem (0);
	}
	else
	{
		// cette variable existe donc
		int index = trWatch->indexOfTopLevelItem ( variableName);
		// retourn le nom de la variable suivant si il y en a une
		return  trWatch->topLevelItem ( index+1 );
	}
}


void Watch::isVariableChangedValue(QTreeWidgetItem *id ,QString variableValue)
{
// regarde la valeur deja presente dans le watch
QString after = id->text(3).right(id->text(3).length() - id->text(3).indexOf("="));
QString befort =  variableValue.right(variableValue.length() - variableValue.indexOf("="));

		// compare avec la nouvelle valeur ?
		if(after != befort)
			// la valeur a change
			// et passe en rouge
			id->setTextColor(3, Qt::red);
		else 
			// elle n'a pas change
			if(!bNotify) 
				id->setTextColor(3, Qt::black);
}

// TYPE

void Watch::getVariableType(UIdebugger_struct *debugger_struct,QString variableName)
{
	debugger_struct->qsCurrentCommand.clear();
	debugger_struct->qsCurrentCommand = QString("whatis ")+ variableName ;//+ "\r\n";
	debugger_struct->iCurrentCommandType = PLUGIN_TO_GDB;
	debugger_struct->iPluginCommand = GET_TYPE;
}


int Watch::getType(struct UIdebugger_struct * debugger_struct)
{
	if(debugger_struct->qsDataFromGdb.contains("type ="))
	{
		// ok , demande l'adresse de la variable
		qtwiCurrentVariable->setText(1,formatVariableType(debugger_struct->qsDataFromGdb));			
		getVariableAdresse(&debugger_struct_out,qtwiCurrentVariable->text(0));
		emit signalPluginAction(this,&debugger_struct_out);
		return PLUGIN_SEND_COMMAND;
	}
	else 
	{
		// pas bon, possible erreur
		QString er = qsGetErGdb(debugger_struct->qsDataFromGdb);
		if(!er.isEmpty())
		{
			// affiche l'erreur
			qtwiCurrentVariable->setText(1, er.remove('\r'));
			getVariableAdresse(&debugger_struct_out,qtwiCurrentVariable->text(0));
			emit signalPluginAction(this,&debugger_struct_out);
			return PLUGIN_SEND_COMMAND;
			
		}
		qDebug("plugin wait type");
		return PLUGIN_WAIT;
	}
}

// VALUE

void Watch::getVariableValue(UIdebugger_struct *debugger_struct,QString variableName)
{
	debugger_struct->qsCurrentCommand.clear();
	debugger_struct->qsCurrentCommand = "p " + variableName ;//+ "\r\n";
	debugger_struct->iCurrentCommandType = PLUGIN_TO_GDB;
	debugger_struct->iPluginCommand = GET_VALUE;
}

	
int Watch::getValue(UIdebugger_struct *debugger_struct)
{
	if(debugger_struct->qsDataFromGdb.contains("$"))
	{
		QString variableFormated = formatVariableValue(debugger_struct->qsDataFromGdb);

		// deformatage des structures

		// ajout les childs si structure					
		if(variableFormated.startsWith("$ = {")) // c'est une structure ?
		{
			// a t-elle deja etait initialis ?
			if(qtwiCurrentVariable->childCount())
			// oui donc update
				decompilStrutUpdate(qtwiCurrentVariable,variableFormated);
			else
				// non ajout les childs a la variable
				decompilStrut(qtwiCurrentVariable,variableFormated);
		}
		// fin deformatage

		// traduction de la variable
		translateReInit();
		if(!translatVariable(&debugger_struct_out).isEmpty())
		{
			// pas de traduction
			isVariableChangedValue( qtwiCurrentVariable, variableFormated);
			qtwiCurrentVariable->setText(3,variableFormated);		

			qtwiCurrentVariable = getVariableNext(qtwiCurrentVariable);
			if(qtwiCurrentVariable == NULL)
			{
				bNotify = false;
				return PLUGIN_TERMINED;
			}

			getVariableType(&debugger_struct_out,qtwiCurrentVariable->text(0));	
			emit signalPluginAction(this,&debugger_struct_out);
			return PLUGIN_SEND_COMMAND;
		}
		else
		{
			emit signalPluginAction(this,&debugger_struct_out);
			 return PLUGIN_SEND_COMMAND;
		}
		// end traduction
	}
	else 
	{
//		QString er = qsGetErPrint(debugger_struct->qsDataFromGdb);
		QString er = qsGetErGdb(debugger_struct->qsDataFromGdb);
		if(!er.isEmpty())
		{
			qtwiCurrentVariable->setText(3,"$ = " + er.remove('\r'));
			qtwiCurrentVariable = getVariableNext(qtwiCurrentVariable);
	
			if(qtwiCurrentVariable==NULL)
				return PLUGIN_TERMINED;
				
			getVariableType(&debugger_struct_out,qtwiCurrentVariable->text(0));	
			emit signalPluginAction(this,&debugger_struct_out);
			return PLUGIN_SEND_COMMAND;
		}
		return PLUGIN_WAIT;
	}
}


// ADRESSE

void Watch::getVariableAdresse(UIdebugger_struct *debugger_struct,QString variableName)
{
	debugger_struct->qsCurrentCommand.clear();
	debugger_struct->iCurrentCommandType = PLUGIN_TO_GDB;
	debugger_struct->iPluginCommand = GET_ADDR;

	// pointeur
	if(variableName.at(0) == '*')
//		debugger_struct->qsCurrentCommand = "p &" + variableName.remove(0,1) ;//+ "\r\n";
		debugger_struct->qsCurrentCommand = "p " + variableName.remove(0,1) ;//+ "\r\n";
	else
	{
		// adresse
		if(variableName.at(0) ==  '&')
			debugger_struct->qsCurrentCommand ="p " + variableName ;//+ "\r\n";
		else
			// variable normal
			debugger_struct->qsCurrentCommand ="p &" + variableName;// + "\r\n";
	}
}


int Watch::getAdresse(UIdebugger_struct *debugger_struct)
{
		// reception de l'adresse
		if(debugger_struct->qsDataFromGdb.contains("$"))
		{
			qtwiCurrentVariable->setText(2,formatVariableAdresse(debugger_struct->qsDataFromGdb));			
			getVariableValue(&debugger_struct_out,qtwiCurrentVariable->text(0));
			emit signalPluginAction(this,&debugger_struct_out);
			return PLUGIN_SEND_COMMAND;
		}
		else
		{	// il y a t il une erreur
			QString er = qsGetErGdb(debugger_struct->qsDataFromGdb);
			if(!er.isEmpty())
			{
				qtwiCurrentVariable->setText(2, er.remove('\r'));
				getVariableValue(&debugger_struct_out,qtwiCurrentVariable->text(0));
				emit signalPluginAction(this,&debugger_struct_out);
				return PLUGIN_SEND_COMMAND;
			}
			return PLUGIN_WAIT;
		}
}


// TRANSLATE

int Watch::getTranslate(struct UIdebugger_struct *debugger_struct)
{
	if(debugger_struct->qsDataFromGdb.contains("$") || debugger_struct->qsDataFromGdb.contains(":") )
	{
		QString translatedVariable  = translatVariable(debugger_struct);
		if(translatedVariable.isEmpty())
		{
			//translate no finish
			emit signalPluginAction(this,&debugger_struct_out);
			return PLUGIN_SEND_COMMAND;
		}
		else
		{
			// translate finished
			isVariableChangedValue( qtwiCurrentVariable, translatedVariable);
			qtwiCurrentVariable->setText(3,translatedVariable);
			
			qtwiCurrentVariable = getVariableNext(qtwiCurrentVariable);
			if(qtwiCurrentVariable==NULL)
			{
				bNotify = false;
				return PLUGIN_TERMINED;
			}				
			getVariableType(&debugger_struct_out,qtwiCurrentVariable->text(0));	
			emit signalPluginAction(this,&debugger_struct_out);
			return PLUGIN_SEND_COMMAND;
		}
	}
	else
	{
//		QString er = qsGetErPrint(debugger_struct->qsDataFromGdb);
		QString er = qsGetErGdb(debugger_struct->qsDataFromGdb);
		if(!er.isEmpty())
		{
			translateReInit();
			qtwiCurrentVariable->setText(3,"$ = " + er.remove('\r'));
			qtwiCurrentVariable = getVariableNext(qtwiCurrentVariable);
			if(qtwiCurrentVariable==NULL)
				return PLUGIN_TERMINED;
					
			getVariableType(&debugger_struct_out,qtwiCurrentVariable->text(0));	
			emit signalPluginAction(this,&debugger_struct_out);
			return PLUGIN_SEND_COMMAND;
		}
		return PLUGIN_WAIT;
	}
}


QString Watch::formatVariableType(QString variableType)
{
// type = int
	variableType.remove(QChar('\n'));
	variableType.remove(QChar('\r')); // for windows
	variableType.remove("type = ");
	if(variableType.startsWith(" ")) 
		variableType.remove(0,1);
	return variableType;
}

QString Watch::formatVariableAdresse(QString variableAdresse)
{
//$4 = (QString *) 0x8063118
	variableAdresse.remove(QChar('\n'));
	variableAdresse.remove(QChar('\r')); // for windows
	int i = variableAdresse.lastIndexOf( ")" );
		
	if( i != -1)
	{
		QString tmp = variableAdresse.right( variableAdresse.length() - i- 2);
		return tmp;
	}
	return variableAdresse;
}

QString Watch::formatVariableValue(QString variableValue)
{
	variableValue.remove("\n");
	variableValue.remove("\r"); // for windows
	// supprime "$x = " 
	return "$ = " + variableValue.right(variableValue.length() - variableValue.indexOf("=") - 2);
}

Q_EXPORT_PLUGIN2(libpnp_plugin , Watch)
