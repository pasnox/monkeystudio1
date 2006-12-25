
#include "interface_plugin.h"


QString UIdebugger_Plugin::qsGetErGdb(QString st)
{
	for(int i =0; i< qslGetEr_Gdb.count(); i++)
		if(st.contains(qslGetEr_Gdb.at(i))) return st.remove('\n');
	return ""; 
}
