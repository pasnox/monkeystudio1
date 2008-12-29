/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : vendredi 27 janvier 2006 - 00h23
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : Tools.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef TOOLS_H
#define TOOLS_H
//
#include <QUrl>
//
class QObject;
class QListWidget;
//
namespace Tools
{
	QString makeAbsolute( const QString&, const QString& = QString() );
	QString makeRelative( const QString&, const QString& = QString() );
	void setLayout( QObject*, int = 0, int = 0 );
	void alignItems( QListWidget*, Qt::Alignment, Qt::ItemFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	QString quoted( const QString& );
	bool isKDERunning();
	bool isGnomeRunning();
	void openUrl( const QUrl& );
	void shellExecute( const QString& );
	//
};
//
#endif // TOOLS_H
