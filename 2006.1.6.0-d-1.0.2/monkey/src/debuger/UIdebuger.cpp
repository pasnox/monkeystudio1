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

// ===================== PLUGIN MANAGER ===================

void plugin_manager::loadPlugins(QObject * parent,QMenu *contextualMenu , QStackedWidget *stackedWidget)
{
int havePlug=0;

	// va au repertoire
	QDir pluginsDir = QDir(qApp->applicationDirPath());
	pluginsDir.cd("plugins");

	// list les fichiers presents
	foreach (QString fileName, pluginsDir.entryList(QDir::Files))
	{
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        		QObject *op = loader.instance();
        		if (op) 
		{
			UIdebugger_Plugin * pPlugin = qobject_cast<UIdebugger_Plugin *>(op);
            			if (pPlugin) 
			{
				QWidget * pWidget = pPlugin->pluginGetWidget();
				addPlugin(pPlugin, pWidget);
			
				// le plug a un interface graphique ?
				if(pWidget != NULL)	// add on menu
				{
					havePlug++;
					QAction *ac = new QAction(pPlugin->pluginMenuIcon(),pPlugin->pluginMenuName(),pWidget);
					contextualMenu->addAction(ac);//op->pluginMenuName());
					stackedWidget->addWidget(pWidget);
					connect(op,SIGNAL(signalPluginMouse(QMouseEvent * )),parent,SLOT(onPluginMouse(QMouseEvent *)));
				}
				connect(op,SIGNAL(signalPluginAction( UIdebugger_Plugin *, UIdebugger_struct *  )),parent,SLOT(onPluginAction( UIdebugger_Plugin * ,UIdebugger_struct *)));
			}
            		}
        	}
	connect(contextualMenu,SIGNAL(triggered ( QAction * )), parent,SLOT(onPluginMenuAction(QAction * )));

	printQstring("loading " + QString::number(havePlug) + " plugins");

	if(havePlug) 	
	{
		// find first plug have widget
		for(int i=0; i< qlPlugin.count(); i++)
		{
			if(qlPlugin.at(i).plugin_Widget != NULL)
			{
				stackedWidget->setCurrentWidget(qlPlugin.at(i).plugin_Widget);
				return;
			}
		}
	}
}				


void plugin_manager::copyStruct(UIdebugger_struct *src)
{
	plugin_struct_out.qsCurrentCommand.clear();
	plugin_struct_out.qsCurrentCommand = src->qsCurrentCommand;
	plugin_struct_out.iCurrentCommandType = src->iCurrentCommandType;
	plugin_struct_out.iPluginCommand = src->iPluginCommand;
}


void plugin_manager::pluginNotify(bool a)
{
		for(int i=0; i<qlPlugin.count(); i++)
			qlPlugin.at(i).plugin_Pointer->pluginGdbStarted(a);
}


void plugin_manager::addPlugin(UIdebugger_Plugin *plugin, QWidget *widget)
{
struct plugin_list  pl;

	pl.plugin_Pointer =  plugin;
	pl.plugin_Widget = widget;
	qlPlugin << pl;
}


int plugin_manager::contains(QWidget *w)
{
	for(int i=0 ; i<qlPlugin.count(); i++)
	{
		if(qlPlugin.at(i).plugin_Widget == w && qlPlugin.at(i).plugin_Widget!=NULL)
			return i;
	}
	return -1;
}


void plugin_manager::execute(QString st)
{
	// gestion des plugins
	plugin_struct_out.qsDataFromGdb.clear();
	plugin_struct_out.qsDataFromGdb = st;


	if(qpPluginSender != NULL)
	{
		
printQstring("Execute plug : " + qpPluginSender->pluginName());

		switch(qpPluginSender->pluginDataFromGdb(&plugin_struct_out))
		{
			case PLUGIN_TERMINED : 
			{
printQstring(qpPluginSender->pluginName() + " return TERMINED");

				iCurrentPlugin++;
				plugin_struct_out.iPluginCommand =   -1;
				qpPluginSender = NULL;
				break;
			}
	
			case PLUGIN_SEND_COMMAND : 
			case PLUGIN_WAIT : 
printQstring(qpPluginSender->pluginName() +" return WAIT or SEND_COMMAND");
			return;

			case PLUGIN_NOTIFY : 
printQstring(qpPluginSender->pluginName() + "Plug return NOTIFY");
				plugin_struct_out.iCurrentCommand = DEBUGGER_NOTIFY;
				plugin_struct_out.iPluginCommand =   -1;
				iCurrentPlugin = 0;
				qpPluginSender = NULL;
		}
	}
	for( ; iCurrentPlugin<qlPlugin.count(); )
	{
		switch( qlPlugin.at(iCurrentPlugin).plugin_Pointer->pluginDataFromGdb(&plugin_struct_out) )
		{
			case PLUGIN_TERMINED : // plug termine
			{
				iCurrentPlugin++;
  				break;
			}

			
			case PLUGIN_WAIT :  // attend la suite
			case PLUGIN_SEND_COMMAND : return; // ce plug demande une commande 
		}
	}
	init();
}


