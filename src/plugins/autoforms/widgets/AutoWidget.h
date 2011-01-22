/*
    Kexi Auto Form Plugin*
    Copyright (C) 2011  Adam Pigg <adam@piggz.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef AUTOWIDGETBASE_H
#define AUTOWIDGETBASE_H

#include <QtGui/QWidget>
#include <QVariant>

class QLabel;
class QHBoxLayout;
namespace KexiDB {
class Field;
}


class AutoWidget : public QWidget
{
    Q_OBJECT
public:
    virtual ~AutoWidget();
    virtual QVariant value() = 0;
    virtual void setValue(QVariant val);
    
protected:
    explicit AutoWidget(KexiDB::Field *fld, QWidget* parent = 0);
    
    KexiDB::Field *m_field;
    
    QVariant m_OriginalValue;
    QHBoxLayout *m_layout;
    
private:
    QLabel *m_fieldLabel;
};

#endif // AUTOWIDGETBASE_H
