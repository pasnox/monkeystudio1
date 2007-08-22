/********************************************************************************************************
		* PROGRAM      : TextEditor
		* DATE - TIME  : lundi 01 mai 2006 - 10h45
		* AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
			* FILENAME     : TextEditor.cpp
			* LICENSE      : GPL
			* COMMENTARY   : fullTextIndent & simpleTextIndent are at my knowledge of Mashin Evgeniy ( mashin.zhenya at gmail dot com )
				********************************************************************************************************/
#include "TextEditor.h"
#include "Gluter.h"
#include "Completion.h"
#include "ReplaceDialog.h"
#include "CppSyntaxHighlighter.h"
//
#include <QPainter>
#include <QKeyEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextBlock>
#include <QTextCursor>
#include <QScrollBar>
#include <QAbstractTextDocumentLayout>
#include <QSyntaxHighlighter>
#include <QMenu>
#include <QInputDialog>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QTextStream>
//
TextEditor::TextEditor( QWidget* parent )
	: QTextEdit( parent ), mDefaultComponents( false )
{
	setAttribute( Qt::WA_DeleteOnClose );
	connect( this, SIGNAL( cursorPositionChanged() ), this, SLOT( cursorPositionChanged()));
	setLineWrapMode( QTextEdit::NoWrap );
	setAcceptRichText( false );
	setDefaultProperties();
}
//
TextEditor::~TextEditor()
{
	closeFile();
}
//
void TextEditor::closeEvent( QCloseEvent* e )
{
	QTextEdit::closeEvent( e );
}
//
void TextEditor::resizeEvent( QResizeEvent* e )
{
	QTextEdit::resizeEvent( e );
	if ( mGluter )
	{
		QRect mGeometry( viewport()->geometry().topLeft(), QSize( mGluter->width(), viewport()->height() ) );
		mGeometry.moveLeft( mGeometry.left() -mGluter->width() );
		if ( mGluter->geometry() != mGeometry )
			mGluter->setGeometry( mGeometry );
	}
}
//
void TextEditor::paintEvent( QPaintEvent* e )
{
	QPainter painter( viewport() );
	// CurrentLine
	if ( mHighlightCurrentLine )
	{
		QRect r = cursorRect();
		r.setX( 0 );
		r.setWidth( viewport()->width() );
		painter.fillRect( r, QBrush( mCurrentLineColor ) );
	}
	// LimitLine
	if ( mLimitLine > 0 )
	{
		int x = ( QFontMetrics( font() ).width( "X" ) * mLimitLine ) -horizontalScrollBar()->value();
		painter.setPen( mLimitLineColor );
		painter.drawLine( x, painter.window().top(), x, painter.window().bottom() );
	}
	//
	painter.end();
	//
	QTextEdit::paintEvent( e );
}
//
void TextEditor::keyPressEvent( QKeyEvent* e )
{
	// if auto repeat, process and quit
	if ( e->isAutoRepeat() )
	{
		QTextEdit::keyPressEvent( e );
		return;
	}
	//
	switch( e->modifiers() )
	{
		case Qt::CTRL:
		{
			switch ( e->key() )
			{
				case Qt::Key_I:
				// Invert Selection
				invertSelection();
				break;
				case Qt::Key_F:
				case Qt::Key_H:
				// FindReplace Dialog
				showReplaceDialog();
				break;
				case Qt::Key_G:
				// GotoLine Dialog
				gotoLineDialog();
				break;
				case Qt::Key_S:
				// Save File
				saveFile( false );
				break;
				case Qt::Key_Space:
				// Completion
				if ( mCompletion )
					emit completionRequested( mCompletion, this );
				//mCompletion->invokeCompletion( this );
				break;
				// for debuger
				// set breakpoint
				case Qt::Key_B:
				{
					// return line number
					int bp_at_line = getCursorYPosition();
					if(mGluter)
					{
						// flip/flop breabpoint
						int i= mGluter->breakpointList.indexOf(bp_at_line);
						if( i != -1)
							mGluter->breakpointList.removeAt(i);
						else
							mGluter->breakpointList << bp_at_line;
						
						emit signalBreakpointAt(mFilePath, bp_at_line);
						
						// force update view
						mGluter->update();
					}
				}
				break;
				// end xiantia
				
				default:
				QTextEdit::keyPressEvent( e );
				break;
			}
			break;
		}
		case Qt::SHIFT:
		{
			switch ( e->key() )
			{
				case Qt::Key_F3:
				// Replace Next
				replaceNext();
				break;
				default:
				QTextEdit::keyPressEvent( e );
				break;
			}
			break;
		}
		case Qt::CTRL | Qt::SHIFT:
		{
			switch ( e->key() )
			{
				case Qt::Key_F3:
				// Replace All
				replaceAll();
				break;
				case Qt::Key_N:
				// Select None
				selectNone();
				break;
				default:
				QTextEdit::keyPressEvent( e );
				break;
			}
			break;
		}
		default:
		switch ( e->key() )
		{
			case Qt::Key_F3:
			// Find Next
			searchNext();
			break;
			case Qt::Key_Insert:
			setOverwriteMode( !overwriteMode() );
			emit overwriteModeChanged( overwriteMode() );
			break;
			case Qt::Key_Return:
			case Qt::Key_Enter:
			QTextEdit::keyPressEvent( e );
			updateTextIndent();
			break;
			default:
			QTextEdit::keyPressEvent( e );
			break;
		}
		break;
	}
}
//

