/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#include <qvariant.h>
#include <qdatetime.h>

#include <kdebug.h>

#include "qtsqlresult.h"
#include "qtsqldb.h"

QtSqlResult::QtSqlResult(/*MYSQL_RES*/void *result, QtSqlDB *parent)
{
/*	m_parent = parent;
	//various initialisations...
	m_result = result;
	m_row = 0;
	m_lengths = 0;
	m_field = 0;
	m_currentRecord = 0;
	m_numFields = mysql_num_fields(m_result);
	kdDebug()<<"m_numFields:"<<m_numFields<<endl;
	//creating field-index
	int i=0;
	while((m_field = mysql_fetch_field(m_result)))
	{
		//field name...
		m_fieldNames.insert(QString::fromLatin1(m_field->name), i);
		//field infos...
		KexiDBField* f = new KexiDBField(m_field->table);
		f->setName(m_field->name);
		f->setColumnType(QtSqlDB::getInternalDataType(m_field->type));
		f->setLength(m_field->length);
		f->setPrecision(m_field->decimals);
		f->setUnsigned(m_field->flags & UNSIGNED_FLAG);
		f->setBinary(m_field->flags & BINARY_FLAG);
		f->setDefaultValue(m_field->def);
		f->setAutoIncrement(m_field->flags & AUTO_INCREMENT_FLAG);
		f->setPrimaryKey(m_field->flags & PRI_KEY_FLAG);
		f->setUniqueKey(m_field->flags & UNIQUE_KEY_FLAG);
		f->setNotNull(m_field->flags & NOT_NULL_FLAG);
		m_fields.insert(i, f);

		//incrementing...
		i++;
	}*/
}

unsigned int
QtSqlResult::numFields()
{
	return m_numFields;
}

unsigned int
QtSqlResult::numRows()
{
//	return mysql_num_rows(m_result);
}

bool
QtSqlResult::next()
{
/*	m_row = mysql_fetch_row(m_result);
	m_lengths = mysql_fetch_lengths(m_result);

	if(!m_row)
	{
		return false;
	}

	m_currentRecord++;
	return true;*/
}

bool
QtSqlResult::prev()
{
/*	mysql_data_seek(m_result, m_currentRecord - 1);
	m_row = mysql_fetch_row(m_result);
	m_lengths = mysql_fetch_lengths(m_result);

	if(!m_row)
		return false;

	m_currentRecord--;*/
	return true;
}

bool
QtSqlResult::gotoRecord(unsigned long r)
{
/*	mysql_data_seek(m_result, r);
	m_row = mysql_fetch_row(m_result);
	m_lengths = mysql_fetch_lengths(m_result);

	if(!m_row)
	{
		kdDebug() << "QtSqlResult::gotoRecord(): jump faild!" << endl;
		return false;
	}

	m_currentRecord = r;*/
	return true;
}

QVariant
QtSqlResult::value(unsigned int field)
{
/*	if(!m_row)
	{
		return 0;
	}

	switch(fieldInfo(field)->type())
	{
		case QVariant::Date:
			return QVariant(QDate::fromString((m_row)[field], Qt::ISODate));
		case QVariant::Int:
			return QVariant(QString((m_row)[field]).toInt());
		case QVariant::ByteArray:
		{
			QByteArray ba;
			ba.duplicate((m_row)[field], m_lengths[field]);
			return QVariant(ba);
		}
		default:
			return QVariant(m_parent->decode((m_row)[field]));
	}
*/
	return 0;
}

QVariant
QtSqlResult::value(QString field)
{
	if(!m_fieldNames.contains(field))
		return QVariant();

	FieldNames::Iterator it;
	it = m_fieldNames.find(field);
	return value(it.data());
}

KexiDBField*
QtSqlResult::fieldInfo(unsigned int field)
{
	return m_fields[field];
}

KexiDBField*
QtSqlResult::fieldInfo(QString field)
{
	FieldNames::Iterator it;
	it = m_fieldNames.find(field);
	return fieldInfo(it.data());
}

unsigned int
QtSqlResult::currentRecord()
{
	return m_currentRecord;
}

QtSqlResult::~QtSqlResult()
{
/*	kdDebug()<<"QtSqlResult::~QtSqlResult()"<<endl;
	mysql_free_result(m_result);*/
}
