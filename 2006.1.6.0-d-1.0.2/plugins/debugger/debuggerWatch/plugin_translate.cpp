/*

Info	:	Traduction de certaine class de QT 	
by	:	Xiantia
version	:	1.0
date	:	28/11/06
licence	:	owner
 	
*/

#include "debuggerWatch.h"

QStringList qslTranslateQStringSupport = QStringList() << "QString" << "const QString";
QStringList qslTranslateQListSupport  = QStringList() << "QList<int>" << " const QList<int>" << "QList<float>" << " const QList<float>" << "QList<unsigned>" << "const QList<unsigned>";
QStringList qslTranslateQStringListSupport = QStringList() << "QStringList" << "QList<QString>" << "const QStringList" << "const QList<QString>";


/*
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

*/

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

/*
	reinitialise la traduction
*/

void Watch::translateReInit()
{
		indexTranslate = 0;
//		tmpTanslate.clear();

		memberCount.clear();
	 	indexMemberCount=0;
		indexTranslateQStringList= 0;
//		tmpQStringList.clear();
}


/*
	main fonction
*/

QString Watch::translatVariable(struct UIdebugger_struct * debugger_struct)
{

	// la variable est du type QString ?
	if(qslTranslateQStringSupport.contains(qtwiCurrentVariable->text(1)))
			return translateQString(debugger_struct,  qtwiCurrentVariable->text(0));

	// la variable est du type QStringList ou QList<QString> ?
	if(qslTranslateQStringListSupport.contains(qtwiCurrentVariable->text(1)))
			return translateQStringList(debugger_struct,  qtwiCurrentVariable->text(0));

	// la variable est de type QList?	
	switch(qslTranslateQListSupport.indexOf(qtwiCurrentVariable->text(1)))
	{
		case 0 :
		case 1 : 	return translateQList(debugger_struct,"d", qtwiCurrentVariable->text(0));
		case 2 :
		case 3 : 	return translateQList(debugger_struct,"f",  qtwiCurrentVariable->text(0));
		case 4 :
		case 5 : 	return translateQList(debugger_struct,"u",  qtwiCurrentVariable->text(0));	
	}
	return "no deformated";//debugger_struct->qsDataFromGdb;
}


/* 
	initialise la structure de sortie
*/

void Watch::setStructure(int to, int userCmd, QString cmd)
{
		debugger_struct_out.iCurrentCommandType = to;
		debugger_struct_out.iPluginCommand = userCmd;
		debugger_struct_out.qsCurrentCommand.clear();
		debugger_struct_out.qsCurrentCommand = cmd;
}

/*
	formatage de la size
*/

QString formatSize(QString qsSize)
{
	qsSize= qsSize.right(qsSize.length() - qsSize.lastIndexOf("=") -2);
	qsSize.remove('\n');
	qsSize.remove('\r');
	return qsSize;
}


/* 
	formatage de l'adresse
*/

QString formatAdr(QString qsAdr)
{
	qsAdr = qsAdr.right( qsAdr.length() - qsAdr.lastIndexOf(")") -1);
	qsAdr.remove('\n');
	qsAdr.remove('\r');
	return qsAdr;
}

/*
	verifit la valeur de qsSize (negative, trop grande, ....)
*/

QString getBornes(QString qsSize)
{
#define str_max_length 100

	if(qsSize.toInt() > str_max_length) 
		qsSize = QString::number(str_max_length);
	if(qsSize.toInt() < 0) 
		qsSize = QString::number(0);

	return qsSize;

}
//=============================================

// deformatage d'un QStringList ou QList<QString>
/*

	nbrMember = p val.d.end			-> retourn le nombre de membre dans la list
	size =  p ((QString)(trf.d.array[0])).size()	-> retourn le nombre de char du QSrting
	adr =p ((QString)(val.d.array[0])).d.data		-> adresse de la chaine
	x /size s adr				-> retourne la chaine 
*/


