/********************************************************************************************************
 * PROGRAM      : TextEditorPlugin
 * DATE - TIME  : lundi 01 mai 2006 - 19h55
 * AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
 * FILENAME     : TextEditorPlugin.cpp
 * LICENSE      : GPL
 * COMMENTARY   : This class create a Qt Designer plugin to add TextEditor available.
 ********************************************************************************************************/
#include "TextEditorPlugin.h"
#include "TextEditor.h"
//
TextEditorPlugin::TextEditorPlugin( QObject* parent )
	: QObject( parent ), initialized( false )
{
}
//
void TextEditorPlugin::initialize( QDesignerFormEditorInterface* core )
{
	Q_UNUSED( core );
	if ( !initialized )
		initialized = true;
}
//
bool TextEditorPlugin::isInitialized() const
{
	return initialized;
}
//
QWidget* TextEditorPlugin::createWidget( QWidget* parent )
{
	return new TextEditor( parent );
}

QString TextEditorPlugin::name() const
{
	return QLatin1String( "TextEditor" );
}

QString TextEditorPlugin::group() const
{
	return QLatin1String( "Nox" );
}

QIcon TextEditorPlugin::icon() const
{
	return QIcon();
}

QString TextEditorPlugin::toolTip() const
{
	return QString();
}

QString TextEditorPlugin::whatsThis() const
{
	return QString();
}

bool TextEditorPlugin::isContainer() const
{
	return false;
}

QString TextEditorPlugin::domXml() const
{
	return QLatin1String("<widget class=\"TextEditor\" name=\"TextEditor1\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>180</width>\n"
		"   <height>80</height>\n"
		"  </rect>\n"
		" </property>\n"
		"</widget>\n");
}
//
QString TextEditorPlugin::includeFile() const
{
	return QLatin1String( "TextEditor.h" );
}
//
QString TextEditorPlugin::codeTemplate() const
{
	return QString();
}
//
Q_EXPORT_PLUGIN2( TextEditor, TextEditorPlugin )
