//===============================
// Autor : Xiantia
// modul Breakpoint
//===============================


// INFO : pas de bug trouv

/*

structure contenant tout les breakpoint et index GDB pour un fichier source

	struct sfile
	{
		QString File_name;
		QList <int> Bp_list;
		QList <int> Bp_index;
	};


	Exemple : le fichier main.cpp a deux breakpoint a la ligne 18 , 45 et 34

	File_name = main.cpp
	Bp_List = 18, 45, 34
	BP_index = 1, 2, 3

	Gdb ne donne pas de nom au breakpoint, seulement un index de creation
	il faut donc les memorisers pour pouvoir les supprimers par exemple.
*/


#include "src/debuger/UIdebuger.h"
//#include "src/debuger/debuger_Register.h"
#include <QCheckBox>

//============== BREAKPOINT =============

void UIdebuger::breakpointInit()
{
	// on ouvre un projet alors supprime de la vue touts les breakpoint 
/*	for(int i=0; twBreakpoint->rowCount(); i++)
		twBreakpoint->removeRow(0);
	
*/	// supprime donc ceux dans la veritable list
	BreakpointList.clear();
}


// UI
/*void UIdebuger::breakpointInitWidget()
{	

	QTableWidgetItem *itemColumnFichier = new QTableWidgetItem("column_fichier");
	QTableWidgetItem *itemColumnLigne = new QTableWidgetItem("column_ligne");

	itemColumnFichier->setText( tr("File"));
	itemColumnLigne->setText( tr("Line"));

	twBreakpoint->insertColumn(0);
	twBreakpoint->insertColumn(1);


	twBreakpoint->setHorizontalHeaderItem(0, itemColumnFichier);
	twBreakpoint->setHorizontalHeaderItem(1, itemColumnLigne);
}

void UIdebuger::breakpointAdd(QString fileName, QString line)
{
int i = twBreakpoint->rowCount();

	twBreakpoint->insertRow(i);
	twBreakpoint->setRowHeight(i,20);

	QTableWidgetItem *itemFile = new 	QTableWidgetItem;
	itemFile->setText(fileName);
	itemFile->setFlags(Qt::ItemIsEnabled );
	twBreakpoint->setItem(i,0, itemFile);

	QTableWidgetItem *itemLine = new 	QTableWidgetItem;
	itemLine->setText(line);
	itemLine->setFlags(Qt::ItemIsEnabled );//| Qt::ItemIsEditable | Qt::ItemIsSelectable);
	twBreakpoint->setItem(i,1, itemLine);
}


void UIdebuger::breakpointSup(QString fileName, QString line)
{
	for(int i=0; i< twBreakpoint->rowCount(); i++)
	{
		
		QTableWidgetItem *itemFile = twBreakpoint->item(i,0);
		QTableWidgetItem *itemLine = twBreakpoint->item(i,1);

		if(itemFile->text() == fileName && itemLine->text() == line)
		{
			twBreakpoint->removeRow(i);
		}
	}
}*/
// end UI


// Parcour la liste a la recherche du fichier source
// si le fichier exist retourn sont index dans la liste
// sinon -1
int UIdebuger::findFile(QString fileName)
{
	int numberfile = BreakpointList.count();

	if(numberfile )
	{	
		for(int i=0; i<numberfile; i++)
		{
			//modif QFileInfo for ubuntu
			if(QFileInfo(BreakpointList[i].File_name).fileName() == QFileInfo(fileName).fileName()) return i;
		}
	}
	return -1;
}


// le breakpoint est memoris par GDB
// il retourne sont index
void UIdebuger::setBreakpointIndex(QString st)
{
// Breakpoint 1 at 0x8052a27: file main.cpp, line 18.
// recupere le 1
//qDebug(st.toAscii());
	QList <QString> list = st.split(" ");

	QString file = list[5]; // non du fichier
	file.remove(",");	
//qDebug(file.toAscii());
	QString numberLine = list[7]; // ligne du fichier
	numberLine.remove(".");

	QString index = list[1]; // index du breakpoint suivant GDB
//qDebug(index.toAscii());

	int i = findFile(file);		// recherche le fichier dans la liste
	if(i !=-1)
	{
//		qDebug("ok index");

		int j = BreakpointList[i].Bp_list.indexOf( numberLine.toInt() );
		if(j != -1)
			BreakpointList[i].Bp_index <<  index.toInt() ;
	}
}


