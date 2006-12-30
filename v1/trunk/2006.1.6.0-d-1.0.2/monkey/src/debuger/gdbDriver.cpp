//======================================
// Autor : Xiantia
//
//CONTROLER GDB
//======================================


// INFO : pas de bug trouv


#include "gdbDriver.h"
#include <QMessageBox>
#include <QTime>



// logger tout les message de gdb
#define LOG_GDB	false

#ifdef Q_OS_WIN32
#define NAME_FILE_LOG	"c:/log_gdb.txt"
#else
#define NAME_FILE_LOG	"/home/yannick/log_gdb.txt"
#endif

// prompt connu de GDB
QList <QString> PROMPT_GDB = QStringList() << "(gdb) " << "gdb> ";
QTime mytime;

extern void printQstring(QString str);


GdbDriver::GdbDriver(  )
{
	
	// fix by Xiantia
	GdbName = "gdb";
	#ifdef Q_OS_WIN32
		if ( !	GdbName.endsWith( ".exe", Qt::CaseInsensitive ) )
			GdbName.append( ".exe" );
	#endif

	GdbIdl=false;
	gdbStarted = false;

	mpGdbProcess = new QProcess(this);
	connect(mpGdbProcess,SIGNAL(readyRead()/*StandardOutput()*/),this, SLOT(gdbFinishCommandStd()));
//	connect(mpGdbProcess,SIGNAL(readyReadStandardError()),this, SLOT(gdbFinishCommandErr()));

	connect(this,SIGNAL(signalGdbCommandEnd()),this,SLOT(slotGdbCommandend()));

	connect(mpGdbProcess,SIGNAL(error(QProcess::ProcessError )),this,SLOT(OnError(QProcess::ProcessError )));

	connect(mpGdbProcess,SIGNAL(finished ( int , QProcess::ExitStatus  )),this,SLOT(gdbfinished ( int , QProcess::ExitStatus  )));

	file_log_gdb = NULL;
	if(LOG_GDB)
	{
		file_log_gdb = new QFile(NAME_FILE_LOG );
			if (!file_log_gdb->open(QIODevice::Append)) file_log_gdb = NULL;
	}
	mpGdbProcess->setReadChannelMode( QProcess::MergedChannels );
	mpGdbProcess->start(	GdbName);
}


GdbDriver::~GdbDriver(  )
{
	if(LOG_GDB && file_log_gdb) file_log_gdb->close();

	delete mpGdbProcess;
}



void GdbDriver::gdbfinished ( int a, QProcess::ExitStatus  b)
{
// no warning
a=a;
b=b;

//qDebug("Gdb termin");
	delete this;
}


void GdbDriver::slotGdbCommandend()
{
	int numberCommand = mCommandList.count();
	if(numberCommand != 0)
	{
//		qDebug("nombre de commandes : " + QString::number(numberCommand).toAscii() + " " +mCommandList.value(0).toAscii());
		GdbIdl = false;
		if(LOG_GDB && file_log_gdb)
		{
			QTextStream out(file_log_gdb);
 			out << QString( "start send command -> temps ecoule apres le signal " + QString::number(mytime.elapsed()) +"\r\n" + mCommandList.value(0)).toAscii() ;
			out.flush();
		}
		mytime.restart();
//		printQstring(mCommandList.value(0));
		mpGdbProcess->write(mCommandList.value(0).toAscii());
		mCommandList. removeAt(0);
	}
}

void GdbDriver::OnError(QProcess::ProcessError er)
{
	switch(er)
	{
		case QProcess::FailedToStart : QMessageBox::critical(0,tr("Error"), tr("Can't start ") + GdbName + tr("\nMake sur you haved this in your path"), QMessageBox::Ok, QMessageBox::NoButton);
		break;
		default : QMessageBox::critical(0,tr("Error"), tr("Gdb have generate a unknow error"), QMessageBox::Ok, QMessageBox::NoButton);

	}
}

