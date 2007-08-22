/********************************************************************************************************
 * PROGRAM      : TextEditor
 * DATE - TIME  : samedi 18 mars 2006 - 14h49
 * AUTHOR       : Nox PasNox ( pasnox at hotmail.com )
 * FILENAME     : BlockData.h
 * LICENSE      : GPL
 * COMMENTARY   : This BlockData is based on the QSASyntaxHighlighter's BlockData from trolltech, 
 as it has been modified to suits my needs, it must be considerated as a derivated work.
 ********************************************************************************************************/
#ifndef BLOCKDATA_H
#define BLOCKDATA_H
//
#include <QTextBlock>
//
const int ParenthesisMatcherPropertyId = QTextFormat::UserProperty;
const int ErrorMarkerPropertyId = QTextFormat::UserProperty +1;
//
struct Parenthesis
{
	enum Type
	{
		Open,
		Closed
	};
	inline Parenthesis() 
	: type( Open ), pos( -1 )
	{}
	inline Parenthesis( Type t, QChar c, int position )
	: type( t ), chr( c ), pos( position )
	{}
	Type type;
	QChar chr;
	int pos;
};
//
struct BlockData : public QTextBlockUserData
{
	static BlockData* data( const QTextBlock &block )
	{
		return static_cast<BlockData*>( block.userData() );
	}
	void setToBlock( QTextBlock &block )
	{
		block.setUserData( this );
	}
	//
	inline BlockData()
	: parenthesisMatchStart( -1 ), parenthesisMatchEnd( -1 )
	{}
	//
	QList<Parenthesis> parentheses;
	QTextCharFormat parenthesisMatchingFormat;
	int parenthesisMatchStart;
	int parenthesisMatchEnd;
	QTextCharFormat errorMarkerFormat;
};
//
#endif // BLOCKDATA_H
