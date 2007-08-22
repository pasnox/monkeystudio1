#include "debuggerWatch.h"

QStringList qslTranslateQStringSupport = QStringList() << "QString" << "const QString";
QStringList qslTranslateQListSupport  = QStringList() << "QList<int>" << "QList<float>" << "QList<unsigned>";


extern void printQstring(QString str);

QString Watch::getPointeurType(QString val)
{
	if(val.at(0) == '*')
	{
		val.remove("*");
		return  val + "->";
	}
	else
		return val + ".";
}



void Watch::translateReInit()
{
		indexTranslate = 0;
		tmpTanslate.clear();
}


// main function


QString Watch::translatVariable(struct UIdebugger_struct * debugger_struct)
{

	// la variable est du type QString ?
	if(qslTranslateQStringSupport.contains(qtwiCurrentVariable->text(1)))
			return translateQString(debugger_struct);

	// la variable est de type QList?	
	switch(qslTranslateQListSupport.indexOf(qtwiCurrentVariable->text(1)))
	{
		case 0 : 	return translateQList(debugger_struct,"d");
		case 1 : 	return translateQList(debugger_struct,"f");
		case 2 :	return translateQList(debugger_struct,"u");	
	}
	

	return "no deformated";//debugger_struct->qsDataFromGdb;
}


void Watch::setStructure(int to, int userCmd, QString cmd)
{
		debugger_struct_out.iCurrentCommandType = to;
		debugger_struct_out.iPluginCommand = userCmd;
		debugger_struct_out.qsCurrentCommand.clear();
		debugger_struct_out.qsCurrentCommand = cmd;
}


QString Watch::translateQList(struct UIdebugger_struct * debugger_struct, QString Type)
{

#define str_max_length 100
QString variableName = qtwiCurrentVariable->text(0) ;

		switch(indexTranslate)// == 0) 
		{
			case 0 :
			{
				tmpTanslate.clear();
				tmpTanslate ="$ = { "; // mise en forme

				setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"p " + getPointeurType(variableName) + "d.alloc");
				indexTranslate ++;
				return "";
			}

			case 1 ://if(indexTranslate == 1) // size recept
			{
				qsSize = debugger_struct->qsDataFromGdb;
				qsSize= qsSize.right(qsSize.length() - qsSize.lastIndexOf("=") -2);
				qsSize.remove('\n');
				qsSize.remove('\r');

				if(qsSize.toInt() == 0 ) 
				{
					indexTranslate =0;
					return tmpTanslate + "}";
				}

				if(qsSize.toInt() > str_max_length) qsSize = QString::number(str_max_length);

				//demande adresse
				setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"p &" + getPointeurType(variableName) +"d.array");
				indexTranslate++;
				return "";
			}

			// size et adresse ok
			case 2://if(indexTranslate == 2)
			{
				qsAdr =   debugger_struct->qsDataFromGdb;
				qsAdr = qsAdr.right( qsAdr.length() - qsAdr.lastIndexOf(")") -1);
				qsAdr.remove('\n');
				qsAdr.remove('\r');

				//demande valeur
				setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"x /"+  qsSize + Type +  " " + qsAdr);
				indexTranslate++;
				return "";
			}

			case 3 : //indexTranslate == 3)
			{
				QStringList list = debugger_struct->qsDataFromGdb.split("\t");
	
				for(int i=1; i<list.count(); i++)
					tmpTanslate.append(list.at(i) + " , ");

				indexTranslate=0;
				tmpTanslate.append("}");	
				tmpTanslate.remove("\n");
				return tmpTanslate.remove("\r") ;
			}	
	}// end switch

	return  ""; // no warning
}



// deformatage d'un QString

QString Watch::translateQString(struct UIdebugger_struct * debugger_struct)
{

QString variableName = qtwiCurrentVariable->text(0) ;

#define str_max_length 100

		// demande la taille, variable.d->size
		switch(indexTranslate)// == 0) 
		{
			case 0 :
			{
				tmpTanslate.clear();
				tmpTanslate ="$ = \""; // mise en forme

				setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"p  " + getPointeurType(variableName) + "d.size");
				indexTranslate ++;
				return "";
			}

			case 1 ://if(indexTranslate == 1) // size recept
			{
				qsSize = debugger_struct->qsDataFromGdb;
				qsSize= qsSize.right(qsSize.length() - qsSize.indexOf("=") -2);
				qsSize.remove('\n');
				qsSize.remove('\r');

				if(qsSize.toInt() > str_max_length) qsSize = QString::number(str_max_length);

				setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"p " + getPointeurType(variableName) + "d.data");
				indexTranslate++;
				return "";
			}

			// size et adresse ok
			case 2://if(indexTranslate == 2)
			{
				qsAdr =   debugger_struct->qsDataFromGdb;
				qsAdr = qsAdr.right( qsAdr.length() - qsAdr.indexOf(")") -1);
				qsAdr.remove('\n');
				qsAdr.remove('\r');

				//demande valeur
				setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"x /"+ qsSize + "s " + qsAdr);
				indexTranslate++;
				return "";
			}

			case 3 : //indexTranslate == 3)
			{
				QStringList list = debugger_struct->qsDataFromGdb.split("\"");
	
				for(int i=0; i<list.count()-2; i+=2)
					tmpTanslate.append(list.at(i+1));

				indexTranslate=0;
				return tmpTanslate + "\"";
			}	
	}// end switch
	return ""; //no warning
}