QString Watch::translateQStringList(struct UIdebugger_struct * debugger_struct, QString variableName)
{
	switch(indexTranslateQStringList)
	{
		case 0 :
		{
			tmpQStringList.clear();
			tmpQStringList = "$ = { "; // mise en forme
			setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"p " + getPointeurType(variableName) + "d.end");
			indexTranslateQStringList ++;
			return "";
		}

		case 1 : // memberCount recept
		{
			if(memberCount.isEmpty())
			{
				memberCount = getBornes(formatSize( debugger_struct->qsDataFromGdb));
				if(memberCount.toInt() == 0 ) 
				{
					memberCount.clear();
					indexMemberCount=0;
					indexTranslateQStringList=0;
					return tmpQStringList +"__Vide__ }";
				}
			}

			if(memberCount.toInt())
			{
				//demande  la QString
				QString tmp = translateQString(debugger_struct, "((QString)(" + getPointeurType(variableName) + "d.array[" + QString::number(indexMemberCount) + "]))");
				if(tmp.isEmpty())
					return "";

				tmp.remove("$ = ");
		
				tmpQStringList.append(tmp + " , ");
				indexMemberCount++;

				if(indexMemberCount < memberCount.toInt())
					return "";
				// fin
				memberCount.clear();
				indexMemberCount=0;
				indexTranslateQStringList=0;
				tmpQStringList.replace(tmpQStringList.lastIndexOf(','),1,"}");
				return tmpQStringList;
			}
			else 
			{
				memberCount.clear();
				indexMemberCount=0;
				indexTranslateQStringList=0;
				return tmpQStringList + "__Vide__ }";
			}
		}
	} // end switch
	return  ""; // no warning
}


//=============================================

// deformatage d'un QList
/*

	size = p val.d.end	-> retourn le nombre d'octet
	adr = p &val.d.array	-> adresse de la chaine
	x /size s adr		-> retourne la chaine 
*/

QString Watch::translateQList(struct UIdebugger_struct * debugger_struct, QString Type, QString variableName)
{
	switch(indexTranslate)
	{
		case 0 :
		{
			tmpTanslate.clear();
			tmpTanslate ="$ = { "; // mise en forme
			setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"p " + getPointeurType(variableName) + "d.end");
			indexTranslate ++;
			return "";
		}

		case 1 : // size recept
		{
			qsSize = getBornes(formatSize( debugger_struct->qsDataFromGdb));
			if(qsSize.toInt() == 0 ) 
			{
				indexTranslate =0;
				return tmpTanslate + "__Vide__ }";
			}
			setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"p &" + getPointeurType(variableName) +"d.array");
			indexTranslate++;
			return "";
		}
				
		case 2: // adr recept
		{
			qsAdr =   formatAdr(debugger_struct->qsDataFromGdb);
			setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"x /"+  qsSize + Type +  " " + qsAdr);
			indexTranslate++;
			return "";
		}

		// size et adresse ok
		case 3 : 
		{
			QStringList list = debugger_struct->qsDataFromGdb.split("\t");

			for(int i=1; i<list.count(); i++)
				tmpTanslate.append(list.at(i) + " , ");
			// fin
			indexTranslate=0;
			tmpTanslate.replace(tmpTanslate.lastIndexOf(','),1,"}");
			tmpTanslate.remove("\n");
			return tmpTanslate.remove("\r") ;
		}	
	}// end switch
	return  ""; // no warning
}


//=============================================

// deformatage d'un QString
/*

	size = p val.d.size	-> retourn le nombre d'octet
	adr = p val.d.data	-> adresse de la chaine
	x /size s adr		-> retourne la chaine 
*/

QString Watch::translateQString(struct UIdebugger_struct * debugger_struct, QString variableName)
{
	switch(indexTranslate)
	{
		case 0 :
		{
			tmpTanslate.clear();
			tmpTanslate ="$ = \""; // mise en forme
			setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"p  " + getPointeurType(variableName) + "d.size");
			indexTranslate ++;
			return "";
		}

		case 1 : // size recept
		{
			qsSize = getBornes(formatSize( debugger_struct->qsDataFromGdb));
			if(qsSize.toInt() == 0 ) 
			{
				indexTranslate =0;
				return tmpTanslate + "__Vide__\"";
			}
			setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"p " + getPointeurType(variableName) + "d.data");
			indexTranslate++;
			return "";
		}

		case 2: // adr recept 
		{
			qsAdr =   formatAdr(debugger_struct->qsDataFromGdb);
			setStructure(PLUGIN_TO_GDB, GET_TRANSLATE,"x /"+ qsSize + "s " + qsAdr);
			indexTranslate++;
			return "";
		}

		// size et adresse ok
		case 3 : 
		{
			QStringList list = debugger_struct->qsDataFromGdb.split("\"");
	
			for(int i=0; i<list.count()-2; i+=2)
				tmpTanslate.append(list.at(i+1));
			// fin
			indexTranslate=0;
			return tmpTanslate + "\"";
		}	
	}// end switch
	return ""; //no warning
}

