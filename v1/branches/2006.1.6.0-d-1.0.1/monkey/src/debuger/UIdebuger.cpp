//==================================
// Autor : Xiantia
// Main debuger frontend
//==================================

#include "UIdebuger.h"

//
#include <QCloseEvent>
#include <QMessageBox>
//
QPointer<UIdebuger> UIdebuger::_self = 0L;
//

void printQstring(QString str)
{ 
 QChar *data = str.data();
    while (*data != 0x0) 
	{
        fprintf(stdout,"%c",data->unicode());
        ++data;
    }
fprintf(stdout,"\n");
	
}



//**********************************************************************

void UIdebuger::loadPlugins()
{
bool havePlug=false;

	QDir pluginsDir = QDir(qApp->applicationDirPath());
	pluginsDir.cd("plugins");
//	printQstring("INIT PLUGIN INTERFACE .... ");
	foreach (QString fileName, pluginsDir.entryList(QDir::Files))
	{
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        		QObject *plugin = loader.instance();
        		if (plugin) 
		{
			UIdebugger_Plugin * op = qobject_cast<UIdebugger_Plugin *>(plugin);
            			if (op) 
			{
				plugin_list << op;
				QWidget * pWidget = op->pluginGetWidget();
//				printQstring("plugin LOAD .... " + fileName);
				if(pWidget != NULL)	// add on menu
				{
					havePlug = true;
					plugin_WidgetList << pWidget;
					QAction *ac = new QAction(op->pluginMenuIcon(),op->pluginMenuName(),pWidget);
					menu->addAction(ac);//op->pluginMenuName());
					stackedWidget->addWidget(pWidget);
					connect(plugin,SIGNAL(signalPluginMouse(QMouseEvent * )),SLOT(onPluginMouse(QMouseEvent *)));
				}
				connect(plugin,SIGNAL(signalPluginAction( UIdebugger_Plugin *, UIdebugger_struct *  )),SLOT(onPluginAction( UIdebugger_Plugin * ,UIdebugger_struct *)));
//				printQstring("LOADED");
			}
            		}
        	}
	connect(menu,SIGNAL(triggered ( QAction * )), SLOT(onPluginMenuAction(QAction * )));
	if(havePlug) 	stackedWidget->setCurrentWidget(plugin_WidgetList.at(0));

}


void UIdebuger::onPluginMouse(QMouseEvent *e)
{
	if( e->button()  == Qt::RightButton)
		menu->popup(QCursor::pos());
}


void memDebugger_struct(UIdebugger_struct *des, UIdebugger_struct *src)
{
	des->qsCurrentCommand.clear();
	des->qsCurrentCommand = src->qsCurrentCommand;
	des->iCurrentCommandType = src->iCurrentCommandType;
	des->iPluginCommand = src->iPluginCommand;
}


void UIdebuger::onPluginAction(UIdebugger_Plugin *plug_sender , struct UIdebugger_struct *debugger_struct )
{
	qpPlugSender = plug_sender;

	switch(debugger_struct->iCurrentCommandType)
	{
		case  PLUGIN_TO_GDB:
		{
			if(!debugger_struct->qsCurrentCommand.isEmpty())
			{
				butonContinue->setEnabled(false);
				butonNextStepInto->setEnabled(false);
				butonNextStepOver->setEnabled(false);
				gdbWorking->setText(tr("Debugger working ..."));
				qActionDebugerStop->setEnabled(false);

				memDebugger_struct(&plugin_struct,debugger_struct);
				pGdbCommand->setCommand(debugger_struct->qsCurrentCommand);
//				printQstring("UIdebugger send : " + debugger_struct->qsCurrentCommand);
			}
			break;
		}
		case  PLUGIN_TO_PARENT:
		{
//			printQstring("-> " + debugger_struct->qsCurrentCommand);
			QStringList list = debugger_struct->qsCurrentCommand.split(":");
//			printQstring("UIdebugger debugger at : " + debugger_struct->qsCurrentCommand);
//	qDebug(QString("UIdebugger debugger at : " + debugger_struct->qsCurrentCommand).toAscii());
			emit debugerSignalAtLine(list.at(0), list.at(1).toInt());
			break;
		}
		default : fprintf(stdout,"PLUG SEND COMMAND IN plugAction unknow !\n");
	}
}	


void UIdebuger::onPluginMenuAction(QAction * action)
{
	if(plugin_WidgetList.contains(action->parentWidget()))
		stackedWidget->setCurrentWidget(action->parentWidget());
}


//************************************************


