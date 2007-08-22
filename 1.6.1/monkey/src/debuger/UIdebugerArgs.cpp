#include "UIdebugerArgs.h"
//
#include <QCloseEvent>
//
QPointer<UIdebugerArgs> UIdebugerArgs::_self = 0L;
//
UIdebugerArgs* UIdebugerArgs::self( QWidget* parent )
{
	if ( !_self )
		_self = new UIdebugerArgs( parent );
	return _self;
}
//
UIdebugerArgs::UIdebugerArgs( QWidget* parent )
	: QDialog( parent )
{
	setupUi( this );

	connect (okButton,SIGNAL(clicked()),SLOT(OnOk()));
}
//

QString UIdebugerArgs::GetArgs()
{
	return Args;
}

void UIdebugerArgs::OnOk()
{
	Args = lineEdit->text();
}
void UIdebugerArgs::closeEvent( QCloseEvent* e )
{
	e->accept();
}
