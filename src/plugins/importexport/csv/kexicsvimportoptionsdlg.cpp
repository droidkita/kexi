/* This file is part of the KDE project
   Copyright (C) 2005-2008 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kexicsvimportoptionsdlg.h"
#include <widget/kexicharencodingcombobox.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qtextcodec.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qgridlayout.h>

#include <kapplication.h>
#include <kconfig.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kglobal.h>
#include <kcharsets.h>

KexiCSVImportOptions::DateFormat dateFormatFromString( const QString& s )
{
	QString str( s.toLower().trimmed() );
	if (str == "dmy")
		return KexiCSVImportOptions::DMY;
	if (str == "ymd")
		return KexiCSVImportOptions::YMD;
	if (str == "mdy")
		return KexiCSVImportOptions::MDY;
	return KexiCSVImportOptions::AutoDateFormat;
}

QString dateFormatToString( KexiCSVImportOptions::DateFormat format )
{
	switch (format) {
	case KexiCSVImportOptions::DMY: return "DMY";
	case KexiCSVImportOptions::YMD: return "YMD";
	case KexiCSVImportOptions::MDY: return "MDY";
	default: break;
	}
	return QString::null;
}

KexiCSVImportOptions::KexiCSVImportOptions()
{
	KConfigGroup importExportGroup( KGlobal::config()->group("ImportExport") );
	encoding = importExportGroup.readEntry("DefaultEncodingForImportingCSVFiles");
	if (encoding.isEmpty()) {
		encoding = QString::fromLatin1(KGlobal::locale()->encoding());
		defaultEncodingExplicitySet = false;
	}
	else
		defaultEncodingExplicitySet = true;

	dateFormat = dateFormatFromString( importExportGroup.readEntry("DateFormatWhenImportingCSVFiles") );

	trimmedInTextValuesChecked 
		= importExportGroup.readEntry("StripBlanksOffOfTextValuesWhenImportingCSVFiles", true);
}

KexiCSVImportOptions::~KexiCSVImportOptions()
{
}

bool KexiCSVImportOptions::operator== ( const KexiCSVImportOptions & opt ) const
{
	return defaultEncodingExplicitySet==opt.defaultEncodingExplicitySet
		&& trimmedInTextValuesChecked==opt.trimmedInTextValuesChecked
		&& encoding==opt.encoding
		&& dateFormat==opt.dateFormat;
}

bool KexiCSVImportOptions::operator!= ( const KexiCSVImportOptions & opt ) const
{
	return !( *this==opt );
}

//----------------------------------

KexiCSVImportOptionsDialog::KexiCSVImportOptionsDialog( 
	const KexiCSVImportOptions& options, QWidget* parent )
 : KDialog( parent )
{
	setCaption(i18n( "CSV Import Options" ));
	setButtons(Ok|Cancel);
	setDefaultButton(Ok);
	setObjectName("KexiCSVImportOptionsDialog");
	setModal(true);
	QWidget *plainPage = new QWidget(this);
	setMainWidget(plainPage);
	
	QGridLayout *lyr = new QGridLayout( plainPage, 4, 3, 
		0, KDialog::spacingHint());

	QGroupBox* textEncodingGroupBox = new QGroupBox( i18n("Text encoding"), plainPage );
	lyr->addMultiCellWidget( textEncodingGroupBox, 0, 0, 0, 1 );
	QVBoxLayout* textEncodingGroupBoxLyr = new QVBoxLayout( textEncodingGroupBox, KDialog::spacingHint(), KDialog::spacingHint() );
	textEncodingGroupBoxLyr->addItem( new QSpacerItem( 20, 15, QSizePolicy::Fixed, QSizePolicy::Fixed ) );

	m_encodingComboBox = new KexiCharacterEncodingComboBox(textEncodingGroupBox, options.encoding);
	textEncodingGroupBoxLyr->addWidget( m_encodingComboBox );

	lyr->addItem( new QSpacerItem( 20, KDialog::spacingHint(), QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 2 );

	m_chkAlwaysUseThisEncoding = new QCheckBox(
		i18n("Always use this encoding when importing CSV data files"), textEncodingGroupBox);
	textEncodingGroupBoxLyr->addWidget( m_chkAlwaysUseThisEncoding );

	m_comboDateFormat = new QComboBox( plainPage );
	m_comboDateFormat->setName( "m_comboDateFormat" );
	m_comboDateFormat->insertItem( i18nc("Date format: Auto", "Auto") );
	QString year( i18n("year") ), month( i18n("month") ), day( i18n("day") );
	QString mask= ki18nc("\"month, year, day\" mask", "%1, %2, %3 (e.g. %4-%5-%6)").toString();
	m_comboDateFormat->insertItem( mask.arg(day).arg(month).arg(year).arg(30).arg(12).arg(2008) );
	m_comboDateFormat->insertItem( mask.arg(year).arg(month).arg(day).arg(2008).arg(12).arg(30) );
	m_comboDateFormat->insertItem( mask.arg(month).arg(day).arg(year).arg(12).arg(30).arg(2008) );
	lyr->addWidget( m_comboDateFormat, 1, 1 );

	QLabel* lblDateFormat = new QLabel( m_comboDateFormat, i18n("Date format:"), plainPage);
	lyr->addWidget( lblDateFormat, 1, 0 );

	m_chkStripWhiteSpaceInTextValues = new QCheckBox(
		i18n("Strip leading and trailing blanks off of text values"), plainPage);
	lyr->addMultiCellWidget( m_chkStripWhiteSpaceInTextValues, 2, 2, 0, 1 );
	lyr->addItem( new QSpacerItem( 30, KDialog::spacingHint(), QSizePolicy::Minimum, QSizePolicy::Expanding ), 3, 0 );

	//update widgets
	if (options.defaultEncodingExplicitySet) {
		m_encodingComboBox->setSelectedEncoding(options.encoding);
		m_chkAlwaysUseThisEncoding->setChecked(true);
	}
	m_comboDateFormat->setCurrentItem( (int)options.dateFormat );
	m_chkStripWhiteSpaceInTextValues->setChecked(options.trimmedInTextValuesChecked);

	adjustSize();
	m_encodingComboBox->setFocus();
}

KexiCSVImportOptionsDialog::~KexiCSVImportOptionsDialog()
{
}

KexiCSVImportOptions KexiCSVImportOptionsDialog::options() const
{
	KexiCSVImportOptions opt;
	opt.encoding = m_encodingComboBox->selectedEncoding();
	opt.trimmedInTextValuesChecked = m_chkStripWhiteSpaceInTextValues->isChecked();
	return opt;
}

void KexiCSVImportOptionsDialog::accept()
{
	KConfigGroup importExportGroup( KGlobal::config()->group("ImportExport") );
	if (m_chkAlwaysUseThisEncoding->isChecked())
		importExportGroup.writeEntry("DefaultEncodingForImportingCSVFiles", 
			m_encodingComboBox->selectedEncoding());
	else
		importExportGroup.deleteEntry("DefaultEncodingForImportingCSVFiles");

	const KexiCSVImportOptions::DateFormat dateFormat 
		= (KexiCSVImportOptions::DateFormat)m_comboDateFormat->currentItem();
	if (dateFormat == KexiCSVImportOptions::AutoDateFormat)
		importExportGroup.deleteEntry("DateFormatWhenImportingCSVFiles");
	else
		importExportGroup.writeEntry("DateFormatWhenImportingCSVFiles", dateFormatToString( dateFormat ));

	importExportGroup.writeEntry("StripBlanksOffOfTextValuesWhenImportingCSVFiles", 
		m_chkStripWhiteSpaceInTextValues->isChecked());

	KDialog::accept();
}

#include "kexicsvimportoptionsdlg.moc"
