/********************************************************************************************************
 * PROGRAM      : TextEditor
 * DATE - TIME  : lundi 01 mai 2006 - 12h48
 * AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
 * FILENAME     : CppSyntaxHighlighter.h
 * LICENSE      : GPL
 * COMMENTARY   : This CppSyntaxHighlighter is based on the QSASyntaxHighlighter from trolltech, 
 as it has been modified to suits my needs, it must be considerated as a derivated work.
 ********************************************************************************************************/
//
#ifndef CPPSYNTAXHIGHLIGHTER_H
#define CPPSYNTAXHIGHLIGHTER_H
//
#include <QSyntaxHighlighter>
#include <QDesignerExportWidget>
class QSettings;
//
#ifndef TEXTEDITORPLUGIN
#undef QDESIGNER_WIDGET_EXPORT
#define QDESIGNER_WIDGET_EXPORT
#endif
//
class QDESIGNER_WIDGET_EXPORT CppSyntaxHighlighter : public QSyntaxHighlighter
{
public:
	void loadColorSchemeFromSettings( QSettings* );
	CppSyntaxHighlighter( QTextDocument* );
	virtual void highlightBlock( const QString& );
	//	
	enum KeywordType
	{
		NormalKeyword,
		TypeKeyword
	};
	struct Keyword
	{
		const char* name;
		KeywordType type;
	};
	static const Keyword keywords[];
	//
private:
	void highlightKeyword( int, const QString& );
	//
	QTextCharFormat numberFormat;
	QTextCharFormat stringFormat;
	QTextCharFormat typeFormat;
	QTextCharFormat keywordFormat;
	QTextCharFormat labelFormat;
	QTextCharFormat commentFormat;
	QTextCharFormat preProcessorFormat;
};
//
#endif // CPPSYNTAXHIGHLIGHTER_H
