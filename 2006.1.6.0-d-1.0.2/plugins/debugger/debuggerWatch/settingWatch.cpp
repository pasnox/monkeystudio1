#include "settingWatch.h"
//
#include <QCloseEvent>
//
QPointer<settingWatch> settingWatch::_self = 0L;
//
settingWatch* settingWatch::self( QWidget* parent )
{
	if ( !_self )
		_self = new settingWatch( parent );
	return _self;
}
//
settingWatch::settingWatch( QWidget* parent )
	: QDialog( parent )
{
	setupUi( this );
	connect(bOk,SIGNAL(clicked()),SLOT(onClose()));

}
//
void settingWatch::closeEvent( QCloseEvent* e )
{
	e->accept();
}

void settingWatch::onClose()
{
	close();	
}
