/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : jeudi 05 janvier 2006 - 20h04
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : QMakeProject.h
 * LICENSE      : GPL
 * COMMENTARY   : This class read/write QT Project (QMake Project) file : .pro
 ********************************************************************************************************/
#ifndef QMAKEPROJECT_H
#define QMAKEPROJECT_H
//
#include <QObject>
#include <QHash>
#include <QStringList>
//
enum pFileType
{
	// Top Level Entry In Projects Tree
	pFormItem = 0,
	pHeaderItem,
	pSourceItem,
	pTranslationItem,
	pTextItem,
	// File Type
	pProject,
	pForm,
	pHeader,
	pSource,
	pTranslation,
	pText,
	pResource,
	pRc,
	pRes,
	pDef,
	// Divers
	pOther,
	pSubDir,
	pOpenedFile,
	//
	pNotExists,
	pInvalidate,
	pUnknow
};
//
class QTreeWidget;
class QTreeWidgetItem;
//
typedef QHash<QString, QString> StringValues; // platform, value
typedef QHash<QString, QStringList> StringListValues; // platform, value
typedef QHash<QString, StringValues> HashStringValues; // setting, StringValues
typedef QHash<QString, StringListValues> HashStringListValues; // setting, StringListValues
//
typedef QHash<QString, QTreeWidgetItem*> Items; // platform, item
typedef QHash<QString, Items> ProjectTree; // setting, items
//
class QMakeProject : public QObject
{
	Q_OBJECT
	//
public:
	QMakeProject( const QString&, QTreeWidget* = 0, QObject* = 0 );
	~QMakeProject();
	//
	static const QStringList uiExtension, headerExtension, sourceExtension, translationExtension, textExtension, resourceExtension, rcExtension, resExtension, defExtension;
	static QString qtExtensions();
	static QStringList getProjectNames( QMakeProject*, const QString& );
	static QMakeProject* getProjectByName( const QString&, QMakeProject* );
	static QMakeProject* getProjectByFilePath( const QString&, QMakeProject* );
	// Project
	bool open();
	void setOpen();
	bool isOpen();
	void setModified( bool );
	bool isModified();
	void setLocked( bool locked );
	bool isLocked();
	bool save( bool = false );
	bool close();
	void setClose();
	void setTreeItem( QTreeWidgetItem* );
	QTreeWidgetItem* treeItem();
	QString name();
	QString filePath();
	QString absolutePath();
	QList<QMakeProject*>& subProjects();
	//
	void setStringValues( const HashStringValues& );
	void setStringValues( const QString&, const QString& , const QString& );
	void addStringValue( const QString&, const QString&, const QString& );
	void removeStringValue( const QString&, const QString&, const QString& );
	const QString stringValue( const QString&, const QString& = "(all)" );
	const HashStringValues cStringValues();
	//
	void setStringListValues( const HashStringListValues& );
	void setStringListValues( const QString&, const QString&, const QStringList& );
	void addStringListValue( const QString&, const QString&, const QString& );
	void addStringListValues( const QString&, const QString&, const QStringList& );
	void removeStringListValue( const QString&, const QString&, const QString& );
	const QStringList stringListValue( const QString&, const QString& = "(all)" );
	const HashStringListValues cStringListValues();
	// Building
	QString qmake();
	QString make();
	QString makeClean();
	QString lupdate();
	QString lrelease();
	QString execute();
	// Tools
	const QString currentPlatform();
	const QString absolute( const QString& );
	const QString relative( const QString& );
	static pFileType getFileTypeFromFilePath( const QString& );
private:
	QString parse_part( const QString& );
	QStringList parse_multiline_part( const QString&, const QString& , int* = 0 );
	void remove_contents( QString& , const QString& );
	void remove_multiline_contents( QString&, const QString& , int* = 0 );
	//
	void readPlatformSettings( const QString& );
	void removePlatformSettings( QString& );
	void writePlatformSettings( QString& );
	bool parse();
	void createTreeProject();
	void updateTreeItem( const QString&, const QString&, const QString&, bool );
	//
	QString mFilePath;
	QTreeWidget* mTree;
	ProjectTree mProjectTree;
	bool mOpen;
	bool mModified;
	bool mLocked;
	QList<QMakeProject*> mSubProjects;
	HashStringValues stringValues;
	HashStringListValues stringListValues;
	//
signals:
	void opened( QMakeProject* );
	void closed( QMakeProject* );
	void modified( QMakeProject*, bool );
	void openFile( QMakeProject*, const QString& );
	//
};
//
#endif // QMAKEPROJECT_H
