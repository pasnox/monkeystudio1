//==================================
// Autor : Xiantia
// Main debuger frontend
//==================================

#include "UIdebuger.h"

//
#include <QCloseEvent>
#include <QMessageBox>
//
QPointer<UIdebugger> UIdebugger::_self = 0L;
//

// my QString print  just for debug
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

//	printQstring("loading " + QString::number(havePlug) + " plugins");

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


void plugin_manager::pluginNotifyGdbStarted(bool a)
{
		for(int i=0; i<qlPlugin.count(); i++)
			qlPlugin.at(i).plugin_Pointer->pluginGdbStarted(a);
}


void plugin_manager::pluginNotifyProjetOpened()
{
		for(int i=0; i<qlPlugin.count(); i++)
			qlPlugin.at(i).plugin_Pointer->pluginInitWidget();
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
		
//printQstring("Execute plug : " + qpPluginSender->pluginName());

		switch(qpPluginSender->pluginDataFromGdb(&plugin_struct_out))
		{
			case PLUGIN_TERMINED : 
			{
//printQstring(qpPluginSender->pluginName() + " return TERMINED");

				iCurrentPlugin++;
				plugin_struct_out.iPluginCommand =   -1;
				qpPluginSender = NULL;
				break;
			}
	
			case PLUGIN_SEND_COMMAND : 
			case PLUGIN_WAIT : 
//printQstring(qpPluginSender->pluginName() +" return WAIT or SEND_COMMAND");
			return;

			case PLUGIN_NOTIFY : 
//printQstring(qpPluginSender->pluginName() + "Plug return NOTIFY");
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

void UIdebugger::onPluginMouse(QMouseEvent *e)
{
	if( e->button()  == Qt::RightButton)
		menu->popup(QCursor::pos());
}

void UIdebugger::onPluginAction(UIdebugger_Plugin *plug_sender , struct UIdebugger_struct *debugger_struct )
{
	pluginDebuggerManager.setPluginSender( plug_sender);

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
				qActionDebuggerStop->setEnabled(false);

				pluginDebuggerManager.copyStruct(debugger_struct);
				
				if(pGdbCommand)
					pGdbCommand->setCommand(debugger_struct->qsCurrentCommand);
		//		pluginWatchDog.start(15000);
			}
			break;
		}

		case  PLUGIN_TO_PARENT:
		{
			QStringList list = debugger_struct->qsCurrentCommand.split(":");
			emit debuggerSignalAtLine(list.at(0), list.at(1).toInt());
			break;
		}
		default :	QMessageBox::critical (this, tr("Error"), tr("PLUGIN   SEND COMMAND IN plugAction unknow "),QMessageBox::Ok,QMessageBox::NoButton);
	}
}	



void UIdebugger::onPluginMenuAction(QAction * action)
{
	if(pluginDebuggerManager.contains(action->parentWidget()) != -1)
		stackedWidget->setCurrentWidget(action->parentWidget());
}

// NO USE
void UIdebugger::onPluginWatchDog()
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

UIdebugger* UIdebugger::self( QWidget* parent )
{
	if ( !_self )
		_self = new UIdebugger( parent );
	return _self;
}
//

UIdebugger::UIdebugger( QWidget* parent )
	: QFrame( parent )
{
	qActionDebuggerStart = new QAction(QIcon( ":/Icons/Icons/buttonnext.png" ), tr("Debugger Start"),this);
	qActionDebuggerStart->setObjectName("qActionDebuggerStart");
	qActionDebuggerStop = new QAction(QIcon( ":/Icons/Icons/projectcloseall.png" ),tr("Debugger Stop"),this);
	qActionDebuggerStop->setObjectName("qActionDebuggerStop");
	qActionDebuggerArgs = new QAction(QIcon( ":/Icons/Icons/editclear.png" ),tr("Program arguments"),this);
	qActionDebuggerArgs->setObjectName("qActionDebuggerArgs");
	qActionPluginSetting = new QAction(QIcon( ":/Icons/Icons/editsettings.png" ),tr("Plugin setting"),this);
	qActionPluginSetting->setObjectName("qActionPluginSetting");

	setupUi( this );

	butonContinue->setEnabled(false);
	butonNextStepInto->setEnabled(false);
	butonNextStepOver->setEnabled(false);

	connect(butonContinue,SIGNAL(clicked()), this, SLOT(ondebuggerContinue()));
	connect(butonNextStepOver,SIGNAL(clicked()),this,SLOT(ondebuggerNextStepOver()));
	connect(butonNextStepInto,SIGNAL(clicked()), this, SLOT(ondebuggerNextStepInto()));
	
	//  GDB driver
	pGdbCommand = NULL;
	// creation du menu contextuel
	debuggerSetMenuContext();
	// charge les plug
	pluginDebuggerManager.init();
	pluginDebuggerManager.loadPlugins(this,menu, stackedWidget);	
}


