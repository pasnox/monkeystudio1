
#include "interface_plugin.h"

extern void printQstring(QString str);
/*
QString UIdebugger_Plugin::qsGetErType(QString st)
{
	for(int i =0; i< qslGetEr_Type.count(); i++)
	{
		if(st.contains(qslGetEr_Type.at(i))) 
			 return st.remove('\n');
	}
	return ""; 
}



QString UIdebugger_Plugin::qsGetErPrint(QString st)
{
	for(int i =0; i< qslGetEr_Print.count(); i++)
		if(st.contains(qslGetEr_Print.at(i))) return st.remove('\n');
	return ""; 
}


QString UIdebugger_Plugin::qsGetErAdresse(QString st)
{
	for(int i =0; i< qslGetEr_Adresse.count(); i++)
		if(st.contains(qslGetEr_Adresse.at(i))) return st.remove('\n');
	return ""; 
}

*/
QString UIdebugger_Plugin::qsGetErGdb(QString st)
{
	for(int i =0; i< qslGetEr_Gdb.count(); i++)
		if(st.contains(qslGetEr_Gdb.at(i))) return st.remove('\n');
	return ""; 
}
