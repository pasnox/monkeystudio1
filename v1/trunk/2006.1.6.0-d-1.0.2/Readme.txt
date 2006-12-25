/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : mardi 31 janvier 2006 - 15h09
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : Readme.txt
 * LICENSE      : GPL
 * COMMENTARY   : This file contains a very small documentation on what is qt4ds, and how to compile it.
 ********************************************************************************************************/

THIS PROJECT USE PART OF CODE FROM OTHER GPLed PROJECT (QSASyntaxHighlighter, etc...),
AS THEY HAVE BEEN MODIFIED TO SUITE MY NEEDS THEY MUST BE CONSIDERED AS SEPARATED WORK
ORIGINAL SOURCE CODE ARE COPYRIGHTS BY THE AUTHORS

This file can be obsolete.

Qt4DS : See current version in Changes.txt
=======

Qt4DS is a short for Qt 4 Developer Studio.
Qt4DS is an advanced cross-platform Qt 4.x IDE.
It's goal is to permit to users to manage their projects in the same IDE on all platform that Qt 4 supports.
Why ?!
Because at my knowledge, there is no cross-platform Qt 4 IDE.
Each platfrom use its well know IDEs ( unix: emacs, KDevelop..., Windows: Visual Studio, Dev-Cpp, ... )
When that come the time to port your application on other OS, it's impossible to continue working the same way!
Because, a new IDE need new configuration, specific configuration, many and many times lost.
With Qt4DS you only need to configure the correct path to Qt 4 installation ( or copy your ini file ).
Qt4DS doesn't use system registry, but ini files allowing users to copy/use already existing settings file.
Accepting Qt4DS is the best choice you can do if you want to work the same way on all platforms.

Since version 0.8.0, Qt4DS is developed with .... Qt4DS :)

PS: You are encouraged to post a link to my website. See the about window for correct url.

How to compil
===========

For compil qt4ds, you only need a correct Qt 4.x installation.
Open a terminal, go to qt4ds directory and write : qmake4 && make.
ATTENTION: QMAKE4
This file doesn't exists. It exists for me because KDE is built on top of Qt 3.x ( that already contains uncomptatible binary : qmake, designer etc... )
So I have create link in /usr/local/bin that point to Qt 4.x binary ( qmake4 -> /usr/local/lib/qt4.1.0/bin/qmake, qtdesigner4 -> /usr/local/lib/qt4.1.0/bin/designer etc... )
If you don't want to create links, then your command line will be : qt4dir/bin/qmake && make

This will compil the binary in the binary directory named like this : qt4ds_xx ( xx represent the wm: win32, x11, mac )

Translations
==========

Qt4DS is in UK language by default, but comes with french translation and maybe more.
Simply configure your translations path to point on where are the qm files and restart it to get your local translation ( if translation exists, else you will continue with default UK language )

What I Need ? - Futur ?!
==================
See ToDo.txt ( bottom )

					Nox P@sNox, ( pasnox@hotmail.com )
--EOF