// ============= PUBLIC SLOT =============== =========

void UIdebugger::debuggerProjetOpened(bool open)
{
	// un projet est ouvert ?
	if(open)
	{
		// initialise les plugin
		pluginDebuggerManager.pluginNotifyProjetOpened();
		qActionDebuggerStart->setEnabled(true);
		qActionDebuggerStop->setEnabled(false);
	}
	else
	{
		qActionDebuggerStart->setEnabled(false);
		qActionDebuggerStop->setEnabled(false);
		// force stop debugger si il est en fonction et que l'on 
		// ferme le projet
		debuggerStop();
	}
}


void UIdebugger::debuggerSetProgName(QString progName)
{
	m_progName =progName;
	// fix by P@sNox
	m_progName.replace( "\"", QString::null );
#ifdef Q_OS_WIN32
	if ( !m_progName.endsWith( ".exe", Qt::CaseInsensitive ) )
		m_progName.append( ".exe" );
#endif
}

//=================== PRIVATE FUNCTION ================

void UIdebugger::debuggerStart()
{
	if(!pGdbCommand)
	{
		if(QFile::exists( m_progName  ))
		{
			pluginDebuggerManager.init();
			currentCommand.clear();

			// lance le debugger
			gdbWorking->setText(tr("Gdb started ..."));

			pGdbCommand = new GdbDriver;
			connect(pGdbCommand,SIGNAL(GdbInfo(QString)), this ,SLOT(onDataFromGdb(QString)));

			// fai lui ouvrir l'executable
			pGdbCommand->setFile(m_progName);
			
			// notify les plug que le debugger est lancer
			pluginDebuggerManager.pluginNotifyGdbStarted(true);

			// send les breakpoints qui sont present avant le lancement du debugger			
			setBreakpointOnStart();

			// lance le programme
			pGdbCommand->setRun(m_progArgs);

			// notify au parent que le debugger est lancer
			emit debuggerSignalStarted();

			// grise les menu start / stop debugger
			qActionDebuggerStart->setEnabled(false);
			qActionDebuggerStop->setEnabled(true);
		}
		else
		{
			QMessageBox::critical (this, tr("Error"), tr("Executable ") + m_progName + tr(" file no found."),QMessageBox::Ok,QMessageBox::NoButton);
			on_qActionDebuggerStop_triggered();
		}
	}
//	else
//		QMessageBox::critical (this, tr("Error"), tr("Erreur interne !  Pointeur de process pGdbCommand non NULL"),QMessageBox::Ok,QMessageBox::NoButton);
}



void UIdebugger::debuggerStop()
{
	if(pGdbCommand)
	{
//		qDebug("enter stop");
		disconnect(pGdbCommand,SIGNAL(GdbInfo(QString)), this ,SLOT(onDataFromGdb(QString)));
	
		pGdbCommand->quitGdb();
		pGdbCommand=NULL;
	
		butonContinue->setEnabled(false);
		butonNextStepInto->setEnabled(false);
		butonNextStepOver->setEnabled(false);

		gdbWorking->setText(tr("Gdb no started"));
		
		emit debuggerSignalStoped();

		// efface la fleche bleu.
		emit debuggerSignalAtLine("",-1);
	
		// notify all plug Gdb stoped
		pluginDebuggerManager.pluginNotifyGdbStarted(false);
	
//		qDebug("end stop");
	}
//	else
//		QMessageBox::critical (this, tr("Error"), tr("Erreur interne ! Pointeur de process Gdb NULL"),QMessageBox::Ok,QMessageBox::NoButton);
}



// relance le programme et va au prochain breakpoint
void UIdebugger::ondebuggerContinue()
{
	if(pGdbCommand)
	{
		butonContinue->setEnabled(false);
		butonNextStepInto->setEnabled(false);
		butonNextStepOver->setEnabled(false);

		gdbWorking->setText(tr("Gdb working ..."));
		qActionDebuggerStop->setEnabled(false);

		pGdbCommand->setContinue();
	}
}


