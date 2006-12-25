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
	//connect(radioDecimal,SIGNAL(clicked(bool)),SLOT(onDecimal(bool)));
	//connect(radioHexAndDecimal,SIGNAL(clicked(bool)),SLOT(onHexAndDecimal(bool)));
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
