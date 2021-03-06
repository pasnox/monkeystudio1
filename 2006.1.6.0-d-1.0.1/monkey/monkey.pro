warning( "This file has been generated by Monkey Studio (2006.1.6.0) - http://sourceforge.net/projects/monkeystudio" )

win32:OBJECTS_DIR	+= ../build/.o/win32
unix:OBJECTS_DIR	+= ../build/.o/unix
mac:OBJECTS_DIR	+= ../build/.o/mac
RCC_DIR	= ../build/.rcc/ide
UI_DIR	= ../build/.ui
MOC_DIR	= ../build/.moc

mac:ICON	+= monkey.icns

AUTHOR	= Nox PasNox
TEMPLATE	= app
EMAIL	= pasnox at gmail dot com
QT	= gui core
LANGUAGE	= C++
INCLUDEPATH	= . src src/ui src/3rdparty/TextEditor/src/ src/3rdparty/TextEditor/src/ui/ src/3rdparty/autocompletion/ src/3rdparty/autocompletion/QIComplete/
CONFIG	+= qt warn_on release app_bundle thread x11 windows assistant designer
DESTDIR	= ../binary
win32:TARGET	= monkey_win32
unix:TARGET	= monkey_x11
mac:TARGET	= monkey_mac
win32:RC_FILE	+= monkey.rc
LICENSE	= GPL
RESOURCES	= src/resources/resources.qrc

TEXTS	= ../Informations.txt \
	../Bugs.txt \
	../Readme.txt \
	../ToDo.txt \
	../Changes.txt \
	../WishList.txt \
	src/debugger.txt

SOURCES	= src/3rdparty/TextEditor/src/Completion.cpp \
	src/3rdparty/TextEditor/src/CppSyntaxHighlighter.cpp \
	src/3rdparty/TextEditor/src/Gluter.cpp \
	src/3rdparty/TextEditor/src/TextEditor.cpp \
	src/3rdparty/TextEditor/src/ui/ReplaceDialog.cpp \
	src/ui/UIMain.cpp \
	src/ui/UIOptions.cpp \
	src/ui/UINewFiles.cpp \
	src/ui/UISubclassWizard.cpp \
	src/ui/UIProjectSettings.cpp \
	src/ui/UIHeaderInformations.cpp \
	src/ui/UIToolsEdit.cpp \
	src/ui/UIAbout.cpp \
	src/UIAddExistingFiles.cpp \
	src/MDIChild.cpp \
	src/MDIClass.cpp \
	src/MDIText.cpp \
	src/Options.cpp \
	src/Tools.cpp \
	src/main.cpp \
	src/QMakeProject.cpp \
	src/3rdparty/autocompletion/InitCompletion.cpp \
	src/3rdparty/autocompletion/QIComplete/parse.cpp \
	src/3rdparty/autocompletion/QIComplete/readtags.cpp \
	src/3rdparty/autocompletion/QIComplete/tree.cpp \
	src/debuger/debuger_Breakpoint.cpp \
	src/debuger/debuger_MenuContext.cpp \
	src/debuger/gdbDriver.cpp \
	src/debuger/UIdebugerArgs.cpp \
	src/debuger/UIdebuger.cpp

FORMS	= src/3rdparty/TextEditor/src/ui/ReplaceDialog.ui \
	src/ui/UIMain.ui \
	src/ui/UIOptions.ui \
	src/ui/UINewFiles.ui \
	src/ui/UISubclassWizard.ui \
	src/ui/UIProjectSettings.ui \
	src/ui/UIHeaderInformations.ui \
	src/ui/UIToolsEdit.ui \
	src/ui/UIAbout.ui \
	src/debuger/UIdebugerArgs.ui \
	src/debuger/UIdebuger.ui

OPENEDFILES	= src/debuger/debuger_MenuContext.cpp \
	src/debuger/debuger_Breakpoint.h \
	src/debuger/debuger_Breakpoint.cpp \
	src/debuger/gdbDriver.h \
	src/debuger/gdbDriver.cpp \
	src/debuger/UIdebuger.h \
	src/debuger/UIdebuger.cpp \
	src/ui/UIMain.h \
	src/ui/UIMain.cpp

HEADERS	= src/3rdparty/TextEditor/src/BlockData.h \
	src/3rdparty/TextEditor/src/Completion.h \
	src/3rdparty/TextEditor/src/CppSyntaxHighlighter.h \
	src/3rdparty/TextEditor/src/Gluter.h \
	src/3rdparty/TextEditor/src/TextEditor.h \
	src/3rdparty/TextEditor/src/ui/ReplaceDialog.h \
	src/ui/UIMain.h \
	src/ui/UIOptions.h \
	src/ui/UINewFiles.h \
	src/ui/UISubclassWizard.h \
	src/ui/UIProjectSettings.h \
	src/ui/UIHeaderInformations.h \
	src/ui/UIToolsEdit.h \
	src/ui/UIAbout.h \
	src/UIAddExistingFiles.h \
	src/MDIChild.h \
	src/MDIClass.h \
	src/MDIText.h \
	src/Options.h \
	src/Tools.h \
	src/main.h \
	src/QMakeProject.h \
	src/3rdparty/autocompletion/InitCompletion.h \
	src/3rdparty/autocompletion/QIComplete/parse.h \
	src/3rdparty/autocompletion/QIComplete/readtags.h \
	src/3rdparty/autocompletion/QIComplete/tree.h \
	src/debuger/debuger_Breakpoint.h \
	src/debuger/gdbDriver.h \
	src/debuger/UIdebugerArgs.h \
	src/debuger/UIdebuger.h \
	src/debuger/interface_plugin.h

TRANSLATIONS	= translations/qt4ds_french.ts \
	translations/qt4ds_german.ts \
	translations/qt4ds_italian.ts \
	translations/qt4ds_spanish.ts \
	translation/qt4ds_russian.ts