// ==================== SLOT des Plugin ======================

void UIdebuger::onPluginMouse(QMouseEvent *e)
{
	if( e->button()  == Qt::RightButton)
		menu->popup(QCursor::pos());
}

void UIdebuger::onPluginAction(UIdebugger_Plugin *plug_sender , struct UIdebugger_struct *debugger_struct )
{
	pluginManager.setPluginSender( plug_sender);

//printQstring(plug_sender->pluginName());
//printQstring(debugger_struct->qsCurrentCommand);

	switch(debugger_struct->iCurrentCommandType)
	{
		case  PLUGIN_TO_GDB:
		{
			if(!debugger_struct->qsCurrentCommand.isEmpty())
			{
				butonContinue->setEnabled(false);
				butonNextStepInto->setEnabled(false);
				butonNextStepOver->setEnabled(false);
				gdbWorking->setText(tr("Gdb working ..."));
				qActionDebugerStop->setEnabled(false);

				pluginManager.copyStruct(debugger_struct);
				
				if(pGdbCommand)
					pGdbCommand->setCommand(debugger_struct->qsCurrentCommand);
		//		pluginWatchDog.start(15000);
			}
			break;
		}

		case  PLUGIN_TO_PARENT:
		{
			QStringList list = debugger_struct->qsCurrentCommand.split(":");
			emit debugerSignalAtLine(list.at(0), list.at(1).toInt());
			break;
		}
		default : printQstring("PLUGIN   SEND COMMAND IN plugAction unknow !\n");
	}
}	



void UIdebuger::onPluginMenuAction(QAction * action)
{
	if(pluginManager.contains(action->parentWidget()) != -1)
		stackedWidget->setCurrentWidget(action->parentWidget());
}


void UIdebuger::onPluginWatchDog()
{
/*	QString str = "Debugger frontend have detected time out.\n\nPlugin name : " + qpPlugSender->pluginName() + \
		"\nLast Command : " + plugin_struct.qsCurrentCommand + \
		"\nLast data from Gdb : " + plugin_struct.qsDataFromGdb + \
		"\nSend to : " + ((plugin_struct.iCurrentCommandType == PLUGIN_TO_GDB) ? "Gdb":"Debugger frontend" ) + \
		"\nYour personnal command number : " +  QString::number(plugin_struct.iPluginCommand);

	QMessageBox::critical (this, tr("Plugin TimeOut"),str,QMessageBox::Ok,QMessageBox::NoButton);
	// il y un plug qui merde , stop le debugger correctement
	on_qActionDebugerStop();
*/}



// ======================== UI_ DEBUGGER ============

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
	qActionDebugerStart = new QAction(QIcon( ":/Icons/Icons/buttonnext.png" ), tr("Debugger Start"),this);
	qActionDebugerStart->setObjectName("qActionDebugerStart");
	qActionDebugerStop = new QAction(QIcon( ":/Icons/Icons/projectcloseall.png" ),tr("Debugger Stop"),this);
	qActionDebugerStop->setObjectName("qActionDebugerStop");
	qActionDebugerArgs = new QAction(QIcon( ":/Icons/Icons/editclear.png" ),tr("Program arguments"),this);
	qActionDebugerArgs->setObjectName("qActionDebugerArgs");
	qActionPluginSetting = new QAction(QIcon( ":/Icons/Icons/editsettings.png" ),tr("Plugin setting"),this);
	qActionPluginSetting->setObjectName("qActionPluginSetting");

	setupUi( this );

	butonContinue->setEnabled(false);
	butonNextStepInto->setEnabled(false);
	butonNextStepOver->setEnabled(false);

	connect(butonContinue,SIGNAL(clicked()), this, SLOT(ondebugerContinue()));
	connect(butonNextStepOver,SIGNAL(clicked()),this,SLOT(ondebugerNextStepOver()));
	connect(butonNextStepInto,SIGNAL(clicked()), this, SLOT(ondebugerNextStepInto()));
	
	//  GDB driver
	pGdbCommand = NULL;
	// creation du menu contextuel
	debugerSetMenuContext();
	// charge les plug
	pluginManager.init();
	pluginManager.loadPlugins(this,menu, stackedWidget);	

	// watchDog des plug
	pluginWatchDog.setSingleShot (true);
	connect(&pluginWatchDog, SIGNAL(timeout()), this, SLOT(onPluginWatchDog()));
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
}


