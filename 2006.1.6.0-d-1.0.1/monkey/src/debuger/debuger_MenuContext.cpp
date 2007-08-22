

#include "UIdebuger.h"
#include "UIdebugerArgs.h"

//#include <QMenu>
#include <QLabel>
 #include <QWidgetAction>

void UIdebuger::debugerEnable(bool enable)
{
	if(enable)
	{
		qActionDebugerStart->setEnabled(true);
		qActionDebugerStop->setEnabled(false);
//		leLog->setEnabled(true);
	}
	else
	{
		qActionDebugerStart->setEnabled(false);
		qActionDebugerStop->setEnabled(false);
//		leLog->setEnabled(false);
	}
	// force l'arret du debuger
	// si on ferme le projet alors que le debuger etait en fonction
//	debugerStop();
}



void UIdebuger::on_qActionDebugerArgs()
{
	UIdebugerArgs::self()->exec();
	m_progArgs = UIdebugerArgs::self()->GetArgs();
	
}

void UIdebuger::on_qActionDebugerStart()
{
	qActionDebugerStart->setEnabled(false);
	qActionDebugerStop->setEnabled(true);
	qActionDebugerArgs->setEnabled(false);
	debugerStart();
}

void UIdebuger::on_qActionDebugerStop()
{
	qActionDebugerStart->setEnabled(true);
	qActionDebugerStop->setEnabled(false);
	qActionDebugerArgs->setEnabled(true);
	debugerStop();
}

/*
void UIdebuger::on_qActionViewBreakpoints()
{
//	butonDeleteWatch->setEnabled(false);
//	butonAddWatch->setEnabled(false);
	
//	stackedWidget->setCurrentWidget(twBreakpoint);
	
}
*/


void UIdebuger::debugerSetMenuContext()
{

#if QT_VERSION >= 0x040200

	QLabel *lbRemote;
	QWidgetAction *qwaRemote;
	QLabel *lbPlugin;
	QWidgetAction *qwaPlugin;

	lbRemote = new QLabel("<html><b><i >Remote debugger</i></b></html>");
	lbRemote->setAlignment(Qt::AlignCenter);

	qwaRemote = new QWidgetAction(0);
	qwaRemote->setDefaultWidget(lbRemote);

	lbPlugin = new QLabel("<html><b><i >Plugins</i></b></html>");
	lbPlugin->setAlignment(Qt::AlignCenter);

	qwaPlugin = new QWidgetAction(0);
	qwaPlugin->setDefaultWidget(lbPlugin);

#endif
	
	menu = new QMenu("coucou",0);

	qActionDebugerStart = new QAction(QIcon( ":/Icons/Icons/buttonnext.png" ), tr("Debugger Start"),0);
	qActionDebugerStop = new QAction(QIcon( ":/Icons/Icons/projectcloseall.png" ),tr("Debugger Stop"),0);
	qActionDebugerArgs = new QAction(QIcon( ":/Icons/Icons/editclear.png" ),tr("Program arguments"),0);

	connect(qActionDebugerStart,SIGNAL(triggered()), SLOT(on_qActionDebugerStart()));
	connect(qActionDebugerStop,SIGNAL(triggered()), SLOT(on_qActionDebugerStop()));
	connect(qActionDebugerArgs,SIGNAL(triggered()), SLOT(on_qActionDebugerArgs()));
	
	qActionDebugerStop->setEnabled(false);

#if QT_VERSION >= 0x040200
	menu->addAction(qwaRemote);
#endif
	menu->addSeparator ();
	menu->addAction(qActionDebugerStart);
	menu->addAction(qActionDebugerStop);
	menu->addAction(qActionDebugerArgs);


	// juste pour voir
	QAction *qActionFullLog = new QAction("full log in file",this);
	qActionFullLog->setCheckable (true);
	QAction *qActionPlugins = new QAction("use plugins",this);
	qActionPlugins->setCheckable (true);
	QMenu *subMenu = menu->addMenu(QIcon( ":/Icons/Icons/editsettings.png" ),"Options");
	subMenu->addAction(qActionFullLog);
	qActionFullLog->setEnabled(false);
	subMenu->addAction(qActionPlugins);
	qActionPlugins->setEnabled(false);

	menu->addSeparator ();
#if QT_VERSION >= 0x040200
	menu->addAction(qwaPlugin);
	menu->addSeparator ();
#endif
}



void UIdebuger::mousePressEvent ( QMouseEvent * event )
{
	if( event->button()  == Qt::RightButton)
		menu->popup(QCursor::pos());
}