QList <int> TextEditor::getAllBreakpoint()
{
	return mGluter->breakpointList;
}

void TextEditor::addBreakpoint(int bp)
{
	mGluter->breakpointList << bp;
}

// for debuger
void TextEditor::gotoDebugerLine(QString file, int line, bool  show)
{
	file=file; // no warning 
//	if( mFilePath  == file) // etre sur que c'est le bon fichier
	{
		if(show) // on affiche la fleche bleu et on va a la ligne
		{
			setTextCursor( getLineCursor( line - 1 ) );
			mGluter->currentDebugerAt = line;
			mGluter->update();
		}
		else // sinon on n'affiche pas la fleche bleu
		{
			mGluter->currentDebugerAt = -1;
			mGluter->update();
		}
	}	
}

// end xiantia

void TextEditor::contextMenuEvent( QContextMenuEvent* e )
{
	// creating the menu for the context menu
	QMenu* menu = createStandardContextMenu();
	// Getting 2 actions for moving
	QList<QAction*> actionsList = menu->findChildren<QAction*>();
	menu->addAction( "Select None", this, SLOT( selectNone() ), tr( "Ctrl+Alt+N" ) );
	menu->addAction( "Invert Selection", this, SLOT( invertSelection() ), tr( "Ctrl+I" ) );
	if ( mReplaceDialog )
	{
		menu->addSeparator();
		menu->addAction( "Find...", this, SLOT( showReplaceDialog() ), tr( "Ctrl+F" ) );
		menu->addAction( "Find Next", this, SLOT( searchNext() ), tr( "F3" ) );
		menu->addAction( "Replace...", this, SLOT( showReplaceDialog() ), tr( "Ctrl+H" ) );
		menu->addAction( "Replace Next", this, SLOT( replaceNext() ), tr( "Shift+F3" ) );
		menu->addAction( "Replace All", this, SLOT( replaceAll() ), tr( "Ctrl+Shift+F3" ) );
		menu->addAction( "Goto Line...", this, SLOT( gotoLineDialog() ), tr( "Ctrl+G" ) );
	}
	menu->addAction( actionsList[ 10 ] );
	menu->addAction( actionsList[ 11 ] );
	actionsList.clear();
	menu->exec( e->globalPos() );
	delete menu;
}
//
void TextEditor::cursorPositionChanged()
{
	if ( mHighlightCurrentLine )
		viewport()->update();
	emit cursorPositionChanged( getCursorXPosition(), getCursorYPosition() );
}
// PROPERTIES
void TextEditor::setIndentMethod( IndentMethod im )
{
	if ( im == mIndentMethod )
		return;
	mIndentMethod = im;
	updateTextIndent();
}
//
TextEditor::IndentMethod TextEditor::indentMethod() const
{
	return mIndentMethod;
}
//
void TextEditor::setHighlightCurrentLine( bool b )
{
	if ( b == mHighlightCurrentLine )
		return;
	mHighlightCurrentLine = b;
	viewport()->update();
}
//
bool TextEditor::highlightCurrentLine() const
{
	return mHighlightCurrentLine;
}
//
void TextEditor::setCurrentLineColor( const QColor& c )
{
	if ( c == mCurrentLineColor )
		return;
	mCurrentLineColor = c;
	viewport()->update();
}
//
const QColor& TextEditor::currentLineColor() const
{
	return mCurrentLineColor;
}
//
void TextEditor::setLimitLine( int i )
{
	if ( i == mLimitLine )
		return;
	mLimitLine = i;
	viewport()->update();
}
//
int TextEditor::limitLine() const
{
	return mLimitLine;
}
//
void TextEditor::setLimitLineColor( const QColor& c )
{
	if ( c == mLimitLineColor )
		return;
	mLimitLineColor = c;
	viewport()->update();
}
//
const QColor& TextEditor::limitLineColor() const
{
	return mLimitLineColor;
}
//
void TextEditor::setTabSize( int i )
{
	if ( i == mTabSize )
		return;
	mTabSize = i;
	setTabStopWidth( fontMetrics().width( "x" ) * mTabSize );
	setPlainText( toPlainText() );
	// need something to go to last position
}
//
int TextEditor::tabSize() const
{
	return mTabSize;
}
//
void TextEditor::setBackupFile( bool b )
{
	if ( b == mBackupFile )
		return;
	mBackupFile = b;
}
//
bool TextEditor::backupFile() const
{
	return mBackupFile;
}
//
void TextEditor::setFilePath( const QString& s )
{
	if ( s == mFilePath )
		return;
	mFilePath = s;
	setWindowTitle( mFilePath );
	emit filePathChanged( mFilePath );
}
//
QString TextEditor::filePath() const
{
	return mFilePath;
}
//
void TextEditor::setFileOpen( bool b )
{
	if ( b == mFileOpen )
		return;
	mFileOpen = b;
	emit fileOpen( b );
}
//
bool TextEditor::fileOpen() const
{
	return mFileOpen;
}
//
void TextEditor::setScrollLines( int i )
{
	if ( i == mScrollLines )
		return;
	mScrollLines = i;
	verticalScrollBar()->setSingleStep( mScrollLines );
}
//
int TextEditor::scrollLines() const
{
	return mScrollLines;
}
//
void TextEditor::setSelectionColor( const QColor& c )
{
	if ( c == mSelectionColor )
		return;
	mSelectionColor = c;
	QPalette p( palette() );
	p.setColor( QPalette::Highlight, mSelectionColor );
	setPalette( p );
	viewport()->update();
}
//
const QColor& TextEditor::selectionColor() const
{
	return mSelectionColor;
}
//
void TextEditor::setBackgroundColor( const QColor& c )
{
	if ( c == mBackgroundColor )
		return;
	mBackgroundColor = c;
	QPalette p( palette() );
	p.setColor( QPalette::Base, mBackgroundColor );
	setPalette( p );
	viewport()->update();
}
//
const QColor& TextEditor::backgroundColor() const
{
	return mBackgroundColor;
}
// END PROPERTIES
void TextEditor::setGluter( Gluter* g )
{
	if ( g == mGluter )
		return;
	if ( mGluter )
		delete mGluter;
	mGluter = g;
	if ( mGluter )
	{
		setViewportMargins( mGluter->width(), 0, 0, 0 );
		if ( !mGluter->isVisible() )
			mGluter->show();
	}
	else
		setViewportMargins( 0, 0, 0, 0 );
}
//
Gluter* TextEditor::gluter()
{
	return mGluter;
}
//
void TextEditor::setCompletion( Completion* c )
{
	if ( c == mCompletion )
		return;
	if ( mCompletion )
		delete mCompletion;
	mCompletion = c;
}
//
Completion* TextEditor::completion()
{
	return mCompletion;
}
//
void TextEditor::setSyntaxHighlighter( QSyntaxHighlighter* sh )
{
	if ( sh == mSyntaxHighlighter )
		return;
	if ( mSyntaxHighlighter )
		delete mSyntaxHighlighter;
	mSyntaxHighlighter = sh;
	if ( mSyntaxHighlighter && !mSyntaxHighlighter->document() )
		mSyntaxHighlighter->setDocument( this->document() );
}
//
QSyntaxHighlighter* TextEditor::syntaxHighlighter()
{
	return mSyntaxHighlighter;
}
//
void TextEditor::setReplaceDialog( ReplaceDialog* rd )
{
	if ( rd == mReplaceDialog )
		return;
	if ( !rd && mReplaceDialog && mReplaceDialog->mode() == ReplaceDialog::mDocked )
		return;
	if ( mReplaceDialog )
		delete mReplaceDialog;
	mReplaceDialog = rd;
}
//
ReplaceDialog* TextEditor::replaceDialog()
{
	return mReplaceDialog;
}
//
void TextEditor::setDefaultComponents( bool o )
{
	if ( o )
	{
		if ( !mGluter )
			setGluter( new Gluter( this ) );
		if ( !mCompletion )
			setCompletion( Completion::self() );
		if ( !mSyntaxHighlighter )
			setSyntaxHighlighter( new CppSyntaxHighlighter( document() ) );
		if ( !mReplaceDialog )
			setReplaceDialog( ReplaceDialog::self() );
	}
	else
	{
		setGluter( 0 );
		setCompletion( 0 );
		setSyntaxHighlighter( 0 );
		setReplaceDialog( 0 );
	}
	mDefaultComponents = o;
}
//
bool TextEditor::defaultComponents() const
{
	return mDefaultComponents;
}
//
int TextEditor::getCursorXPosition() const
{
	if ( !textCursor().block().isValid() )
		return 0;
	QTextCursor c = textCursor();
	QTextBlock b = c.block();
	int x = c.position() -b.position();
	return x;
}
//
int TextEditor::getCursorYPosition() const
{
	if ( !textCursor().block().isValid() )
		return 0;
	QTextBlock b;
	int y = 1;
	for( b = document()->begin(); b.isValid(); b = b.next(), y++ )
		if ( b == textCursor().block() )
			break;
	return y;
}
//
QPoint TextEditor::getCursorPosition() const
{
	return QPoint( getCursorXPosition(), getCursorYPosition() );
}
//
int TextEditor::linesCount() const
{
	int count = 0;
	for ( QTextBlock b = document()->begin(); b.isValid(); b = b.next() )
		count++;
	return count;
}
//
QTextCursor TextEditor::getLineCursor( int line ) const
{
	int count = 0;
	for ( QTextBlock b = document()->begin(); b.isValid(); b = b.next(), count++ )
	{
		if ( count == line )
		{
			return QTextCursor( b );
			break;
		}
	}
	QTextCursor c = textCursor();
	c.movePosition( QTextCursor::End );
	c.movePosition( QTextCursor::StartOfLine );
	return c;
}
//
bool TextEditor::openFile( const QString& fp, QTextCodec* c )
{
	if ( !fp.isNull() )
		setFilePath( fp );
	if ( mFilePath.isNull() )
		return false;
	if ( mFileOpen )
		closeFile();
	QFile f( mFilePath );
	if ( f.open( QFile::ReadOnly | QFile::Text ) )
	{
		if ( !c )
			c = QTextCodec::codecForLocale();
		QTextStream t( &f );
		t.setCodec( c );
		setPlainText( t.readAll() );
		document()->setModified( false );
		f.close();
		if ( !mFileOpen )
			setFileOpen( true );
		return true;
	}
	return false;
}
//
bool TextEditor::saveFile( bool a, QTextCodec* c )
{
	bool modified = document()->isModified(), haveFilePath = !mFilePath.isNull();
	//
	if ( !modified )
		return true;
	if ( modified && !haveFilePath )
	{
		const QString fp = QFileDialog::getSaveFileName( this, tr( "Choose a filename to save under" ), QDir::currentPath(), tr( "All Files (*)" ) );
		if ( fp.isEmpty() )
			return false;
		setFilePath( fp );
		a = false;
	}
	if ( !a || ( a && QMessageBox::question( this, tr( "Save File..." ), QString( tr( "This file has been modified,\n[%1],\nsave it ?" ).arg( mFilePath ) ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes ) )
	{
		// Creating bakup
		if ( mBackupFile )
		{
			QString bfp = mFilePath + ".bak";
			if ( QFile::exists( bfp ) )
				QFile::remove( bfp );
			emit fileBackup( bfp, QFile::copy( mFilePath, bfp ) );
		}
		QFile f( mFilePath );
		if ( f.open( QFile::WriteOnly | QFile::Text ) )
		{
			if ( !c )
				c = QTextCodec::codecForLocale();
			QTextStream t( &f );
			t.setCodec( c );
			t << toPlainText();
			f.close();
			document()->setModified( false );
			if ( !mFileOpen )
				setFileOpen( true );
			emit fileSaved( mFilePath, true );
			return true;
		}
	}
	emit fileSaved( mFilePath, false );
	return false;
}
//
void TextEditor::closeFile()
{
	saveFile( true );
	clear();
	setFileOpen( false );
}
// SLOTS
void TextEditor::gotoLine( int line )
{
	setTextCursor( getLineCursor( line ) );
}
//
void TextEditor::selectNone()
{
	if ( !textCursor().selectedText().isEmpty() )
	{
		QTextCursor c( textCursor() );
		c.clearSelection();
		setTextCursor( c );
	}
}
//
void TextEditor::invertSelection()
{
	qWarning( "Invert Selection : not yet implemented" );
}
//
void TextEditor::showReplaceDialog()
{
	if ( mReplaceDialog )
	{
		mReplaceDialog->setWorkingEdit( this );
		emit replaceDialogRequested();
		mReplaceDialog->tasks();
	}
}
//
void TextEditor::searchNext()
{
	if ( mReplaceDialog )
		mReplaceDialog->searchNext();
}
//
void TextEditor::replaceNext()
{
	if ( mReplaceDialog )
		mReplaceDialog->replaceNext();
}
//
void TextEditor::replaceAll()
{
	if ( mReplaceDialog )
		mReplaceDialog->replaceAll();
}
//
void TextEditor::gotoLineDialog()
{
	bool ok;
	int i = QInputDialog::getInteger( this, tr( "Goto Line..." ), tr( "Enter the line number you want to go to:" ), 1, 1, linesCount(), 1, &ok );
	if ( ok )
		gotoLine( i -1 );
}
//
void TextEditor::printDocument( bool q )
{
	if ( document()->toPlainText().isEmpty() )
		return;
	QPrinter p( QPrinter::HighResolution );
	p.setFullPage( true );
	if ( !q )
	{
		QPrintDialog pd( &p, this );
		if ( pd.exec() == QDialog::Accepted )
			document()->print( &p );
	}
	else
		document()->print( &p );
}
//
void TextEditor::updateTextIndent()
{
	switch ( mIndentMethod )
	{
		case 0: // nothing
		break;
		case 1: // simple
		simpleTextIndent();
		break;
		case 2: // full
		fullTextIndent();
		break;
	}
}
//
void TextEditor::fullTextIndent()
{
	QTextBlock blockPrev = document()->begin();
	QTextBlock block = blockPrev.next();
	bool indentYet = false;
	QString tmpIndent = QString();
	QString switchIndent = QString();
	int switchCount = 0;
	int caseCount = 0;
	bool inSwitch = false;
	bool inCase = false;
	int firstLetter = -1;
	int bracketCount = 0;
	int roundBracketCount = 0;
	for(; block!= document()->end(); block = block.next())
	{
		if (blockPrev.userState() == 1)
		{
			blockPrev = block;
			continue;
		}
		QString curText = block.text();
		QString prevText = block.previous().text();
		QString fullCopy = curText;
		firstLetter = prevText.indexOf(QRegExp("\\S"));
		if (firstLetter>0 	&& !indentYet)
			tmpIndent = prevText.left(firstLetter);
		//normilize text
		prevText = prevText.trimmed();
		curText = curText.trimmed();
		QString curTmp = curText;
		
		QRegExp reg;
		// remove text in qoute
		reg = QRegExp("\".*\"");
		reg.setMinimal(true);
		if (!prevText.endsWith("\""))
			prevText.remove(reg);
		if (!curTmp.endsWith("\""))
			curTmp.remove(reg);
		reg = QRegExp("\'.*\'");
		reg.setMinimal(true);
		if (!prevText.endsWith("\'"))
			prevText.remove(reg);
		if (!curTmp.endsWith("\'"))
			curTmp.remove(reg);
		// end remove text in qoute
		// remove comments
		reg = QRegExp("/\\*.*\\*/");
		reg.setMinimal(true);
		prevText.remove(reg);
		curTmp.remove(reg);
		reg = QRegExp("//[^\n]*");
		prevText.remove(reg);
		curTmp.remove(reg);
		prevText.remove(QRegExp("(/\\*)"));
		curTmp.remove(QRegExp("(/\\*)"));
		prevText.remove(QRegExp("(\\*/)"));
		curTmp.remove(QRegExp("(\\*/)"));
		prevText = prevText.trimmed();
		curTmp = curTmp.trimmed();
		// end remove comments
		//end normilize text
		QString indent;
		if (curTmp.indexOf(QRegExp("^\\b(public|protected|private|signals)\\b")) !=-1 ||
				prevText.indexOf(QRegExp("^\\bclass\\b"))!=-1 || curTmp.indexOf(QRegExp("^\\bclass\\b"))!=-1)
		{
			if (curTmp.indexOf(QRegExp("^\\b(public|protected|private|signals)\\b"))!=-1 &&
					(prevText.count("{")>0 || prevText.count("}")>0))
			{
				int open = prevText.count("{");
				int close = prevText.count("}");
				int brCount = open - close;
				bracketCount+=brCount;
				//				cout <<prevText.toStdString()<<endl;
				//				cout <<"bc= "<<bracketCount<<endl;
			}
			
			indent = QString();
		}
		else
		{
			// switch case
			if (curTmp.indexOf(QRegExp("^\\bswitch\\b"))!=-1 && curTmp.endsWith("{")||
					prevText.indexOf(QRegExp("^\\bswitch\\b"))!=-1 && curTmp.startsWith("{") )
			{
				inSwitch = true;
			}
			
			if (inSwitch && ((curTmp.startsWith("{")&&prevText.indexOf(QRegExp("^\\b(case|default)\\b"))!=-1)||
					(curTmp.endsWith("{")&&curTmp.indexOf(QRegExp("^\\b(case|default)\\b"))!=-1)))
			{
				inCase = true;
			}
			
			if (inSwitch)
			{
				if (prevText.startsWith("}") || prevText.endsWith("}"))
				{
					switchCount-=1;
					if (inCase)
						caseCount-=1;
				}
				if (curTmp.startsWith("{") || curTmp.endsWith("{"))
				{
					switchCount+=1;
					if (inCase)
						caseCount+=1;
				}
			}
			if (inSwitch)
			{
				if (curTmp.indexOf(QRegExp("^\\b(switch|case|default)\\b"))!=-1)
					switchIndent="";
				else if ((curTmp.startsWith("{") && prevText.indexOf(QRegExp("^\\b(switch|case|default)\\b"))!=-1 ) ||
						(curTmp.endsWith("{") && curTmp.indexOf(QRegExp("^\\b(switch|case|default)\\b"))!=-1))
					switchIndent="";
				else if (switchCount == 0 ||
						(switchCount == 1 && (curTmp.startsWith("}") || curTmp.endsWith("}")) ))
				{
					switchIndent="";
					inSwitch = false;
				}
				else if (inCase)
				{
					switchIndent="";
					if ((caseCount == 0) ||
							(caseCount == 1 && (curTmp.startsWith("}") || curTmp.endsWith("}")) ))
						inCase = false;
				}
				else
					switchIndent="\t";//QString::number(switchCount)+QString::number(caseCount);
			}
			//end switch case
			curTmp.remove("\\s"); // remove whitespace
			
			if ( (prevText.startsWith("{")  &&  (prevText.indexOf(QRegExp("};?$")) != -1) ) || prevText.endsWith("{")) //why !=-1
			{
				bracketCount+=1;
			}
			if ( curTmp.startsWith("}") || curTmp.indexOf(QRegExp("};?$"))!=-1 )
				bracketCount-=1;
			
			if (curTmp.indexOf(QRegExp("^\\bnamespace\\b(.*)\\{$"))!=-1 ||
					(curTmp.startsWith("{") && prevText.indexOf(QRegExp("^\\bnamespace\\b"))!=-1))
				bracketCount -=1;
			
			QString bracketIndent = QString();
			for (int i=1; i <= bracketCount; ++i)
				bracketIndent+="\t";
			
			roundBracketCount += prevText.count("(")-prevText.count(")");
			
			if ( !curTmp.startsWith("{")  && !curTmp.startsWith(".") && !roundBracketCount &&
					(prevText.endsWith(")") || prevText.indexOf(QRegExp("^\\b(else|do)\\b$"))!=-1) )//&& !(prevText.count("(") - prevText.count(")")) )
			{
				tmpIndent.truncate(tmpIndent.count()-bracketIndent.count());
				tmpIndent+="\t";
			}
			else if ( (!curTmp.startsWith("#") &&
					!prevText.isEmpty() &&
					!curTmp.isEmpty() &&
					//!curTmp.startsWith("{") &&
					!prevText.endsWith("{") &&
					!prevText.endsWith("}") &&
					!prevText.endsWith(")") &&
					!prevText.endsWith(";") &&
					!prevText.endsWith(":") &&
					!prevText.endsWith("const") &&
					prevText.indexOf(QRegExp("\\b(else|do)\\b$")) == -1 ) ||
					curTmp.startsWith(".") ||
					roundBracketCount>0)
			{
				if (!indentYet)
				{
					indentYet = true;
					tmpIndent.truncate(tmpIndent.count()-bracketIndent.count());
					if (!curTmp.startsWith("{"))
						tmpIndent+="\t\t";
				}
			}
			else if (prevText.endsWith("{") && indentYet)
			{}
			else
			{
				tmpIndent  = QString();
				if (indentYet) 	indentYet = false;
			}
			indent = bracketIndent  + switchIndent + tmpIndent ;
		} // if (public|private...
		
		curText = indent + curText;
		// start align
		if (QString::compare(curText, fullCopy))
		{
			QTextCursor cursor = QTextCursor(block);
			cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
			cursor.insertText(curText);
		}
		blockPrev = block;
	}
}
//
void TextEditor::simpleTextIndent()
{
	QTextBlock block = textCursor().block();
	QTextBlock blockPrev = block.previous();
	QString tmpIndent = QString();
	int firstLetter = -1;
	QString curText = block.text();
	QString prevText = block.previous().text();
	QString fullCopy = curText;
	firstLetter = prevText.indexOf(QRegExp("\\S"));
	if (firstLetter!=-1)
		tmpIndent = prevText.left(firstLetter);
	//
	curText = curText.trimmed();
	curText = tmpIndent + curText;
	//
	if (QString::compare(curText, fullCopy))
	{
		QTextCursor cursor = QTextCursor(block);
		cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
		cursor.insertText(curText);
	}
}
//
void TextEditor::setDefaultProperties()
{
	// Default properties
	setIndentMethod( imSimple );
	setHighlightCurrentLine( true );
	setCurrentLineColor( QColor( "#eef6ff" ) );
	setLimitLine( 80 );
	setLimitLineColor( Qt::red );
	setTabSize( 4 );
	setBackupFile( false );
	setFileOpen( false );
	setScrollLines( 4 );
	setTextColor( Qt::black );
	setSelectionColor( QColor( "#aaaaff" ) );
	setBackgroundColor( Qt::white );
}