// initialise les breakpoint qui ont etaient fait
// avant le lancement de GDB
void UIdebuger::setBreakpointOnStart()
{
int numberfile = BreakpointList.count();

	for(int i =0; i<numberfile; i++)
	{
		int numberlist = BreakpointList[i].Bp_list.count();

		for(int j =0; j<numberlist ;j++)
			if(pGdbCommand) 
				pGdbCommand->setBreakpoint("b " + BreakpointList[i].File_name + ":" + QString::number(BreakpointList[i].Bp_list[j]));
	}		
}
 


// slot public
// CTRL + B venant de TextEditor
// OK
void UIdebuger::debugerToggleBreakpoint(QString fileName, int bp_line)
{

//	ondebugerMsgFrom(QString("toggleBreakpoint:"+ fileName +":"+ QString::number(bp_line)));
//return;

struct sfile mBreakpointList;

	mBreakpointList.File_name  = fileName;
	mBreakpointList.Bp_list << bp_line;
	
	int numberFile = BreakpointList.count();

	// liste deja initialise ?
	if(numberFile)// != -1)
	{	
		// oui
		// la liste contient deja le fichier ?
		int i = findFile(fileName);
//qDebug(fileName.toAscii());
		if(i != -1)
		{
//qDebug("fichier trouvé :)");
			// oui
			// si le breakpoint exit , alors supprime le
			if(  BreakpointList[i].Bp_list.contains(bp_line))
			{
				int j = BreakpointList[i].Bp_list.indexOf(bp_line);
				BreakpointList[i].Bp_list.removeAt(j);
//qDebug("supprime");
				// on verif que la liste n'est pas vide			
				// cas d'ajout puis suppression d'un bp avant le lancement de GDB
				// command "file /home/...."
				// bug fix				
				if(BreakpointList[i].Bp_index.count())
				{
					QString Breakpoint_at= QString("delete ") +QString::number(BreakpointList[i].Bp_index[j]) ;
					if(pGdbCommand) 
						pGdbCommand->setBreakpoint( Breakpoint_at);
					BreakpointList[i].Bp_index.removeAt(j);
				}	
//				breakpointSup( fileName ,QString::number(bp_line));
			}
			else
			{
//qDebug("ajoute");
				// sinon ajoute
				QString breakpoint_at = "b " + fileName + ":" + QString::number(bp_line) ;
				if(pGdbCommand) pGdbCommand->setBreakpoint(breakpoint_at);
				BreakpointList[i].Bp_list << bp_line;
//				breakpointAdd(fileName,QString::number(bp_line));

			}
		}			
		else
		{
			// la liste est initialis
			// mais ne contient pas ce fichier source
//qDebug("fichier non trouvé -> ajoute le ");
			BreakpointList << mBreakpointList; 
			QString breakpoint_at = QString("b ") + fileName + QString(":") + QString::number(bp_line);
			if(pGdbCommand) pGdbCommand->setBreakpoint(breakpoint_at);		
//			breakpointAdd(fileName,QString::number(bp_line));

		}
	}
	else
	// premier de la list
	{
//qDebug("List vierge");
		BreakpointList << mBreakpointList; 
		QString breakpoint_at = QString("b ") + fileName + QString(":") + QString::number(bp_line) ;
		if(pGdbCommand) pGdbCommand->setBreakpoint(breakpoint_at);		
//		breakpointAdd(fileName,QString::number(bp_line));
	}
}


// Retourn la list des breakpoints du fichier source
/*QList <int> UIdebuger::debugerGetBreakpoint(QString fileName)
{
	int index = findFile(fileName);

	if(index != -1)
		return BreakpointList[index].Bp_list;
}


QList <UIdebuger::sfile > UIdebuger::debugerGetAllBreakpoint()
{
	return BreakpointList;
}
*/