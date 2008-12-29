#ifndef UIDEBUGERARGS_H
#define UIDEBUGERARGS_H
//
#include "ui_UIdebugerArgs.h"
//
#include <QPointer>
//
class UIdebugerArgs : public QDialog, public Ui::UIdebugerArgs
{
	Q_OBJECT
	//
public:
	static UIdebugerArgs* self( QWidget* = 0 );
	//
	QString GetArgs();
	
protected:
	void closeEvent( QCloseEvent* );
	//
private:
	UIdebugerArgs( QWidget* = 0 );
	static QPointer<UIdebugerArgs> _self;
	//
	QString Args;
private slots:
	void OnOk();
	
};
//
#endif // UIDEBUGERARGS_H