void UIdebuger::debugerStart()
{
	if(!pGdbCommand)
	{
		if(QFile::exists( m_progName  ))
		{
			pluginManager.init();
//			pluginManager.debuggerStartUp = true;
			currentCommand.clear();

			// lance le debugger
			gdbWorking->setText(tr("Gdb started ..."));

			pGdbCommand = new GdbDriver;
			connect(pGdbCommand,SIGNAL(GdbInfo(QString)), this ,SLOT(ondebugerMsgFrom(QString)));

			// fai lui ouvrir l'executable
			pGdbCommand->setFile(m_progName);
			// send les breakpoints qui sont present avant le lancement du debugger			
			
			// notify les plug que le debugger est lancer
			pluginManager.pluginNotify(true);

//			pluginManager.setCurrentCommand(DEBUGGER_BEFORT_START);
//			pluginManager.execute("");

			setBreakpointOnStart();
			// lance le programme
			pGdbCommand->setRun(m_progArgs);
			// notify au parent que le debugger est lancer
			emit debugerSignalStarted();

			// grise les menu start / stop debugger
			debugerEnable(false);		// you can't stop gdb if working


		}
		else
		{
			QMessageBox::critical (this, tr("Error"), tr("Executable ") + m_progName + tr(" file no found."),QMessageBox::Ok,QMessageBox::NoButton);
			on_qActionDebugerStop_triggered();
		}
	}
	else
		QMessageBox::critical (this, tr("Error"), tr("Erreur interne !  Pointeur de process pGdbCommand non NULL"),QMessageBox::Ok,QMessageBox::NoButton);
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

		gdbWorking->setText(tr("Gdb no started"));
		
		emit debugerSignalStoped();
		// efface la fleche bleu.
		emit debugerSignalAtLine("",-1);
	
		pluginManager.debuggerStartUp = false;
		pluginManager.pluginNotify(false);
	}
	else
		QMessageBox::critical (this, tr("Error"), tr("Erreur interne ! Pointeur de process Gdb NULL"),QMessageBox::Ok,QMessageBox::NoButton);
}



// relance le programme et va au prochain breakpoint
void UIdebuger::ondebugerContinue()
{
	if(pGdbCommand)
	{
		butonContinue->setEnabled(false);
		butonNextStepInto->setEnabled(false);
		butonNextStepOver->setEnabled(false);

		gdbWorking->setText(tr("Gdb working ..."));
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

		gdbWorking->setText(tr("Gdb working ..."));

		qActionDebugerStop->setEnabled(false);

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

		gdbWorking->setText(tr("Gdb working ..."));

		qActionDebugerStop->setEnabled(false);

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
//printQstring(st);

	if( st.contains("exited "))
	{
		QMessageBox::information(0,tr("Information."), tr("Program finished."),QMessageBox::Ok);
		on_qActionDebugerStop_triggered(); // after it call debugerStop();
		return;
	}

	if( st.contains("no debugging symbols found"))
	{
		QMessageBox::information(0,tr("Information."), tr("Your project is not building in debug mode. ! "),QMessageBox::Ok);
		on_qActionDebugerStop_triggered(); // after it call debugerStop();
		return;
	}
		
	// Gdb est sur un breakpoint
	// Breakpoint 1, main (argc=0x3, argv=0xbfe6ef84) at main.cpp:18
	// Breakpoint 1, boucle (i=0) at src/main.cpp:24
	
	if( (st.contains("Breakpoint") || st.contains("breakpoint") ) && !st.contains("file"))// && !st.contains("toggle"))
	{
		qDebug(st.toAscii());
		pluginManager.setCurrentCommand(DEBUGGER_ON_BREAKPOINT);
	}
	// gdb retourn l'index du breakpoint
	// Breakpoint 1 at 0x8052a27: file main.cpp, line 18. (gentoo)
	// Breakpoint 2 at 0x804a39b: file src/main.cpp, line 26. (kubuntu)

	if( (st.contains("Breakpoint")  ) && st.contains("at") && st.contains("file") && st.contains("line"))
		setBreakpointIndex(st);

	// l'utilisateur execute un pas suivant
	if(currentCommand ==  "GET_NEXT_OVER" || currentCommand == "GET_NEXT_INTO")
		pluginManager.setCurrentCommand(DEBUGGER_STEP_OVER);

//	if( st.contains("toggleBreakpoint:"))
//		pluginManager.setCurrentCommand(DEBUGGER_NEW_BREAKPOINT);

	// gestion des plugins
	pluginManager.execute(st);

/*	if(pluginManager.debuggerStartUp)
	{
		pluginManager.debuggerStartUp = false;

		pGdbCommand->setRun(m_progArgs);
		// notify au parent que le debugger est lancer
		emit debugerSignalStarted();
		// grise les menu start / stop debugger
		debugerEnable(false);		// you can't stop gdb if working
	}
*/	
//	if(pGdbCommand)
//	{
		butonContinue->setEnabled(true);
		butonNextStepInto->setEnabled(true);
		butonNextStepOver->setEnabled(true);

		gdbWorking->setText(tr("Gdb waitting you"));
		qActionDebugerStop->setEnabled(true);
//	}

}

void UIdebuger::mousePressEvent ( QMouseEvent * event )
{
	if( event->button()  == Qt::RightButton)
		menu->popup(QCursor::pos());
}


void UIdebuger::closeEvent( QCloseEvent* e )
{
	e->accept();
}

