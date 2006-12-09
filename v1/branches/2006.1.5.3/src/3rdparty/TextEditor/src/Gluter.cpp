/********************************************************************************************************
 * PROGRAM      : TextEditor
 * DATE - TIME  : lundi 01 mai 2006 - 10h25
 * AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
 * FILENAME     : Gluter.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "Gluter.h"
//
#include <QTextEdit>
#include <QGridLayout>
#include <QScrollBar>
#include <QPainter>
#include <QAbstractTextDocumentLayout>
#include <QTextBlock>
//
Gluter::Gluter( QTextEdit* edit )
	: QWidget( edit ), mEdit( edit )
{
	setAutoFillBackground( true );
	connect( mEdit->document()->documentLayout(), SIGNAL( update( const QRectF& ) ), this, SLOT( update() ) );
	connect( mEdit->verticalScrollBar(), SIGNAL( valueChanged( int ) ), this, SLOT( update() ) );
	setDefaultProperties();
}
//
void Gluter::paintEvent( QPaintEvent* )
{
	int contentsY = mEdit->verticalScrollBar()->value();
	qreal pageBottom = contentsY + mEdit->viewport()->height();
	int lineNumber = 1;
	const QFontMetrics fm = fontMetrics();
	const int ascent = fontMetrics().ascent() +1;
	//
	QPainter p( this );
	// need a hack to only browse the viewed block for big document
	for ( QTextBlock block = mEdit->document()->begin(); block.isValid(); block = block.next(), lineNumber++ )
	{
		QTextLayout* layout = block.layout();
		const QRectF boundingRect = layout->boundingRect();
		QPointF position = layout->position();
		if ( position.y() +boundingRect.height() < contentsY )
			continue;
		if ( position.y() > pageBottom )
			break;
		const QString txt = QString::number( lineNumber );
		p.drawText( width() -fm.width( txt ) -fm.width( "0" ), qRound( position.y() ) -contentsY +ascent, txt ); // -fm.width( "0" ) is an ampty place/indent
	}
}
// PROPERTIES
void Gluter::setDigitNumbers( int i )
{
	if ( i == mDigitNumbers )
		return;
	mDigitNumbers = i;
	setFixedWidth( fontMetrics().width( "0" ) * ( mDigitNumbers +2 ) ); // +2 = 1 empty place before and 1 empty place after
	emit digitNumbersChanged( mDigitNumbers );
}
//
int Gluter::digitNumbers() const
{
	return mDigitNumbers;
}
//
void Gluter::setTextColor( const QColor& c )
{
	if ( c == mTextColor )
		return;
	mTextColor = c;
	QPalette p( palette() );
	p.setColor( foregroundRole(), mTextColor );
	setPalette( p );
	emit textColorChanged( mTextColor );
}
//
const QColor& Gluter::textColor() const
{
	return mTextColor;
}
//
void Gluter::setBackgroundColor( const QColor& c )
{
	if ( c == mBackgroundColor )
		return;
	mBackgroundColor = c;
	QPalette p( palette() );
	p.setColor( backgroundRole(), mBackgroundColor );
	setPalette( p );
	emit backgroundColorChanged( mBackgroundColor );
}
//
const QColor& Gluter::backgroundColor() const
{
	return mBackgroundColor;
}
// END PROPERTIES
void Gluter::setDefaultProperties()
{
	// Default properties
	setBackgroundColor( QColor( "#ffffd2" ) );
	setTextColor( QColor( "#aaaaff" ) );
	setDigitNumbers( 4 );
}
