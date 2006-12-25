/*

Info	:	decompilation des structures 	
by	:	Xiantia
version	:	1.0
date	:	28/11/06
licence	:	owner
 	
*/


#include "debuggerWatch.h"


// extrait les membres de la  structure
// exemple : "rtf = {art = {0}, a = 5 , b = 6 , rtf  = {t = 12 , r = 34}}"
// return  :
// 1 : art = {0}
// 2 : a = 5
// 3 : b = 6
// 4 : rtf = {t = 12 , r = 34}

QStringList Watch::extractMember(QString val)
{
int count =-1;
QString member;
QStringList memberList;

	for(int i = val.indexOf("{")+1; i<val.length(); i++)
	{
		if(val.at(i) == '{')
		{
			do
			{
				if(val.at(i) == '}')
					count--;
				if(val.at(i) == '{')
					count==-1 ? count=1 : count++;
		
				 member.append(val.at(i));
				
				i++;
			}
			while(count !=0);
		
			memberList << member;
			member.clear();
		}
		else
		{
			if(val.at(i) == ',' || val.at(i) == '}')
			{
				if(!member.isEmpty())
				{
					memberList << member;
				 }
				member.clear();
			}
			else
				member.append(val.at(i));
		}
	}
	return memberList;
}


// return le type du membre de la structure
// 2 : c'est un autre structure
// 1 : membre " x= y"
// 0 : membre orphelin "0x00"
int Watch::memberType(QString val)
{
QString name;
QStringList arg;

	arg = val.split("{");
	if(arg.at(0).isEmpty()) return 0;
	
	if(val.contains(" = {"))
		 return 2;

	if(val.contains("="))
		return 1;
	return 0; // member orphelin
}


// formatage des membres
QStringList Watch::formatMember(QString val)
{
	if(val.contains("="))
	{
		QStringList list = val.split(" = ");
		return QStringList()<<list.at(0) <<list.at(1);
	}	
	if(val.contains(" = {"))
	{
		return QStringList()<<val <<"";
	}	
	return QStringList()<<val << val;
}


// recursive function
void Watch::decompilStrut(QTreeWidgetItem *parentItem, QString val)
{
QString memberBlock;
int indexTab=0;

	QStringList memberList = extractMember(val);
	
	for(int i=0; i<memberList.count();i++)
	{
		memberBlock = memberList.at(i);
		while(memberBlock.startsWith(" ")) memberBlock.remove(0,1);
		QTreeWidgetItem *child;	
		child = new QTreeWidgetItem(parentItem);
		switch(memberType(memberBlock))
		{
			case 0:
				child->setText(0, "[0x" + QString::number(indexTab++,16) +"]");
				child->setText(3, "$ = " + memberBlock);
				decompilStrut(child, memberBlock);
			break;

			case 1 :
				child->setText(0, formatMember(memberBlock).at(0));
				child->setText(3, "$ = " + formatMember(memberBlock).at(1));
			break;
			case 2:
				child->setText(0, formatMember(memberBlock).at(0));
				child->setText(3, "$ = " + memberList.at(i).right(memberList.at(i).length() - memberList.at(i).indexOf(" = {") - 3));
				decompilStrut(child, memberBlock);
		} // end switch
	}// end for
}


void Watch::decompilStrutUpdate(QTreeWidgetItem *parentItem, QString val)
{
QString memberBlock;
int index=0;

	QStringList memberList = extractMember(val);

	for(int i=0; i<memberList.count();i++)
	{
		memberBlock = memberList.at(i);
		while(memberBlock.startsWith(" ")) memberBlock.remove(0,1);
		QTreeWidgetItem *child;	
		child = parentItem->child(index);
		// quelques fois Gdb ajout un nouveau membre et comme il n'est 
		// pas initialis� on plante d'iou le test suivant
		if(child == NULL ) return;
		
		switch(memberType(memberBlock))
		{
			case 0:
				child->setText(3, "$ = " + memberBlock);
				decompilStrutUpdate(child, memberBlock);
			break;
			case 1 :
				isVariableChangedValue(child,"$ = " + formatMember(memberBlock).at(1));
				child->setText(3, "$ = " + formatMember(memberBlock).at(1));
			break;
			case 2:
				isVariableChangedValue(child,"$ = " + memberList.at(i).right(memberList.at(i).length() - memberList.at(i).indexOf(" = {") - 3));
				child->setText(3, "$ = " + memberList.at(i).right(memberList.at(i).length() - memberList.at(i).indexOf(" = {") - 3));
				decompilStrutUpdate(child, memberBlock);
		} // end switch
		index++;
	}// end for
}
