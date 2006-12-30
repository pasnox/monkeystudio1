

#include "UIdebuger.h"
#include "UIdebugerArgs.h"
#include <QMessageBox>

//#include <QMenu>
#include <QLabel>
#if QT_VERSION >= 0x040200
 #include <QWidgetAction>
#endif
/*
void UIdebuger::debugerEnable(bool enable)
{
	if(enable)
		qActionDebugerStart->setEnabled(true);
	else
		qActionDebugerStart->setEnabled(false);

	qActionDebugerStop->setEnabled(false);
}
*/


void UIdebugger::on_qActionDebuggerArgs_triggered()
{
	UIdebugerArgs::self()->exec();
	m_progArgs = UIdebugerArgs::self()->GetArgs();
	
}

void UIdebugger::on_qActionDebuggerStart_triggered()
{
	qActionDebuggerStart->setEnabled(false);
	qActionDebuggerStop->setEnabled(true);
	qActionDebuggerArgs->setEnabled(false);
	debuggerStart();
}

void UIdebugger::on_qActionDebuggerStop_triggered()
{
	qActionDebuggerStart->setEnabled(true);
	qActionDebuggerStop->setEnabled(false);
	qActionDebuggerArgs->setEnabled(true);
	debuggerStop();
}

void UIdebugger::on_qActionPluginSetting_triggered()
{
	// recherche le plugin courant
	QWidget *w = stackedWidget->currentWidget();
	int i = pluginDebuggerManager.contains(w);
	if(i!=-1)
	{
		// ce plug a t-il une configuration possible ?
		QDialog *d = pluginDebuggerManager.plugin(i)->pluginSetting();
		if(d!=NULL) 
			d->exec();
		else
			QMessageBox::information(0,pluginDebuggerManager.plugin(i)->pluginName(), tr("This plugin don't have a setting. ! "),QMessageBox::Ok);
	}
}


void UIdebugger::debuggerSetMenuContext()
{

#if QT_VERSION >= 0x040200

	QLabel *lbRemote;
	QWidgetAction *qwaRemote;
	QLabel *lbPlugin;
	QWidgetAction *qwaPlugin;

	lbRemote = new QLabel("<html><b><i >Remote debugger 1.0.2</i></b></html>");
	lbRemote->setAlignment(Qt::AlignCenter);

	qwaRemote = new QWidgetAction(0);
	qwaRemote->setDefaultWidget(lbRemote);

	lbPlugin = new QLabel("<html><b><i >Plugins</i></b></html>");
	lbPlugin->setAlignment(Qt::AlignCenter);

	qwaPlugin = new QWidgetAction(0);
	qwaPlugin->setDefaultWidget(lbPlugin);

#endif
	
	menu = new QMenu("coucou",0);

	qActionDebuggerStop->setEnabled(false);


#if QT_VERSION >= 0x040200
	menu->addAction(qwaRemote);
#endif
	menu->addSeparator ();
	menu->addAction(qActionDebuggerStart);
	menu->addAction(qActionDebuggerStop);
	menu->addAction(qActionDebuggerArgs);
	menu->addAction(qActionPluginSetting);

	// juste pour voir
/*
	QAction *qActionFullLog = new QAction("full log in file",this);
	qActionFullLog->setCheckable (true);
	QAction *qActionPlugins = new QAction("use plugins",this);
	qActionPlugins->setCheckable (true);
	QMenu *subMenu = menu->addMenu(QIcon( ":/Icons/Icons/editsettings.png" ),"Plugin setting");

	subMenu->setEnabled(false);
	subMenu->addAction(qActionFullLog);
//	qActionFullLog->setEnabled(false);
	subMenu->addAction(qActionPlugins);
//	qActionPlugins->setEnabled(false);
*/
	menu->addSeparator ();

#if QT_VERSION >= 0x040200
	menu->addAction(qwaPlugin);
	menu->addSeparator ();
#endif
}




