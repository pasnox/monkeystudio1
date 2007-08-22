#ifndef SETTINGREGISTER_H
#define SETTINGREGISTER_H
//
#include "ui_settingRegister.h"
//
#include <QPointer>
//
class settingRegister : public QDialog, public Ui::settingRegister
{
	Q_OBJECT
	//
public:
	static settingRegister* self( QWidget* = 0 );
	//
protected:
	void closeEvent( QCloseEvent* );
	//
private:
	settingRegister( QWidget* = 0 );
	static QPointer<settingRegister> _self;
	//
public slots:
	void onClose();
};
//
#endif // SETTINGREGISTER_H