void GdbDriver::setCommand(QString command)
{
	if(!command.isEmpty())
		 mCommandList << command +"\r\n";

	if(GdbIdl && gdbStarted)
	{
		mytime.restart();
		GdbIdl = false;
		emit signalGdbCommandEnd();
	}
}


int GdbDriver::gdbAutoPrompt(QString st)
{
	for(int i=0; i<PROMPT_GDB.count(); i++)
	{
		if( st.contains(PROMPT_GDB.at(i)))
			return i;
	}
	return 0;
}

void GdbDriver::gdbFinishCommandStd()
{
	QByteArray tes= mpGdbProcess->readAll();//StandardOutput();
	QTextStream out(file_log_gdb);
	
	
	st.append(QString::fromLocal8Bit (tes));

//printQstring(st);

	/* 
	On start gdb
	*/

	if(st.contains("This GDB was configured as"))
	{
		if(LOG_GDB && file_log_gdb)
		{
			out << "gdb Sarted OK\r\n";
			out.flush();
		}
		gdbStarted = true;
		GdbIdl = true;	
		emit signalGdbCommandEnd();
		st.clear();

		return;
	}

	/*
	when you quit gdb befort program finished
	gdb tell you "Exit anyway? y, n"
	*/

	if(st.contains("Exit anyway?"))
	{
		setCommand("y");
		GdbIdl = true;
		st.clear();
		emit signalGdbCommandEnd();
		return;
	}

	/*
	under linux
	when you want debugging program that no have debug option.
	when you send "b main.cpp:9" , gdb tell you 
	"No source file named main.cpp.
	Make breakpoint pending on future shared library load? (y or [n])
	*/

/*	if(st.contains("Make breakpoint pending on future shared library load"))
	{
		printQstring("demande question");
		setCommand("n");
		GdbIdl = true;		
		emit GdbInfo(st.remove("(gdb) "));		
		st.clear();
		emit signalGdbCommandEnd();
		return;
	}
*/
	if(st.contains/*endsWith*/("(gdb) "))
	{
		if(LOG_GDB && file_log_gdb)
		{
			out << "---> temps de reponce " + QString::number(mytime.elapsed()) + "\r\n" + st.toAscii() + "\r\n---<\r\n";
			out.flush();
		}
		emit GdbInfo(st.remove("(gdb) "));
		st.clear();
		GdbIdl = true;		

		emit signalGdbCommandEnd();
		return;
	}
	//	out << "---> UN TRUC  " + st.toAscii() +"\r\n";
	//	out.flush();

return;

// last version ->
// encienne version ->
	if(st.contains("gdb") && gdbStarted)
	{
		QStringList  msglist = st.split(PROMPT_GDB.at(gdbAutoPrompt(st)),QString::SkipEmptyParts);

		if(msglist.count())
		{
			for(int i=0; i<msglist.count(); i++)
			{
				emit GdbInfo(msglist.at(i));	

				if(LOG_GDB && file_log_gdb)
				{
	//				out << QString( "temps -> " + QString::number(mytime.elapsed()) + "\r\n" +  msglist.at(i) ).toAscii();
				}
			}
			GdbIdl = true;		

			emit signalGdbCommandEnd();
			st.clear();
		}
	}
}

// no use
void GdbDriver::gdbFinishCommandErr()
{
	QByteArray tes= mpGdbProcess->readAllStandardError();
	st.append(QString::fromLocal8Bit (tes));
	QTextStream out(file_log_gdb);

//		out << "---> UNE Erreur   " + st.toAscii() +"\r\n";
//		out.flush();
//		st.clear();

printQstring("err -> " + st);
	
	if(st.contains("\n"))
	{
//		qDebug("erreur");
		emit GdbInfo(st);
		GdbIdl = true;		

		emit signalGdbCommandEnd();
		st.clear();
	}
}
