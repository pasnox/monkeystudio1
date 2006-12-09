/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : jeudi 05 janvier 2006 - 19h19
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : main.h
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#ifndef MAIN_H
#define MAIN_H
//
#include <QApplication>
#include <QMessageBox>
//
#define _PROGRAM_NAME "Monkey Studio"
#define _PROGRAM_SHORT_NAME "Monkey"
#define _PROGRAM_TITLE _PROGRAM_NAME
#define _PROGRAM_VERSION "2006.1.6.0"
#define _PROGRAM_WEBSITE "http://sourceforge.net/projects/monkeystudio"
#define _ORGANIZATION "Monkey Studio Team"
#define _COPYRIGHTS "(c) 2005, 2006"
//
#define _CRITICAL QObject::tr( "Critical..." )
#define _INFORMATION QObject::tr( "Information..." )
#define _QUESTION QObject::tr( "Question..." )
#define _WARNING QObject::tr( "Warning..." )
//
#define _APP_DIR_PATH qPrintable( QApplication::applicationDirPath() )
#define _APP_FILE_PATH qPrintable( QApplication::applicationFilePath() )
//
#endif // MAIN_H