UIdebuger* UIdebuger::self( QWidget* parent )
{
	if ( !_self )
		_self = new UIdebuger( parent );
	return _self;
}
//
UIdebuger::UIdebuger( QWidget* parent )
	: QFrame( parent )
{
	setupUi( this );

	connect(butonContinue,SIGNAL(clicked()), this, SLOT(ondebugerContinue()));
	connect(butonNextStepOver,SIGNAL(clicked()),this,SLOT(ondebugerNextStepOver()));
	connect(butonNextStepInto,SIGNAL(clicked()), this, SLOT(ondebugerNextStepInto()));
	
	//  GDB driver
	pGdbCommand = NULL;

	butonContinue->setEnabled(false);
	butonNextStepInto->setEnabled(false);
	butonNextStepOver->setEnabled(false);

	// creation du menu contextuel
	debugerSetMenuContext();
	
	loadPlugins();	

	qpPlugSender =NULL;
}


// ============= START / STOP programme =========

void UIdebuger::debugerSetProgName(QString progName)
{
	m_progName =progName;
	// fix by P@sNox
	m_progName.replace( "\"", QString::null );
#ifdef Q_OS_WIN32
	if ( !m_progName.endsWith( ".exe", Qt::CaseInsensitive ) )
		m_progName.append( ".exe" );
#endif
//	qDebug(m_progName.toAscii());
}


void UIdebuger::debugerStart()
{
	if(!pGdbCommand)
	{
		if(QFile::exists( m_progName  ))
		{
			pGdbCommand = new GdbDriver;
			connect(pGdbCommand,SIGNAL(GdbInfo(QString)), this ,SLOT(ondebugerMsgFrom(QString)));
		
			// fai lui ouvrir l'executable
			pGdbCommand->setFile(m_progName);
			
			setBreakpointOnStart();
			
			// lance le programme
			pGdbCommand->setRun(m_progArgs);
			currentCommand.clear();
			
			emit debugerSignalStarted();
			for(int i=0; i<plugin_list.count(); i++)
				plugin_list.at(i)->pluginGdbStarted(true);
			gdbWorking->setText(tr("Debugger started ..."));
			debugerEnable(false);		// you can't stop gdb if working

		}
		else
		{
			QMessageBox::critical (this, tr("Error"), tr("Executable ") + m_progName + tr(" file no found."),QMessageBox::Ok,QMessageBox::NoButton);
			on_qActionDebugerStop();
		}
	}
	else
		QMessageBox::critical (this, tr("Error"), tr("Erreur intern de pointeur de process pGdbCommand"),QMessageBox::Ok,QMessageBox::NoButton);
}



void UIdebuger::debugerStop()
{
	if(pGdbCommand)
	{
		disconnect(pGdbCommand,SIGNAL(GdbInfo(QString)), this ,SLOT(ondebugerMsgFrom(QString)));
	
		pGdbCommand->quitGdb();
		pGdbCommand=NULL;
	
		butonContinue->setEnabled(false);
		butonNextStepInto->setEnabled(false);
		butonNextStepOver->setEnabled(false);

		gdbWorking->setText(tr("Debugger no started"));
		
		emit debugerSignalStoped();
		// efface la fleche bleu.
		emit debugerSignalAtLine("",-1);
		
		qpPlugSender = NULL;

		for(int i=0; i<plugin_list.count(); i++)
			plugin_list.at(i)->pluginGdbStarted(false);
	}
	else
		QMessageBox::critical (this, tr("Error"), tr("pointeur de process NULL"),QMessageBox::Ok,QMessageBox::NoButton);
}



// relance le programme et va au prochain breakpoint
void UIdebuger::ondebugerContinue()
{
	if(pGdbCommand)
	{
		butonContinue->setEnabled(false);
		butonNextStepInto->setEnabled(false);
		butonNextStepOver->setEnabled(false);
		gdbWorking->setText(tr("Debugger working ..."));
		qActionDebugerStop->setEnabled(false);

		pGdbCommand->setContinue();
	}
}




// effectu une seule commande et redonne la main
// dans le thread courant
// step over
void UIdebuger::ondebugerNextStepOver()
{
	if(pGdbCommand)
	{
		butonContinue->setEnabled(false);
		butonNextStepInto->setEnabled(false);
		butonNextStepOver->setEnabled(false);
		gdbWorking->setText(tr("Debugger working ..."));
		qActionDebugerStop->setEnabled(false);

		currentCommand.clear();
		currentCommand = "GET_NEXT_OVER";
		pGdbCommand->setNextStepOver();
	}
}


// step into
void UIdebuger::ondebugerNextStepInto()
{
	if(pGdbCommand)
	{
		butonContinue->setEnabled(false);
		butonNextStepInto->setEnabled(false);
		butonNextStepOver->setEnabled(false);
		gdbWorking->setText(tr("Debugger working ..."));
		qActionDebugerStop->setEnabled(false);

		currentCommand.clear();
		currentCommand = "GET_NEXT_INTO";
		pGdbCommand->setNextStepInto();
	}
}



//=========== DEBUGER TOOLS ===================


