/********************************************************************************************************
		* PROGRAM      : TextEditor
		* DATE - TIME  : lundi 01 mai 2006 - 10h25
		* AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
			* FILENAME     : Gluter.cpp
			* LICENSE      : GPL
			* COMMENTARY   :
********************************************************************************************************/
#include "Gluter.h"
// xiantia , for debuger
#include "src/debuger/UIdebuger.h"
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

	breakpointList.clear();
	currentDebugerAt = -1;
}
//
void Gluter::mousePressedEvent( QMouseEvent *e)
{
	if( e->button() == Qt::RightButton)
		;
}


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
		
		// xiantia , draw breakpoint
		if(breakpointList.indexOf(lineNumber) != -1)
		{
			QRectF target(-1, qRound( position.y() ) - contentsY +1, 16.0, 16.0);
			 QRectF source(0.0, 0.0, 22.0, 22.0);
			 QPixmap pixmap(":/Icons/Icons/fileclose.png");

			// QPainter(this);
 			p.drawPixmap(target, pixmap, source);
/*			p.setBrush(Qt::red);
			p.setPen(Qt::red);
			p.drawEllipse(2,qRound( position.y() ) - contentsY +4,10,10);
			p.setPen(Qt::black);
*/		}
		
		// fleche blue
		if(currentDebugerAt == lineNumber)
		{
	/*		QRectF target(0, qRound( position.y() ) - contentsY +2, 18.0, 15.0);
			 QRectF source(0.0, 0.0, 22.0, 22.0);
			 QPixmap pixmap(":/Icons/Icons/buttonnext.png");
 			p.drawPixmap(target, pixmap, source);
*/			int t=qRound( position.y() ) - contentsY+4 ;
			QPointF fpoint[7] = 
			{
				QPointF(5,0+t),
						QPointF(10,5+t),
						QPointF(5,10+t),
						QPointF(5,8+t),
						QPointF(0,8+t),
						QPointF(0,2+t),
						QPointF(5,2+t)};
				
				p.setBrush(Qt::blue);
				p.setPen(Qt::blue);
				p.drawPolygon (fpoint,7);
				p.setPen(Qt::black);
		}
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
	