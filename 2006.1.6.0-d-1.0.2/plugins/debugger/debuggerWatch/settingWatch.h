#ifndef SETTINGWATCH_H
#define SETTINGWATCH_H
//
#include "ui_settingWatch.h"
//
#include <QPointer>
//
class settingWatch : public QDialog, public Ui::settingWatch
{
	Q_OBJECT
	//
public:
	static settingWatch* self( QWidget* = 0 );
	//
protected:
	void closeEvent( QCloseEvent* );
	//
private:
	settingWatch( QWidget* = 0 );
	static QPointer<settingWatch> _self;
	//
};
//
#endif // SETTINGWATCH_H
