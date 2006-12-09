#include <QThread>
#include <QProcess>
#include <QFile>
#include <QTextStream>


class GdbDriver : public QObject
{
	Q_OBJECT
public:
	GdbDriver();
	~GdbDriver();
	QProcess *mpGdbProcess;
	
//	void run();
	void quitGdb() { setCommand("stop\r\nquit"); }

	// ouvre le fichier a debuger
	void setFile(QString progpath)
		{setCommand("file " + QString(progpath) );  }

// Breakpoint
	// ajoute un breackpoint
	void setBreakpoint(QString breakpoint_at)
		{setCommand(breakpoint_at);}

	// list les breakpoints
	void getBreakpointInfo()
		{setCommand("info break");}

// controle
	void setRun(QString args)
		{setCommand("run " + args );}

	void setNextStepInto()
		{setCommand("step");}

	void setNextStepOver()
		{setCommand("next");}

	void setContinue()
		{setCommand("c");}

// variables	
	// retourne la valeur d'une variable
	void getVariableValue(QString variable)
		{setCommand("p " + variable );}

	// retourne le type d'une varirable
	void getVariableType(QString variable)
		{setCommand("whatis "+ variable );}

	void getVariableAdresse(QString variable)
	{

		if(variable.at(0) == '*')
		{
			setCommand("p &" + variable.remove(0,1) );
			return;
		}

		if(variable.at(0) ==  '&')
		{
			setCommand("p " + variable );
			return;
		}
		
		setCommand("p &" + variable );
	}

	// change la valeur de la variable	
	void setVariableValue(QString variable, QString valeur)
		{setCommand("p " + variable + "=" + valeur );}


// registres
	void getAllRegistersValue()
		{setCommand("info register");};

	void getRegisterValue(QString regname)
		{ setCommand("p $" + regname ); };

	void setRegisterValue(QString regname, QString value)
		{setCommand("set $" + regname +"=" + value );};

// 	
	void getInfoLine()
		{setCommand("info line");};

	void getDebugerAt()
		{setCommand("bt");};

	void setCommand(QString command);	


private:

//	void setCommand(QString command);	
	int gdbAutoPrompt(QString st);
	QString GdbName;
	// queue command
	QList <QString> mCommandList;
	// Gdb Idl ?
	bool GdbIdl;
	bool gdbStarted;
	QString st;
	QFile *file_log_gdb;


private slots:
	void gdbFinishCommandStd();
	void gdbFinishCommandErr();
	void slotGdbCommandend();
	void OnError(QProcess::ProcessError er);
void gdbfinished ( int , QProcess::ExitStatus  );

signals:
	void signalGdbCommandEnd();
	void GdbInfo(QString);
};

