/********************************************************************************************************
		* PROGRAM      : qt4ds
		* DATE - TIME  : lundi 26 d�embre 2005 - 12h41
		* AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
			* FILENAME     : MDIChild.cpp
			* LICENSE      : GPL
			* COMMENTARY   :
********************************************************************************************************/
#include "MDIChild.h"
#include "UIMain.h"
#include "Options.h"
#include "Tools.h"
#include "UIHeaderInformations.h"
#include "CppSyntaxHighlighter.h"
#include "Completion.h"
//
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QCloseEvent>
#include <QScrollBar>

#include <QMessageBox>
//
MDIChild::MDIChild( QWidget* parent )
	: QFrame( parent ), mType( ctEmpty ), mProject( 0L )
{
	setAttribute( Qt::WA_DeleteOnClose );
	setFrameShape( QFrame::StyledPanel );
	setFrameShadow( QFrame::Plain );
}
//
void MDIChild::showEvent( QShowEvent* e )
{
	emit activated( this );
	QWidget::showEvent( e );
}
//
void MDIChild::closeEvent( QCloseEvent* e )
{
	emit activated( 0 );
	emit removeTab( this );
	QFrame::closeEvent( e );
}
//
void MDIChild::setSettings( TextEditor* e )
{
	QFont myFont;
	//
	e->setBackupFile( Options::self()->value( "Options/General/CreateBackup" ).toBool() );
	e->setTabSize( Options::self()->value( "Options/Editor/TextTabSpacing" ).toInt() );
	myFont.fromString( Options::self()->value( "Options/Editor/TextFont" ).toString() );
	e->setFont( myFont );
	e->setTextColor( Options::self()->value( "Options/Editor/TextForegroundColor" ).toString() );
	e->setSelectionColor( QColor( Options::self()->value( "Options/Editor/TextSelectionColor" ).toString() ) );
	e->setBackgroundColor( QColor( Options::self()->value( "Options/Editor/TextBackgroundColor" ).toString() ) );
	e->setScrollLines( Options::self()->value( "Options/Editor/TextScrollLines" ).toInt() );
	e->setLimitLine( Options::self()->value( "Options/Editor/LimitLine" ).toInt() );
	e->setLimitLineColor( QColor( Options::self()->value( "Options/Editor/LimitLineColor" ).toString() ) );
	e->setHighlightCurrentLine( Options::self()->value( "Options/Editor/HighlightCurrentLine" ).toBool() );
	e->setCurrentLineColor( QColor( Options::self()->value( "Options/Editor/CurrentLineColor" ).toString() ) );
	e->setIndentMethod( (TextEditor::IndentMethod)Options::self()->value( "Options/Editor/SourceIndentationMethod" ).toInt() );
	if ( e->syntaxHighlighter() )
		qobject_cast<CppSyntaxHighlighter*>( e->syntaxHighlighter() )->loadColorSchemeFromSettings( Options::self() );
}
//
void MDIChild::setType( const childType type )
{ mType = type; }
//
childType MDIChild::type()
{ return mType; }
//
void MDIChild::setTabFilePath( const QString& tabFilePath )
{
	mTabFilePath = tabFilePath;
	setWindowTitle( QFileInfo( mTabFilePath ).fileName() );
}
//
QString MDIChild::tabFilePath() const
{ return mTabFilePath; }
//
void MDIChild::setProject( QMakeProject* project )
{ mProject = project; }
//
QMakeProject* MDIChild::project()
{ return mProject; }
//

void MDIChild::fileOpened( bool opened )
{

	TextEditor* editor = qobject_cast<TextEditor*>( sender() );
	if ( !editor )
		return;
	//
	connect(editor,SIGNAL(signalBreakpointAt(QString,int)),this,SIGNAL(signalBreakpointAt(QString,int)));
	
	if ( !opened )
	{
		if ( mProject )
			mProject->removeStringListValue( "OPENEDFILES", "(all)", editor->filePath() );
		return;
	}
	//
	QString content = editor->toPlainText();
	if ( mProject && Options::self()->value( "Options/General/ShowHeaderInformations", true ).toBool() && !content.contains( "* PROGRAM" ) )
	{
		QString informations = UIHeaderInformations::self( mProject, editor->filePath(), this )->getInformations();
		if ( !informations.isNull() )
		{
			content.prepend( informations );
			editor->setPlainText( content );
			editor->document()->setModified( true );
		}
	}
	
	if ( mProject )
	{
		mProject->addStringListValue( "OPENEDFILES", "(all)", editor->filePath() );

		// xiantia 
		// lecture des breakpoint present dans le .pro
/*		QStringList s = mProject->stringListValue( "DEBUGGER");
		for(int i=0; i<s.count(); i++)
		{
			QStringList l = s.at(i).split(":");
			if(l.at(0) == QFileInfo(editor->filePath()).fileName())
			{
				for(int k= 0; k<l.count()-1; k++)
				{
					editor->addBreakpoint(l.at(k+1).toInt());
// ici le signal est emit mais non recu non connect avec UIMain, il
// l'ais apres ?? BUG					
fprintf(stdout,"emit signal\n");				
	emit signalBreakpointAt(QFileInfo(editor->filePath()).fileName(),l.at(k+1).toInt());
				}
			}
		}*/
		// end
	}
}
//
void MDIChild::save( bool )
{
	/* ReImplement and save your files according to bool all */
}
//
bool MDIChild::isModified()
{
	/* Must be reimplemented */
	return false;
}
//
void MDIChild::setEditorFont( const QFont& )
{
	/* Must be reimplemented */
}
//
QString MDIChild::currentFilePath()
{
	/* Must be reimplemented */
	return QString();
}
//
void MDIChild::beforeCompletionShow()
{
	/* Must be reimplemented */
}
//
void MDIChild::completionRequested( Completion* c, TextEditor* t )
{
	c->invokeCompletion( t, project() );
}
