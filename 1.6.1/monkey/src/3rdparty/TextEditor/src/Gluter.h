/********************************************************************************************************
		* PROGRAM      : TextEditor
		* DATE - TIME  : lundi 01 mai 2006 - 10h24
		* AUTHOR       : Azevedo Filipe aka Nox PasNox ( pasnox at gmail dot com )
			* FILENAME     : Gluter.h
			* LICENSE      : GPL
			* COMMENTARY   :
********************************************************************************************************/
#ifndef GLUTER_H
#define GLUTER_H
//
#include <QWidget>
#include <QDesignerExportWidget>
//
class QTextEdit;
//
#ifndef TEXTEDITORPLUGIN
#undef QDESIGNER_WIDGET_EXPORT
#define QDESIGNER_WIDGET_EXPORT
#endif
//
class QDESIGNER_WIDGET_EXPORT Gluter : public QWidget
{
	Q_OBJECT
			Q_PROPERTY( int digitNumbers READ digitNumbers WRITE setDigitNumbers )
			Q_PROPERTY( QColor textColor READ textColor WRITE setTextColor )
			Q_PROPERTY( QColor backgroundColor READ backgroundColor WRITE setBackgroundColor )
			//
public:
	Gluter( QTextEdit* );
	//
	void setDigitNumbers( int );
	int digitNumbers() const;
	//
	void setTextColor( const QColor& );
	const QColor& textColor() const;
	//
	void setBackgroundColor( const QColor& );
	const QColor& backgroundColor() const;
	
	// xiantia
	QList <int> breakpointList;
	int currentDebugerAt;

	//
protected:
	virtual void paintEvent( QPaintEvent* );
	//
private:
	QTextEdit* mEdit;
	int mDigitNumbers;
	QColor mTextColor;
	QColor mBackgroundColor;
	
	//
signals:
	void digitNumbersChanged( int );
	void textColorChanged( const QColor& );
	void backgroundColorChanged( const QColor& );
	//
public slots:
	void setDefaultProperties();
	void mousePressedEvent( QMouseEvent *e);
};
//
#endif // GLUTER_H

