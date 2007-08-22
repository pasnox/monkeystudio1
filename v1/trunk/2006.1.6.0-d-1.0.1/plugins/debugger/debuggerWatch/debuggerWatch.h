#ifndef plug_H
#define plug_H

#include <QPointer>
#include <QWidget>
#include <QMouseEvent>
#include <QIcon>
#include <QPushButton>

#include "interface_plugin.h"

// your include
#include <QTreeWidget>
#include <QBoxLayout>


// my define 

#define GET_TYPE		PLUGIN_BASE_USER+1
#define GET_ADDR		PLUGIN_BASE_USER+2
#define GET_VALUE		PLUGIN_BASE_USER+3
#define GET_TRANSLATE		PLUGIN_BASE_USER+4
#define SET_VALUE		PLUGIN_BASE_USER+5



// derivation de la class QTreeWidget afin d'avoir le mousePressEvent
class QTreeWidgetEx : public QTreeWidget 
{
	Q_OBJECT
public : 
	QTreeWidgetEx(){};
	QString memItemText;

	void dragEnterEvent(QDragEnterEvent *event);
 	void dropEvent(QDropEvent *event);
	void dragMoveEvent(QDragMoveEvent *event);

private slots:
	void mousePressEvent ( QMouseEvent * event ) ;
//	void itemEntered ( QTreeWidgetItem * item, int column ); 


signals: 
	void signalPluginMouse(QMouseEvent *);
//	void itemEntered ( QTreeWidgetItem * item, int column );
};



class Watch :  public QObject, public UIdebugger_Plugin
{
	Q_OBJECT
	Q_INTERFACES(UIdebugger_Plugin)
 
public:
	// call when your plugin is loaded
	QPointer <QWidget> pluginGetWidget();
	void pluginInitWidget();	

	QString pluginMenuName();
	QIcon pluginMenuIcon();
	QString pluginName();
	QStringList pluginInfos();


	int  pluginDataFromGdb(struct UIdebugger_struct * debugger_struct);
	void pluginGdbStarted(bool);

private :
	struct UIdebugger_struct debugger_struct_out;		// stucture de sortie
	QTreeWidgetItem *qtwiCurrentVariable;			// varialbe courtante
	bool bGdbStarted;								// gsbStarted ?
	bool bNotify;									// notification d'un autre plugin

	// contenaire
	QFrame *frContener;
	QPushButton bAddVariable;
	QPushButton bDelVariable;
	QTreeWidgetEx *trWatch;
	QHBoxLayout *layoutH;
	QVBoxLayout *layoutV;

	// TYPE
	void getVariableType(UIdebugger_struct *,QString variableName);
	int getType(struct UIdebugger_struct * debugger_stuct);
	QString formatVariableType(QString variableType);

	// ADRESSE
	void getVariableAdresse(UIdebugger_struct *,QString variableName);
	int getAdresse(UIdebugger_struct *debugger_struct);
	QString formatVariableAdresse(QString variableAdresse);

	// VALUE
	void getVariableValue(UIdebugger_struct *,QString variableName);	
	int getValue(UIdebugger_struct *debugger_struct);
	QString formatVariableValue(QString variableValue);
	
	// retourne l'item qui suis celui en cours
	QTreeWidgetItem * getVariableNext(QTreeWidgetItem * variableName);

	void isVariableChangedValue(QTreeWidgetItem *id ,QString variableValue);

	// fonction de deformatage des structures			fichier plugin_decompil.cpp
	QStringList extractMember(QString val);		// extrait chaque membre de la strcuture
	QString checkBlock(QString val);			// supprime tout les doubles '{'
	int memberType(QString val);			// retourne le type de membre
	QStringList formatMember(QString val);
	void decompilStrut(QTreeWidgetItem *parentItem, QString val);	// ajoute a la variable ces membres
	void decompilStrutUpdate(QTreeWidgetItem *parentItem,QString val); // mise a jour des membres


	// fonction de traduction
	void translateReInit();
	void setStructure(int to, int userCmd, QString cmd);

	int indexTranslate;
	QString qsSize;
	QString qsAdr;
	QString tmpTanslate;
	int getTranslate(struct UIdebugger_struct *debugger_struct);
	QString getPointeurType(QString val);
	QString translatVariable(struct UIdebugger_struct * debugger_struct);

	// QString
	QString translateQString(struct UIdebugger_struct * debugger_struct);

	// QList
	QString translateQList(struct UIdebugger_struct * debugger_struct,QString Type);


private slots:

	void onAddVariable();
	void onDelVariable();
	void itemChanged( QTreeWidgetItem *id, int column );
	void itemDoubleClicked( QTreeWidgetItem * item, int column ); 
	void currentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous ) ;
signals : 
	void signalPluginAction(UIdebugger_Plugin * ,struct UIdebugger_struct *debugger_struct);
	void signalPluginMouse(QMouseEvent *);
};

#endif
