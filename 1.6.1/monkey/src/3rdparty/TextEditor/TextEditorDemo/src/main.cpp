/********************************************************************************************************
 * PROGRAM      : QTextEditorApp
 * DATE - TIME  : samedi 18 mars 2006 - 15h08
 * AUTHOR       : Nox PasNox ( pasnox at hotmail dot com )
 * FILENAME     : main.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include <QApplication>
//
#include "UITextEditorDemo.h"
//
int main(int argc, char ** argv)
{
	// just a small app for testing the component
	QApplication app( argc, argv );
	UITextEditorDemo::self()->show();
	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
	return app.exec();
}