// msg venant de GDB
// fonction mere
// a chaque message de Gdb on appel cette fonction qui reagit

void UIdebuger::ondebugerMsgFrom(QString st)
{
static int iCurrentPlugin=0;

//	printQstring("on msg from data -> " + st);
//possible msg si pas mod debug
/*
Gdb > Reading symbols from C:/dev/2006.1.6.0-d-1.0.1/binary/monkey_win32.exe...(no debugging symbols found)...done.

Gdb >  
Gdb >  
Gdb > No symbol table is loaded.  Use the "file" command.
*/


	if( st.contains("exited "))
	{
		QMessageBox::information(0,tr("Information."), tr("Program finished."),QMessageBox::Ok);
		on_qActionDebugerStop(); // after it call debugerStop();
		return;
	}

	if( st.contains("no debugging symbols found"))
	{
		QMessageBox::information(0,tr("Information."), tr("Your project is not building in debug mode."),QMessageBox::Ok);
		on_qActionDebugerStop(); // after it call debugerStop();
		return;
	}
		
	// Gdb est sur un breakpoint
	// Breakpoint 1, main (argc=0x3, argv=0xbfe6ef84) at main.cpp:18
	if( (st.contains("Breakpoint") || st.contains("breakpoint") ) && !st.contains("file"))
	{
		
		plugin_struct.iCurrentCommand =   DEBUGGER_ON_BREAKPOINT;
//		butonContinue->setEnabled(true);
//		butonNextStepInto->setEnabled(true);
//		butonNextStepOver->setEnabled(true);
	}

	// gdb retourn l'index du breakpoint
	// Breakpoint 1 at 0x8052a27: file main.cpp, line 18.
	if( (st.contains("Breakpoint")  ) && st.contains("file"))
	{
//		currentCommand.clear();
//		currentCommand = "BREAKPOINT_INDEX";
		setBreakpointIndex(st);
//		return;
	}

	if(currentCommand ==  "GET_NEXT_OVER" || currentCommand == "GET_NEXT_INTO")
	{
		plugin_struct.iCurrentCommand =   DEBUGGER_STEP_OVER;
		currentCommand.clear();
//		butonContinue->setEnabled(true);
//		butonNextStepInto->setEnabled(true);
//		butonNextStepOver->setEnabled(true);

	}

	plugin_struct.qsDataFromGdb.clear();
	plugin_struct.qsDataFromGdb = st;

	if(qpPlugSender != NULL)
	{
		switch(qpPlugSender->pluginDataFromGdb(&plugin_struct))
		{
			case PLUGIN_TERMINED : 
			{
//				printQstring("one plugin termined");
				iCurrentPlugin++;
				plugin_struct.iPluginCommand =   -1;
				qpPlugSender = NULL;
				break;
			}
	
			case PLUGIN_SEND_COMMAND : 
			case PLUGIN_WAIT : 
			return;

			case PLUGIN_NOTIFY : 
				plugin_struct.iCurrentCommand = DEBUGGER_NOTIFY;
				plugin_struct.iPluginCommand =   -1;
				iCurrentPlugin = 0;
				qpPlugSender = NULL;
		}
	}
//	else printQstring("qsPluginSender == NULL");


	for( ; iCurrentPlugin<plugin_list.count(); )
	{
//		printQstring("plugin n: " + QString::number(iCurrentPlugin));
		switch( plugin_list.at(iCurrentPlugin)->pluginDataFromGdb(&plugin_struct) )
		{
			case PLUGIN_TERMINED : // plug termin
			{
//				printQstring("return Termin");
//				printQstring("plug termin pase au suivant plug");
				iCurrentPlugin++;
  				break;
			}
			
			case PLUGIN_WAIT : 
			{
//				printQstring("return Wait");
  				return ; // attend autre chainne
			}
			
			case PLUGIN_SEND_COMMAND : // ce plug demande une commande 
			{
//				printQstring("plug send comand return");
				return;
			}
			default : printQstring("datafromgdb return unknow");
		}
	}

	iCurrentPlugin=0;
	plugin_struct.iPluginCommand =   -1;
	plugin_struct.iCurrentCommand =   -1;

	butonContinue->setEnabled(true);
	butonNextStepInto->setEnabled(true);
	butonNextStepOver->setEnabled(true);

	gdbWorking->setText(tr("Debugger waitting you"));
	qActionDebugerStop->setEnabled(true);

//return;		
}



void UIdebuger::closeEvent( QCloseEvent* e )
{
	e->accept();
}


void UIdebuger::resizeEvent( QResizeEvent *e )
{
}


void QTableWidgetEx::mousePressEvent ( QMouseEvent * event ) 
{
	if( event->button()  == Qt::RightButton)
		UIdebuger::self()->mousePressEvent(event);
	else
		QTableWidget::mousePressEvent(event);//UIdebuger::self()->mousePressEvent(event);
}