// effectu une seule commande et redonne la main
// dans le thread courant
// step over
void UIdebugger::ondebuggerNextStepOver()
{
	if(pGdbCommand)
	{
		butonContinue->setEnabled(false);
		butonNextStepInto->setEnabled(false);
		butonNextStepOver->setEnabled(false);

		gdbWorking->setText(tr("Gdb working ..."));
		qActionDebuggerStop->setEnabled(false);

		currentCommand = "GET_NEXT_OVER";
		pGdbCommand->setNextStepOver();
	}
}


// step into
void UIdebugger::ondebuggerNextStepInto()
{
	if(pGdbCommand)
	{
		butonContinue->setEnabled(false);
		butonNextStepInto->setEnabled(false);
		butonNextStepOver->setEnabled(false);

		gdbWorking->setText(tr("Gdb working ..."));
		qActionDebuggerStop->setEnabled(false);

		currentCommand = "GET_NEXT_INTO";
		pGdbCommand->setNextStepInto();
	}
}



// msg venant de GDB
// fonction mere
// a chaque message de Gdb on appel cette fonction qui reagit

void UIdebugger::onDataFromGdb(QString st)
{

	/*
	Program termined
	gdb -> Program exited normaly.
	
	gdb for windows 5.2.1, i686-pc-mingw32 (ok)
	gdb for linux
	*/

	if( st.contains("Program exited "))
	{
		QMessageBox::information(0,tr("Information."), tr("Program finished."),QMessageBox::Ok);
		on_qActionDebuggerStop_triggered(); // after it call debugerStop();
		return;
	}

	/*
	Program no compiled with debug option
	gdb ->  Reading symbols from /home/dev/..... no debugging symbols found

	gdb for windows 5.2.1, i686-pc-mingw32 (ok)
	gdb for linux
	*/

	if( st.contains("no debugging symbols found")) 
	{
		QMessageBox::information(this,tr("Information."), tr("Your project is not building in debug mode. ! "),QMessageBox::Ok);
		on_qActionDebuggerStop_triggered(); // after it call debugerStop();
		return;
	}

	/*
	The code source is more recent than program
	gdb -> warning: Source file is more recent than executable

	gdb for windows 5.2.1, i686-pc-mingw32 (ok)
	gdb for linux
	*/

	if(st.contains("warning: Source file is more recent than executable"))
	{
		QMessageBox::information(this,tr("Information."), tr("Your source file is more recent than executable ! \nBuild it befort."),QMessageBox::Ok);
		on_qActionDebuggerStop_triggered(); // after it call debugerStop();
		return;
	}
		
	/* 
	Gdb est sur un breakpoint
	gdb -> Breakpoint 1, main (argc=0x3, argv=0xbfe6ef84) at main.cpp:18
	gdb -> Breakpoint 1, boucle (i=0) at src/main.cpp:24

	gdb for windows 5.2.1, i686-pc-mingw32 (ok)
	gdb for linux
	*/

	if( st.contains("Breakpoint")  && st.contains("at") && !st.contains("file"))
	 	pluginDebuggerManager.setCurrentCommand(DEBUGGER_ON_BREAKPOINT);

	/* 
	Gdb retourn l'index du breakpoint
	Breakpoint 1 at 0x8052a27: file main.cpp, line 18. (gentoo)
	Breakpoint 2 at 0x804a39b: file src/main.cpp, line 26. (kubuntu)

	gdb for windows 5.2.1, i686-pc-mingw32 (ok)
	gdb for linux
	*/

	if( (st.contains("Breakpoint")  ) && st.contains("at") && st.contains("file") && st.contains("line"))
		setBreakpointIndex(st);

	// l'utilisateur execute un pas suivant
	if(currentCommand ==  "GET_NEXT_OVER" || currentCommand == "GET_NEXT_INTO")
		pluginDebuggerManager.setCurrentCommand(DEBUGGER_STEP_OVER);

	/*
	Gestion des plugins
	*/

	pluginDebuggerManager.execute(st);

	butonContinue->setEnabled(true);
	butonNextStepInto->setEnabled(true);
	butonNextStepOver->setEnabled(true);

	gdbWorking->setText(tr("Gdb waitting you"));
	qActionDebuggerStop->setEnabled(true);
}


void UIdebugger::mousePressEvent ( QMouseEvent * event )
{
	if( event->button()  == Qt::RightButton)
		menu->popup(QCursor::pos());
}


void UIdebugger::closeEvent( QCloseEvent* e )
{
	e->accept();
}

