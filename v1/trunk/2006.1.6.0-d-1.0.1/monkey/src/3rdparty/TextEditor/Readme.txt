/********************************************************************************************************
 * PROGRAM      : TextEditor Widgets
 * DATE - TIME  : jeudi 04 mai 2006 - 22h20
 * AUTHOR       : Nox PasNox ( pasnox at hotmail dot com )
 * FILENAME     : Readme.txt
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/

This is TextEditor v0.6.0
=========================

Compil
======

qmake && make, run TextEditorDemo to see a demo of the widget

How To Use
==========

Copy *.h to an include path ( or add a reference to plugin\src in your project includes path )
Copy *.so to a lib dir ( or add a reference where the so is in your project libs )
Add an entry to your linker : -lTextEditorPlugin

Example
=======

if the dir TextEditor is at : /home/login/TextEditor
and if you don't want to copy files then your pro file must be like this

INCLUDEPATH += /home/login/TextEditor/plugin/src
LIBS += -lTextEditorPlugin -L/home/login/TextEditor


Report comments, ideas, bugs to pasnox@gmail.com


--EOF