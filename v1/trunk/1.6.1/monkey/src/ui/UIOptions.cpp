/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : lundi 30 janvier 2006 - 17h47
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UIOptions.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "UIOptions.h"
#include "Options.h"
//
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>
//
QPointer<UIOptions> UIOptions::_self = 0L;
//
UIOptions* UIOptions::self( QWidget* parent )
{
	if ( !_self )
		_self = new UIOptions( parent );
	return _self;
}
//
UIOptions::UIOptions( QWidget* parent )
	: QDialog( parent )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
	//
	loadOptions();
	// Make Connections
	QList<QToolButton*> tbs = tbOptions->findChildren<QToolButton*>( QRegExp( "tb*" ) );
	foreach ( QToolButton* tb, tbs )
		if ( tb != tbPath )
			connect( tb, SIGNAL( clicked() ), this, SLOT( tbText_clicked() ) );
}
//
void UIOptions::loadOptions()
{
	if ( Options::self()->value( "Options/FirstTimeSetting", true ).toBool() )
		Options::setDefaultOptions();
	//
	QFont myFont;
	QPalette myPalette;
	QColor myColor;
	QString myFontFamily = qApp->font().toString();
	QStringList myList;
	//	General
	// Project
	cbReloadLastProject->setChecked( Options::self()->value( "Options/General/ReloadLastProject" ).toBool() );
	cbReloadLastProjectFiles->setChecked( Options::self()->value( "Options/General/ReloadLastProjectFiles" ).toBool() );
	cbSaveBeforeBuilding->setChecked( Options::self()->value( "Options/General/SaveBeforeBuilding" ).toBool() );
	cbShowHeaderInformations->setChecked( Options::self()->value( "Options/General/ShowHeaderInformations" ).toBool() );
	cbCreateBackup->setChecked( Options::self()->value( "Options/General/CreateBackup" ).toBool() );
	// Path
	cbPath->setItemData( 0, Options::self()->value( "Options/General/MingwPath" ).toString() );
	cbPath->setItemData( 1, Options::self()->value( "Options/General/QtPath" ).toString() );
	cbPath->setItemData( 2, Options::self()->value( "Options/General/TranslationsPath" ).toString() );
	cbPath->setItemData( 3, Options::self()->value( "Options/General/DocumentationsPath" ).toString() );
	cbPath->setItemData( 4, Options::self()->value( "Options/General/TemplatesPath" ).toString() );
	cbPath->setItemData( 5, Options::self()->value( "Options/General/PdfViewer" ).toString() );
	cbPath->setItemData( 6, Options::self()->value( "Options/General/WebBrowser" ).toString() );
	on_cbPath_currentIndexChanged( 0 );
	// Others
	cbShowQtWarnings->setChecked( Options::self()->value( "Options/General/ShowQtWarnings" ).toBool() );
	//	Editor
	// Editor
	if ( !myFont.fromString( Options::self()->value( "Options/Editor/TextFont" ).toString() ) )
		myFont = qApp->font();
	leTextFontPreview->setFont( myFont );
	leTextFontPreview->setText( myFont.family() );
	lTextForegroundColorPreview->setText( Options::self()->value( "Options/Editor/TextForegroundColor" ).toString() );
	lTextBackgroundColorPreview->setText( Options::self()->value( "Options/Editor/TextBackgroundColor" ).toString() );
	lTextSelectionColorPreview->setText( Options::self()->value( "Options/Editor/TextSelectionColor" ).toString() );
	sbTextTabSpacing->setValue( Options::self()->value( "Options/Editor/TextTabSpacing" ).toInt() );
	sbTextScrollLines->setValue( Options::self()->value( "Options/Editor/TextScrollLines" ).toInt() );
	sbLimitLine->setValue( Options::self()->value( "Options/Editor/LimitLine" ).toInt() );
	lLimitLineColorPreview->setText( Options::self()->value( "Options/Editor/LimitLineColor" ).toString() );
	lCurrentLineColorPreview->setText( Options::self()->value( "Options/Editor/CurrentLineColor" ).toString() );
	cbSourceShowingMethod->setCurrentIndex( Options::self()->value( "Options/Editor/SourceShowingMethod" ).toInt() );
	cbSourceIndentationMethod->setCurrentIndex( Options::self()->value( "Options/Editor/SourceIndentationMethod" ).toInt() );
	cbPrintingColor->setChecked( Options::self()->value( "Options/Editor/PrintingColor" ).toBool() );
	cbPrintingLineNumbers->setChecked( Options::self()->value( "Options/Editor/PrintingLineNumbers" ).toBool() );
	cbHighlightCurrentLine->setChecked( Options::self()->value( "Options/Editor/HighlightCurrentLine" ).toBool() );
	cbActivateAutoCompletion->setChecked( Options::self()->value( "Options/Editor/activateAutoCompletion" ).toBool() );
	cbActivateParentheseMatcher->setChecked( Options::self()->value( "Options/Editor/activateParentheseMatcher" ).toBool() );
	//	Color Scheme
	myFont = qApp->font();
	//
	myList = Options::self()->value( "Options/Editor/Number" ).toStringList();
	myFont.fromString( myList.at( 0 ) );
	lColorSchemePreview1->setFont( myFont );
	lColorSchemePreview1->setText( myFont.family() );
	myPalette = lColorSchemePreview1->palette();
	myColor = QColor( myList.at( 1 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::Button, myColor );
	myColor = QColor( myList.at( 2 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::ButtonText, myColor );
	lColorSchemePreview1->setPalette( myPalette );
	//
	myList = Options::self()->value( "Options/Editor/String" ).toStringList();
	myFont.fromString( myList.at( 0 ) );
	lColorSchemePreview2->setFont( myFont );
	lColorSchemePreview2->setText( myFont.family() );
	myPalette = lColorSchemePreview2->palette();
	myColor = QColor( myList.at( 1 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::Button, myColor );
	myColor = QColor( myList.at( 2 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::ButtonText, myColor );
	lColorSchemePreview2->setPalette( myPalette );
	//
	myList = Options::self()->value( "Options/Editor/Type" ).toStringList();
	myFont.fromString( myList.at( 0 ) );
	lColorSchemePreview3->setFont( myFont );
	lColorSchemePreview3->setText( myFont.family() );
	myPalette = lColorSchemePreview3->palette();
	myColor = QColor( myList.at( 1 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::Button, myColor );
	myColor = QColor( myList.at( 2 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::ButtonText, myColor );
	lColorSchemePreview3->setPalette( myPalette );
	//
	myList = Options::self()->value( "Options/Editor/Keyword" ).toStringList();
	myFont.fromString( myList.at( 0 ) );
	lColorSchemePreview4->setFont( myFont );
	lColorSchemePreview4->setText( myFont.family() );
	myPalette = lColorSchemePreview4->palette();
	myColor = QColor( myList.at( 1 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::Button, myColor );
	myColor = QColor( myList.at( 2 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::ButtonText, myColor );
	lColorSchemePreview4->setPalette( myPalette );
	//
	myList = Options::self()->value( "Options/Editor/Label" ).toStringList();
	myFont.fromString( myList.at( 0 ) );
	lColorSchemePreview5->setFont( myFont );
	lColorSchemePreview5->setText( myFont.family() );
	myPalette = lColorSchemePreview5->palette();
	myColor = QColor( myList.at( 1 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::Button, myColor );
	myColor = QColor( myList.at( 2 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::ButtonText, myColor );
	lColorSchemePreview5->setPalette( myPalette );
	//
	myList = Options::self()->value( "Options/Editor/Comment" ).toStringList();
	myFont.fromString( myList.at( 0 ) );
	lColorSchemePreview6->setFont( myFont );
	lColorSchemePreview6->setText( myFont.family() );
	myPalette = lColorSchemePreview6->palette();
	myColor = QColor( myList.at( 1 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::Button, myColor );
	myColor = QColor( myList.at( 2 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::ButtonText, myColor );
	lColorSchemePreview6->setPalette( myPalette );
	//
	myList = Options::self()->value( "Options/Editor/Preprocessor" ).toStringList();
	myFont.fromString( myList.at( 0 ) );
	lColorSchemePreview7->setFont( myFont );
	lColorSchemePreview7->setText( myFont.family() );
	myPalette = lColorSchemePreview7->palette();
	myColor = QColor( myList.at( 1 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::Button, myColor );
	myColor = QColor( myList.at( 2 ) );
	if ( myColor.isValid() )
		myPalette.setColor( QPalette::ButtonText, myColor );
	lColorSchemePreview7->setPalette( myPalette );
	//
	colorize();
}
//
void UIOptions::saveOptions()
{
	QFont myFont;
	QPalette myPalette;
	QColor myColor;
	QString myFontFamily = qApp->font().toString();
	QStringList myList;
	// 	First Time Setting
	Options::self()->setValue( "Options/FirstTimeSetting", false );
	//	General
	// Project
	Options::self()->setValue( "Options/General/ReloadLastProject", cbReloadLastProject->isChecked() );
	Options::self()->setValue( "Options/General/ReloadLastProjectFiles", cbReloadLastProjectFiles->isChecked() );
	Options::self()->setValue( "Options/General/SaveBeforeBuilding", cbSaveBeforeBuilding->isChecked() );
	Options::self()->setValue( "Options/General/ShowHeaderInformations", cbShowHeaderInformations->isChecked() );
	Options::self()->setValue( "Options/General/CreateBackup", cbCreateBackup->isChecked() );
	// Path
	Options::self()->setValue( "Options/General/MingwPath", cbPath->itemData( 0 ).toString() );
	Options::self()->setValue( "Options/General/QtPath", cbPath->itemData( 1 ).toString() );
	Options::self()->setValue( "Options/General/TranslationsPath", cbPath->itemData( 2 ).toString() );
	Options::self()->setValue( "Options/General/DocumentationsPath", cbPath->itemData( 3 ).toString() );
	Options::self()->setValue( "Options/General/TemplatesPath", cbPath->itemData( 4 ).toString() );
	Options::self()->setValue( "Options/General/PdfViewer", cbPath->itemData( 5 ).toString() );
	Options::self()->setValue( "Options/General/WebBrowser", cbPath->itemData( 6 ).toString() );
	// Others
	Options::self()->setValue( "Options/General/ShowQtWarnings", cbShowQtWarnings->isChecked() );
	//	Editor
	// Editor
	Options::self()->setValue( "Options/Editor/TextFont", leTextFontPreview->font().toString() );
	Options::self()->setValue( "Options/Editor/TextForegroundColor", lTextForegroundColorPreview->text() );
	Options::self()->setValue( "Options/Editor/TextBackgroundColor", lTextBackgroundColorPreview->text() );
	Options::self()->setValue( "Options/Editor/TextSelectionColor", lTextSelectionColorPreview->text() );
	Options::self()->setValue( "Options/Editor/TextTabSpacing", sbTextTabSpacing->value() );
	Options::self()->setValue( "Options/Editor/TextScrollLines", sbTextScrollLines->value() );
	Options::self()->setValue( "Options/Editor/LimitLine", sbLimitLine->value() );
	Options::self()->setValue( "Options/Editor/LimitLineColor", lLimitLineColorPreview->text() );
	Options::self()->setValue( "Options/Editor/CurrentLineColor", lCurrentLineColorPreview->text() );
	Options::self()->setValue( "Options/Editor/SourceShowingMethod", cbSourceShowingMethod->currentIndex() );
	Options::self()->setValue( "Options/Editor/SourceIndentationMethod", cbSourceIndentationMethod->currentIndex() );
	Options::self()->setValue( "Options/Editor/PrintingColor", cbPrintingColor->isChecked() );
	Options::self()->setValue( "Options/Editor/PrintingLineNumbers", cbPrintingLineNumbers->isChecked() );
	Options::self()->setValue( "Options/Editor/HighlightCurrentLine", cbHighlightCurrentLine->isChecked() );
	Options::self()->setValue( "Options/Editor/ActivateAutoCompletion", cbActivateAutoCompletion->isChecked() );
	Options::self()->setValue( "Options/Editor/ActivateParentheseMatcher", cbActivateParentheseMatcher->isChecked() );
	//	ColorScheme
	//
	myList = QStringList() << lColorSchemePreview1->font().toString() << lColorSchemePreview1->palette().color( QPalette::Button ).name() << lColorSchemePreview1->palette().color( QPalette::ButtonText ).name();
	Options::self()->setValue( "Options/Editor/Number", myList );
	myList = QStringList() << lColorSchemePreview2->font().toString() << lColorSchemePreview2->palette().color( QPalette::Button ).name() << lColorSchemePreview2->palette().color( QPalette::ButtonText ).name();
	Options::self()->setValue( "Options/Editor/String", myList );
	myList = QStringList() << lColorSchemePreview3->font().toString() << lColorSchemePreview3->palette().color( QPalette::Button ).name() << lColorSchemePreview3->palette().color( QPalette::ButtonText ).name();
	Options::self()->setValue( "Options/Editor/Type", myList );
	myList = QStringList() << lColorSchemePreview4->font().toString() << lColorSchemePreview4->palette().color( QPalette::Button ).name() << lColorSchemePreview4->palette().color( QPalette::ButtonText ).name();
	Options::self()->setValue( "Options/Editor/Keyword", myList );
	myList = QStringList() << lColorSchemePreview5->font().toString() << lColorSchemePreview5->palette().color( QPalette::Button ).name() << lColorSchemePreview5->palette().color( QPalette::ButtonText ).name();
	Options::self()->setValue( "Options/Editor/Label", myList );
	myList = QStringList() << lColorSchemePreview6->font().toString() << lColorSchemePreview6->palette().color( QPalette::Button ).name() << lColorSchemePreview6->palette().color( QPalette::ButtonText ).name();
	Options::self()->setValue( "Options/Editor/Comment", myList );
	myList = QStringList() << lColorSchemePreview7->font().toString() << lColorSchemePreview7->palette().color( QPalette::Button ).name() << lColorSchemePreview7->palette().color( QPalette::ButtonText ).name();
	Options::self()->setValue( "Options/Editor/Preprocessor", myList );
}
//
void UIOptions::on_cbPath_currentIndexChanged( int id )
{
	lePath->setText( cbPath->itemData( id ).toString() );
	lePath->setFocus();
}
//
void UIOptions::on_tbPath_clicked()
{
	QString pathName;
	switch ( cbPath->currentIndex() )
	{
	case 0: // mingwPath
		{
			pathName = QFileDialog::getExistingDirectory( this, tr( "Choose the mingw path" ), lePath->text() );
			if ( !pathName.isEmpty() )
				lePath->setText( pathName );
			break;
		}
	case 1: // qtPath
		{
			pathName = QFileDialog::getExistingDirectory( this, tr( "Choose the Qt 4 path" ), lePath->text() );
			if ( !pathName.isEmpty() )
				lePath->setText( pathName );
			break;
		}
	case 2: // translationsPath
		{
			pathName = QFileDialog::getExistingDirectory( this, tr( "Choose the translations path" ), lePath->text() );
			if ( !pathName.isEmpty() )
				lePath->setText( pathName );
			break;
		}
	case 3: // documentationsPath
		{
			pathName = QFileDialog::getExistingDirectory( this, tr( "Choose the documentation path" ), lePath->text() );
			if ( !pathName.isEmpty() )
				lePath->setText( pathName );
			break;
		}
	case 4: // templatesPath
		{
			pathName = QFileDialog::getExistingDirectory( this, tr( "Choose the templates path" ), lePath->text() );
			if ( !pathName.isEmpty() )
				lePath->setText( pathName );
			break;
		}
	case 5: // pdfViewer
		{
			pathName = QFileDialog::getOpenFileName( this, tr( "Choose the executable for the PDF Viewer" ), lePath->text() );
			if ( !pathName.isEmpty() )
				lePath->setText( pathName );
			break;
		}
	case 6: // webBrowser
		{
			pathName = QFileDialog::getOpenFileName( this, tr( "Choose the executable for the web browser" ), lePath->text() );
			if ( !pathName.isEmpty() )
				lePath->setText( pathName );
			break;
		}
	}
	lePath->setFocus();
}
//
void UIOptions::on_lePath_editingFinished()
{
	cbPath->setItemData( cbPath->currentIndex(), lePath->text() );
}
//
void UIOptions::tbText_clicked()
{
	QToolButton* tb = qobject_cast<QToolButton*>( sender() );
	if ( !tb )
		return;
	//
	bool ok;
	QString text;
	QColor color;
	//
	if ( tb == tbTextFont )
	{
		QFont font = QFont( leTextFontPreview->font() );
		font = QFontDialog::getFont( &ok, font, this );
		if ( ok )
		{
			leTextFontPreview->setFont( font );
			leTextFontPreview->setText( font.family() );
		}
	}
	else if ( tb == tbTextForegroundColor )
	{
		color = QColorDialog::getColor( QColor( lTextForegroundColorPreview->text() ), this );
		if ( color.isValid() )
		{
			lTextForegroundColorPreview->setText( color.name() );
			lTextForegroundColorPreview->setPalette( QPalette( color ) );
		}
	}
	else if ( tb == tbTextSelectionColor )
	{
		color = QColorDialog::getColor( QColor( lTextSelectionColorPreview->text() ), this );
		if ( color.isValid() )
		{
			lTextSelectionColorPreview->setText( color.name() );
			lTextSelectionColorPreview->setPalette( QPalette( color ) );
		}
	}
	else if ( tb == tbTextBackgroundColor )
	{
		color = QColorDialog::getColor( QColor( lTextBackgroundColorPreview->text() ), this );
		if ( color.isValid() )
		{
			lTextBackgroundColorPreview->setText( color.name() );
			lTextBackgroundColorPreview->setPalette( QPalette( color ) );
		}
	}
	else if ( tb == tbLimitLineColor )
	{
		color = QColorDialog::getColor( QColor( lLimitLineColorPreview->text() ), this );
		if ( color.isValid() )
		{
			lLimitLineColorPreview->setText( color.name() );
			lLimitLineColorPreview->setPalette( QPalette( color ) );
		}
	}
	else if ( tb == tbCurrentLineColor )
	{
		color = QColorDialog::getColor( QColor( lCurrentLineColorPreview->text() ), this );
		if ( color.isValid() )
		{
			lCurrentLineColorPreview->setText( color.name() );
			lCurrentLineColorPreview->setPalette( QPalette( color ) );
		}
	}
	else if ( tb == tbColorSchemeFont1 )
	{
		QFont font = QFont( lColorSchemePreview1->font() );
		font = QFontDialog::getFont( &ok, font, this );
		if ( ok )
		{
			lColorSchemePreview1->setFont( font );
			lColorSchemePreview1->setText( font.family() );
		}
	}
	else if ( tb == tbColorSchemeFont2 )
	{
		QFont font = QFont( lColorSchemePreview2->font() );
		font = QFontDialog::getFont( &ok, font, this );
		if ( ok )
		{
			lColorSchemePreview2->setFont( font );
			lColorSchemePreview2->setText( font.family() );
		}
	}
	else if ( tb == tbColorSchemeFont3 )
	{
		QFont font = QFont( lColorSchemePreview3->font() );
		font = QFontDialog::getFont( &ok, font, this );
		if ( ok )
		{
			lColorSchemePreview3->setFont( font );
			lColorSchemePreview3->setText( font.family() );
		}
	}
	else if ( tb == tbColorSchemeFont4 )
	{
		QFont font = QFont( lColorSchemePreview4->font() );
		font = QFontDialog::getFont( &ok, font, this );
		if ( ok )
		{
			lColorSchemePreview4->setFont( font );
			lColorSchemePreview4->setText( font.family() );
		}
	}
	else if ( tb == tbColorSchemeFont5 )
	{
		QFont font = QFont( lColorSchemePreview5->font() );
		font = QFontDialog::getFont( &ok, font, this );
		if ( ok )
		{
			lColorSchemePreview5->setFont( font );
			lColorSchemePreview5->setText( font.family() );
		}
	}
	else if ( tb == tbColorSchemeFont6 )
	{
		QFont font = QFont( lColorSchemePreview6->font() );
		font = QFontDialog::getFont( &ok, font, this );
		if ( ok )
		{
			lColorSchemePreview6->setFont( font );
			lColorSchemePreview6->setText( font.family() );
		}
	}
	else if ( tb == tbColorSchemeFont7 )
	{
		QFont font = QFont( lColorSchemePreview7->font() );
		font = QFontDialog::getFont( &ok, font, this );
		if ( ok )
		{
			lColorSchemePreview7->setFont( font );
			lColorSchemePreview7->setText( font.family() );
		}
	}
	else if ( tb == tbColorSchemeForeground1 )
	{
		color = QColorDialog::getColor( lColorSchemePreview1->palette().color( QPalette::ButtonText ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview1->palette();
			pal.setColor( QPalette::ButtonText, color );
			lColorSchemePreview1->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeForeground2 )
	{
		color = QColorDialog::getColor( lColorSchemePreview2->palette().color( QPalette::ButtonText ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview2->palette();
			pal.setColor( QPalette::ButtonText, color );
			lColorSchemePreview2->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeForeground3 )
	{
		color = QColorDialog::getColor( lColorSchemePreview3->palette().color( QPalette::ButtonText ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview3->palette();
			pal.setColor( QPalette::ButtonText, color );
			lColorSchemePreview3->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeForeground4 )
	{
		color = QColorDialog::getColor( lColorSchemePreview4->palette().color( QPalette::ButtonText ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview4->palette();
			pal.setColor( QPalette::ButtonText, color );
			lColorSchemePreview4->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeForeground5 )
	{
		color = QColorDialog::getColor( lColorSchemePreview5->palette().color( QPalette::ButtonText ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview5->palette();
			pal.setColor( QPalette::ButtonText, color );
			lColorSchemePreview5->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeForeground6 )
	{
		color = QColorDialog::getColor( lColorSchemePreview6->palette().color( QPalette::ButtonText ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview6->palette();
			pal.setColor( QPalette::ButtonText, color );
			lColorSchemePreview6->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeForeground7 )
	{
		color = QColorDialog::getColor( lColorSchemePreview7->palette().color( QPalette::ButtonText ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview7->palette();
			pal.setColor( QPalette::ButtonText, color );
			lColorSchemePreview7->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeBackground1 )
	{
		color = QColorDialog::getColor( lColorSchemePreview1->palette().color( QPalette::Button ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview1->palette();
			pal.setColor( QPalette::Button, color );
			lColorSchemePreview1->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeBackground2 )
	{
		color = QColorDialog::getColor( lColorSchemePreview2->palette().color( QPalette::Button ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview2->palette();
			pal.setColor( QPalette::Button, color );
			lColorSchemePreview2->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeBackground3 )
	{
		color = QColorDialog::getColor( lColorSchemePreview3->palette().color( QPalette::Button ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview3->palette();
			pal.setColor( QPalette::Button, color );
			lColorSchemePreview3->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeBackground4 )
	{
		color = QColorDialog::getColor( lColorSchemePreview4->palette().color( QPalette::Button ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview4->palette();
			pal.setColor( QPalette::Button, color );
			lColorSchemePreview4->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeBackground5 )
	{
		color = QColorDialog::getColor( lColorSchemePreview5->palette().color( QPalette::Button ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview5->palette();
			pal.setColor( QPalette::Button, color );
			lColorSchemePreview5->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeBackground6 )
	{
		color = QColorDialog::getColor( lColorSchemePreview6->palette().color( QPalette::Button ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview6->palette();
			pal.setColor( QPalette::Button, color );
			lColorSchemePreview6->setPalette( pal );
		}
	}
	else if ( tb == tbColorSchemeBackground7 )
	{
		color = QColorDialog::getColor( lColorSchemePreview7->palette().color( QPalette::Button ), this );
		if ( color.isValid() )
		{
			QPalette pal = lColorSchemePreview7->palette();
			pal.setColor( QPalette::Button, color );
			lColorSchemePreview7->setPalette( pal );
		}
	}
}
//
void UIOptions::colorize()
{
	lTextForegroundColorPreview->setPalette( QPalette( QColor( lTextForegroundColorPreview->text() ) ) );
	lTextSelectionColorPreview->setPalette( QPalette( QColor( lTextSelectionColorPreview->text() ) ) );
	lTextBackgroundColorPreview->setPalette( QPalette( QColor( lTextBackgroundColorPreview->text() ) ) );
	lLimitLineColorPreview->setPalette( QPalette( QColor( lLimitLineColorPreview->text() ) ) );
	lCurrentLineColorPreview->setPalette( QPalette( QColor( lCurrentLineColorPreview->text() ) ) );
}
//
void UIOptions::on_pbDefault_clicked()
{
	Options::setDefaultOptions();
	loadOptions();
}
//
void UIOptions::accept()
{
	saveOptions();
	QDialog::accept();
}

