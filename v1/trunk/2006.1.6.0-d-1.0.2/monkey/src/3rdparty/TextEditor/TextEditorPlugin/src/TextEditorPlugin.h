/********************************************************************************************************
 * PROGRAM      : TextEditorPlugin
 * DATE - TIME  : lundi 01 mai 2006 - 19h55
 * AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
 * FILENAME     : TextEditorPlugin.h
 * LICENSE      : GPL
 * COMMENTARY   : This class create a Qt Designer plugin to add TextEditor available.
 ********************************************************************************************************/
#ifndef TEXTEDITORPLUGIN_H
#define TEXTEDITORPLUGIN_H
//
#include <QDesignerContainerExtension>
#include <QDesignerCustomWidgetInterface>
//
#include <qplugin.h>
#include <QIcon>
//
class QDesignerFormEditorInterface;
//
class TextEditorPlugin : public QObject, public QDesignerCustomWidgetInterface
{
	Q_OBJECT
	Q_INTERFACES( QDesignerCustomWidgetInterface )
public:
	TextEditorPlugin( QObject* = 0 );
	bool isContainer() const;
	bool isInitialized() const;
	QIcon icon() const;
	QString codeTemplate() const;
	QString domXml() const;
	QString group() const;
	QString includeFile() const;
	QString name() const;
	QString toolTip() const;
	QString whatsThis() const;
	QWidget* createWidget( QWidget* );
	void initialize( QDesignerFormEditorInterface* );
	//
private:
	bool initialized;
	//
};
//
#endif // TEXTEDITORPLUGIN_H
