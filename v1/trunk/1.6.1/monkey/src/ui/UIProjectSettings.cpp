/********************************************************************************************************
 * PROGRAM      : qt4ds
 * DATE - TIME  : vendredi 27 janvier 2006 - 02h22
 * AUTHOR       : Nox P@sNox ( pasnox@hotmail.com )
 * FILENAME     : UIProjectSettings.cpp
 * LICENSE      : GPL
 * COMMENTARY   : 
 ********************************************************************************************************/
#include "UIProjectSettings.h"
#include "main.h"
#include "Options.h"
#include "Tools.h"
//
#include <QFileDialog>
#include <QInputDialog>
//
QPointer<UIProjectSettings> UIProjectSettings::_self = 0L;
//
UIProjectSettings* UIProjectSettings::self( QMakeProject* project, QWidget* parent )
{
	if ( !_self )
		_self = new UIProjectSettings( project, parent );
	return _self;
}
//
QStringList UIProjectSettings::parseString( const QString& value )
{
	QString values = value;
	bool libs = cbSetting->currentText().toLower() == "libs";
	QStringList list;
	int i;
	while ( !values.isEmpty() )
	{
		if ( ( i = values.indexOf( "\"" ) ) != -1 )
		{
			list << values.left( i -( libs ? 3 : 0 ) ).split( " ", QString::SkipEmptyParts );
			values = values.mid( i +1 -( libs ? 3 : 0 ) );
			if ( ( i = values.indexOf( "\"", libs ? 3 : 0 ) ) == -1 )
			{
				QMessageBox::warning( this, _WARNING, "Can't parse the data, abording." );
				break;
			}
			list << values.left( i +( libs ? 1 : 0 ) );
			values = values.mid( i +1 );
		}
		else
		{
			list << values.split( " ", QString::SkipEmptyParts );
			values.clear();
		}
	}
	return list;
}
//
QString UIProjectSettings::getText( QWidget* parent, const QString& caption, const QString& msg, bool* ok, const QString& value )
{
	return QInputDialog::getText( parent, caption, msg, QLineEdit::Normal, value, *&ok );
}
//
QString UIProjectSettings::getExistingDirectory( QWidget* parent, const QString& caption, const QString& directory )
{
	return QFileDialog::getExistingDirectory( parent, caption, directory );
}
//
QString UIProjectSettings::getOpenFileName( QWidget* parent, const QString& caption, const QString& filePath, const QString& filter )
{
	return QFileDialog::getOpenFileName( parent, caption, filePath, filter );
}
//
UIProjectSettings::UIProjectSettings( QMakeProject* project, QWidget* parent )
	: QDialog( parent ), mProject( project )
{
	setupUi( this );
	setAttribute( Qt::WA_DeleteOnClose );
	setFixedSize( size() );
	setWindowTitle( QString( "Project Settings - %1" ).arg( mProject->name() ) );
	//
	stringValues = mProject->cStringValues();
	QStringList tmpStringList;
	QString tmpString;
	QCheckBox* tmpCheckBox;
	// TEMPLATE
	cbTemplate->setCurrentIndex( cbTemplate->findText( mProject->stringValue( "TEMPLATE" ) ) );
	if ( cbTemplate->currentIndex() == -1 )
		cbTemplate->setCurrentIndex( 0 );
	// LANGUAGE
	cbLanguage->setCurrentIndex( cbLanguage->findText( mProject->stringValue( "LANGUAGE" ) ) );
	if ( cbLanguage->currentIndex() == -1 )
		cbLanguage->setCurrentIndex( 0 );
	// Qt Configuration
	// CONFIG
	tmpString = mProject->stringValue( "CONFIG" );
	tmpStringList = tmpString.split( " ", QString::SkipEmptyParts );
	if ( tmpString.contains( "qt", Qt::CaseInsensitive ) )
		gbQtModules->setChecked( true );
	foreach ( QString value, tmpStringList )
		if ( ( tmpCheckBox = pConfiguration->findChild<QCheckBox*>( "cb_" + value ) ) && tmpCheckBox->isEnabled() )
			tmpCheckBox->setChecked( true );
	// QT
	tmpString = mProject->stringValue( "QT" );
	tmpStringList = tmpString.split( " ", QString::SkipEmptyParts );
	foreach ( QString value, tmpStringList )
		if ( ( tmpCheckBox = pConfiguration->findChild<QCheckBox*>( "cb_qt_" + value ) ) && tmpCheckBox->isEnabled() )
			tmpCheckBox->setChecked( true );
	// WS Specific Configuration
	on_cbSystem_currentIndexChanged( "(all)" );
	// TRANSLATIONS
	QListWidgetItem* item;
	QStringList translations = mProject->stringListValue( "TRANSLATIONS" );
	foreach ( QString language, Options::languages() )
	{
		item = new QListWidgetItem( language, lwTranslations );
		item->setCheckState( Qt::Unchecked );
		if ( translations.contains( QString( "translations/%1_%2.ts" ).arg( mProject->name(), language.toLower() ) ) )
			item->setCheckState( Qt::Checked );	
	}
}
//
void UIProjectSettings::on_cbSystem_currentIndexChanged( const QString& system )
{
	// TARGET, DESTDIR, VERSION
	leTarget->setText( stringValues.value( "TARGET" ).value( system ) );
	leDestination->setText( stringValues.value( "DESTDIR" ).value( system ) );
	leVersion->setText( stringValues.value( "VERSION" ).value( system ) );
	// SETTING
	on_cbSetting_currentIndexChanged( cbSetting->currentText() );
}
//
void UIProjectSettings::on_cbSystem_highlighted( int id )
{
	if ( cbSystem->currentIndex() != id )
	{
		// TARGET, DESTDIR, VERSION
		stringValues[ "TARGET" ][ cbSystem->currentText() ] = leTarget->text();
		stringValues[ "DESTDIR" ][ cbSystem->currentText() ] = leDestination->text();
		stringValues[ "VERSION" ][ cbSystem->currentText() ] = leVersion->text();
		// SETTING
		on_cbSetting_highlighted( -1 );
	}
}
//
void UIProjectSettings::on_tbDestination_clicked()
{
	QString from = mProject->absolutePath().append( "/" ).append( leDestination->text() );
	QString value = QFileDialog::getExistingDirectory( this, tr( "Choose the destination path" ), from );
	if ( !value.isNull() )
		leDestination->setText( mProject->relative( value ) );
}
//
void UIProjectSettings::on_cbSetting_currentIndexChanged( const QString& setting )
{
	lwSettings->clear();
	lwSettings->addItems( parseString( stringValues.value( setting ).value( cbSystem->currentText() ) ) );
}
//
void UIProjectSettings::on_cbSetting_highlighted( int id )
{
	if ( cbSetting->currentIndex() != id )
	{
		QStringList values;
		QString value;
		for ( int i = 0; i < lwSettings->count(); i++ )
		{
			value = lwSettings->item( i )->text();
			if ( value.contains( " " ) && !value.contains( "\"" ) )
				value.prepend( "\"" ).append( "\"" );
			values << value;
		}
		stringValues[ cbSetting->currentText() ][ cbSystem->currentText() ] = values.join( " " );
	}
}
//
void UIProjectSettings::on_pbAdd_clicked()
{
	QString value, filter;
	bool ok = false;
	switch( cbSetting->currentIndex() )
	{
	case 0: // LIBS
		value = getText( this, tr( "Add a library" ), tr( "Type your library name or path\n(like: -lLIB or -L\"PATH\")" ), &ok );
		break;
	case 1: // DEFINES
		value = getText( this, tr( "Add a define" ), tr( "Type your define" ), &ok );
		break;
	case 2: // INCLUDEPATH
	case 3: // DEPENDPATH
	case 4: // VPATH
		value = getExistingDirectory( this, QString( tr( "Choose your new %1 directory" ) ).arg( cbSetting->currentText() ), mProject->absolutePath() );
		if ( !value.isNull() )
		{
			ok = true;
			value = mProject->relative( value );
		}
		break;
	case 5: // RESOURCES
		if ( filter.isNull() )
			filter = QString( tr( "Qt Resources (%1);;") ).arg( QString( "*." + QMakeProject::resourceExtension.join( " *" ).replace( "*", "*." ) ) );
	case 6: // DEF_FILE
		if ( filter.isNull() )
			filter = QString( tr( "Def Files (%1);;") ).arg( QString( "*." + QMakeProject::defExtension.join( " *" ).replace( "*", "*." ) ) );
	case 7: // RC_FILE
		if ( filter.isNull() )
			filter = QString( tr( "Rc Files (%1);;") ).arg( QString( "*." + QMakeProject::rcExtension.join( " *" ).replace( "*", "*." ) ) );
	case 8: // RES_FILE
		if ( filter.isNull() )
			filter = QString( tr( "Res Files (%1);;") ).arg( QString( "*." + QMakeProject::resExtension.join( " *" ).replace( "*", "*." ) ) );
		filter += tr( "All Files (*)" );
		value = getOpenFileName( this, QString( tr( "Choose your new %1 file" ) ).arg( cbSetting->currentText() ), mProject->absolutePath(), filter );
		if ( !value.isNull() )
		{
			ok = true;
			value = mProject->relative( value );
		}
		break;
	}
	if ( ok && !value.isEmpty() )
		lwSettings->addItem( value );
}
//
void UIProjectSettings::on_pbModify_clicked()
{
	if ( !lwSettings->currentItem() )
		return;
	QString value, filter, currentValue = lwSettings->currentItem()->text();
	bool ok = false;
	switch( cbSetting->currentIndex() )
	{
	case 0: // LIBS
		value = getText( this, tr( "Modify a library" ), tr( "Type your library name or path\n(like: -lLIB or -L\"PATH\")" ), &ok, currentValue );
		break;
	case 1: // DEFINES
		value = getText( this, tr( "Modify a define" ), tr( "Type your define" ), &ok, currentValue );
		break;
	case 2: // INCLUDEPATH
	case 3: // DEPENDPATH
	case 4: // VPATH
		value = getExistingDirectory( this, QString( tr( "Choose your new %1 directory" ) ).arg( cbSetting->currentText() ), mProject->absolutePath().append( "/" ).append( currentValue ) );
		if ( !value.isNull() )
		{
			ok = true;
			value = mProject->relative( value );
		}
		break;
	case 5: // RESOURCES
		if ( filter.isNull() )
			filter = QString( tr( "Qt Resources (%1);;") ).arg( QString( "*." + QMakeProject::resourceExtension.join( " *" ).replace( "*", "*." ) ) );
	case 6: // DEF_FILE
		if ( filter.isNull() )
			filter = QString( tr( "Def Files (%1);;") ).arg( QString( "*." + QMakeProject::defExtension.join( " *" ).replace( "*", "*." ) ) );
	case 7: // RC_FILE
		if ( filter.isNull() )
			filter = QString( tr( "Rc Files (%1);;") ).arg( QString( "*." + QMakeProject::rcExtension.join( " *" ).replace( "*", "*." ) ) );
	case 8: // RES_FILE
		if ( filter.isNull() )
			filter = QString( tr( "Res Files (%1);;") ).arg( QString( "*." + QMakeProject::resExtension.join( " *" ).replace( "*", "*." ) ) );
		filter += tr( "All Files (*)" );
		value = getOpenFileName( this, QString( tr( "Choose your new %1 file" ) ).arg( cbSetting->currentText() ), mProject->absolutePath().append( "/" ).append( currentValue ), filter );
		if ( !value.isNull() )
		{
			ok = true;
			value = mProject->relative( value );
		}
		break;
	}
	if ( ok && !value.isEmpty() )
		lwSettings->currentItem()->setText( value );
}
//
void UIProjectSettings::on_pbDelete_clicked()
{
	if ( lwSettings->currentItem() )
		delete lwSettings->currentItem();
}
//
void UIProjectSettings::on_pbClear_clicked()
{
	lwSettings->clear();
}
//
void UIProjectSettings::accept()
{
	const QStringList platforms = QStringList() << "(all)" << "win32" << "unix" << "mac";
	QStringList tmpStringList;
	QString tmpString;
	// TEMPLATE
	stringValues[ "TEMPLATE" ][ "(all)" ] = cbTemplate->currentText();
	// LANGUAGE
	stringValues[ "LANGUAGE" ][ "(all)" ] = cbLanguage->currentText();
	// Qt Configuration
	// CONFIG & QT
	stringValues[ "CONFIG" ][ "(all)" ].clear();
	stringValues[ "QT" ][ "(all)" ].clear();
	QList<QCheckBox*> boxes = pConfiguration->findChildren<QCheckBox*>();
	if ( gbQtModules->isChecked() )
		stringValues[ "CONFIG" ][ "(all)" ] = "qt";
	foreach ( QCheckBox* box, boxes )
	{
		if ( !box->isChecked() )
			continue;
		tmpString = box->objectName().mid( 3 );
		if ( tmpString.contains( "qt_" ) && gbQtModules->isChecked() )
			stringValues[ "QT" ][ "(all)" ].append( ( !stringValues[ "QT" ][ "(all)" ].isEmpty() ? " " : "" ) + tmpString.mid( 3 ) );
		else
			stringValues[ "CONFIG" ][ "(all)" ].append( ( !stringValues[ "CONFIG" ][ "(all)" ].isEmpty() ? " " : "" ) + tmpString );
	}
	// SETTINGS
	on_cbSystem_highlighted( -1 );
	mProject->setStringValues( stringValues );
	// TRANSLATIONS
	for ( int i = 0; i < lwTranslations->count(); i++ )
	{
		if ( lwTranslations->item( i )->checkState() == Qt::Checked )
			mProject->addStringListValue( "TRANSLATIONS", "(all)", QString( "translations/%1_%2.ts" ).arg( mProject->name(), Options::languages().at( i ).toLower() ) );
		else
			mProject->removeStringListValue( "TRANSLATIONS", "(all)", QString( "translations/%1_%2.ts" ).arg( mProject->name(), Options::languages().at( i ).toLower() ) );
	}	
	//
	mProject->setModified( true );
	QDialog::accept();
}
