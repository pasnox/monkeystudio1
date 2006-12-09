/********************************************************************************************************
		* PROGRAM      : TextEditor
		* DATE - TIME  : lundi 01 mai 2006 - 10h45
		* AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
			* FILENAME     : TextEditor.h
			* LICENSE      : GPL
			* COMMENTARY   : fullTextIndent & simpleTextIndent are at my knowledge of Mashin Evgeniy ( mashin.zhenya at gmail dot com )
				********************************************************************************************************/
#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H
//
#include <QTextEdit>
#include <QPointer>
#include <QDesignerExportWidget>
//
class Gluter;
class Completion;
class QSyntaxHighlighter;
class ReplaceDialog;
class QTextCodec;
//
#ifndef TEXTEDITORPLUGIN
#undef QDESIGNER_WIDGET_EXPORT
#define QDESIGNER_WIDGET_EXPORT
#endif
//
class QDESIGNER_WIDGET_EXPORT TextEditor : public QTextEdit
{
	Q_OBJECT
			Q_ENUMS( IndentMethod )
			Q_PROPERTY( IndentMethod indentMethod READ indentMethod WRITE setIndentMethod )
			Q_PROPERTY( bool highlightCurrentLine READ highlightCurrentLine WRITE setHighlightCurrentLine )
			Q_PROPERTY( QColor currentLineColor READ currentLineColor WRITE setCurrentLineColor )
			Q_PROPERTY( int limitLine READ limitLine WRITE setLimitLine )
			Q_PROPERTY( QColor limitLineColor READ limitLineColor WRITE setLimitLineColor )
			Q_PROPERTY( int tabSize READ tabSize WRITE setTabSize )
			Q_PROPERTY( bool backupFile READ backupFile WRITE setBackupFile )
			Q_PROPERTY( QString filePath READ filePath WRITE setFilePath )
			Q_PROPERTY( bool fileOpen READ fileOpen WRITE setFileOpen )
			Q_PROPERTY( int scrollLines READ scrollLines WRITE setScrollLines )
			Q_PROPERTY( QColor selectionColor READ selectionColor WRITE setSelectionColor )
			Q_PROPERTY( QColor backgroundColor READ backgroundColor WRITE setBackgroundColor )
			Q_PROPERTY( bool defaultComponents READ defaultComponents WRITE setDefaultComponents )
			//
public:
	TextEditor( QWidget* = 0 );
	~TextEditor();
	//
	enum IndentMethod { imNone = 0, imSimple, imFull };
	void setIndentMethod( IndentMethod );
	IndentMethod indentMethod() const;
	//
	void setHighlightCurrentLine( bool );
	bool highlightCurrentLine() const;
	//
	void setCurrentLineColor( const QColor& );
	const QColor& currentLineColor() const;
	//
	void setLimitLine( int );
	int limitLine() const;
	//
	void setLimitLineColor( const QColor& );
	const QColor& limitLineColor() const;
	//
	void setTabSize( int );
	int tabSize() const;
	//
	void setBackupFile( bool );
	bool backupFile() const;
	//
	void setFilePath( const QString& );
	QString filePath() const;
	//
	void setFileOpen( bool );
	bool fileOpen() const;
	//
	void setScrollLines( int );
	int scrollLines() const;
	//
	void setSelectionColor( const QColor& );
	const QColor& selectionColor() const;
	//
	void setBackgroundColor( const QColor& );
	const QColor& backgroundColor() const;
	//
	void setGluter( Gluter* );
	Gluter* gluter();
	//
	void setCompletion( Completion* );
	Completion* completion();
	//
	void setSyntaxHighlighter( QSyntaxHighlighter* );
	QSyntaxHighlighter* syntaxHighlighter();
	//
	void setReplaceDialog( ReplaceDialog* );
	ReplaceDialog* replaceDialog();
	//
	void setDefaultComponents( bool );
	bool defaultComponents() const;
	//
	int getCursorXPosition() const;
	int getCursorYPosition() const;
	QPoint getCursorPosition() const;
	int linesCount() const;
	QTextCursor getLineCursor( int line ) const;
	bool openFile( const QString& = QString::null, QTextCodec* = 0 );
	bool saveFile( bool = false, QTextCodec* = 0 );
	//
protected:
	void closeEvent( QCloseEvent* );
	void resizeEvent( QResizeEvent* );
	void paintEvent( QPaintEvent* );
	void keyPressEvent( QKeyEvent* );
	void contextMenuEvent( QContextMenuEvent* );
	//
private:
	// Properties
	IndentMethod mIndentMethod;
	bool mHighlightCurrentLine;
	QColor mCurrentLineColor;
	int mLimitLine;
	QColor mLimitLineColor;
	int mTabSize;
	bool mBackupFile;
	QString mFilePath;
	bool mFileOpen;
	int mScrollLines;
	QColor mSelectionColor;
	QColor mBackgroundColor;
	bool mDefaultComponents;
	//
	QPointer<Gluter> mGluter;
	QPointer<Completion> mCompletion;
	QPointer<QSyntaxHighlighter> mSyntaxHighlighter;
	QPointer<ReplaceDialog> mReplaceDialog;
	//
public slots:
	void closeFile();
	void gotoLine( int line );
	void selectNone();
	void invertSelection();
	void showReplaceDialog();
	void searchNext();
	void replaceNext();
	void replaceAll();
	void gotoLineDialog();
	void printDocument( bool = true );
	void updateTextIndent();
	void fullTextIndent();
	void simpleTextIndent();
	//
	void setDefaultProperties();
	//
	void gotoDebugerLine(QString file, int line , bool show);
	void addBreakpoint(int bp);
	QList <int> getAllBreakpoint();

private slots:
	void cursorPositionChanged();
	//
signals:
	void replaceDialogRequested();
	void beforeCompletionShow();
	void fileOpen( bool );
	void overwriteModeChanged( bool );
	void cursorPositionChanged( int, int );
	void filePathChanged( const QString& );
	void fileBackup( const QString&, bool );
	void fileSaved( const QString&, bool );
	void completionRequested( Completion*, TextEditor* );
	//
	// xiantia debuger CTRL +B
	void signalBreakpointAt(QString, int);
	
};
//
#endif // TEXTEDITOR_H
