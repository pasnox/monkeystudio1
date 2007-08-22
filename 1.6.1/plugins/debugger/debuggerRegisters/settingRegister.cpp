#include "settingRegister.h"
//
#include <QCloseEvent>
//
QPointer<settingRegister> settingRegister::_self = 0L;
//
settingRegister* settingRegister::self( QWidget* parent )
{
	if ( !_self )
		_self = new settingRegister( parent );
	return _self;
}
//
settingRegister::settingRegister( QWidget* parent )
	: QDialog( parent )
{
	setupUi( this );
	connect(bOk,SIGNAL(clicked()),SLOT(onClose()));
}
//
void settingRegister::closeEvent( QCloseEvent* e )
{
	e->accept();
}


void settingRegister::onClose()
{
	close();	
}
