#include "debuggerWatch.h"

/* change
	plus de carret a la fin des ligne pour windows
	decompilation correct des structures
*/

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

// sur quelque structure , gdb envoit des double '{' a la suite.
// pour la decompilation il faut les supprimers.
QString Watch::checkBlock(QString val)
{
	if(val.contains(" = {")) 
	{
		int index = val.indexOf("{{");
		if(index != -1)
		{
			// remove first
			val.remove(index,1);
			// il faut aussi supprimer la fermante

			int count = -1;
			do
			{
				if(val.at(index) == '}')
					count--;
				if(val.at(index) == '{')
					count==-1 ? count=1 : count++;
				index++;
			}
			while(count !=0);
			val.remove(index-1,1);
			return val;
		}
		return val;
	}
	return val;
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
//QString nameBlock;
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
		}
	}
}


void Watch::decompilStrutUpdate(QTreeWidgetItem *parentItem, QString val)
{
//QString nameBlock;
QString memberBlock;
int index=0;
//int indexTab=0;
//	val = checkBlock(val);

	QStringList memberList = extractMember(val);

//	nameBlock = val.left(val.indexOf(" = {"));

	for(int i=0; i<memberList.count();i++)
	{
		memberBlock = memberList.at(i);
		while(memberBlock.startsWith(" ")) memberBlock.remove(0,1);
		QTreeWidgetItem *child;	
		child = parentItem->child(index);
		if(child == NULL ) return;
		switch(memberType(memberBlock))
		{
			case 0:
		//		child->setText(0, "[0x" + QString::number(indexTab++,16) +"]");
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
		}
		index++;
	}
}
