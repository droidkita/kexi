/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIREPORTFORM_H
#define KEXIREPORTFORM_H

#include <qwidget.h>
#include <qpixmap.h>

#include "form.h"

//! The report top widget
class KexiReportForm : public QWidget, public KFormDesigner::FormWidget
{
	Q_OBJECT

	public:
		KexiReportForm(QWidget *parent, const char *name="kexi_dbform");
		virtual ~KexiReportForm();

		/*QString datasource() const { return m_ds; }
		bool navigatorShown() const { return m_nav; }
		void setDatasource(const QString &s) { m_ds = s; }
		void showRecordNavigator(bool s) { m_nav = s; }*/

		virtual void drawRect(const QRect& r, int type);
		virtual void drawRects(const QValueList<QRect> &list, int type);
		virtual void initBuffer();
		virtual void clearForm();
		virtual void highlightWidgets(QWidget *from, QWidget *to/*, const QPoint &p*/);

		virtual QSize sizeHint() const;

	private:
		/*QString m_ds;
		bool m_nav;
		KexiDB::Connection *m_conn;*/

		QPixmap buffer; //!< stores grabbed entire form's area for redraw
		QRect prev_rect; //!< previously selected rectangle
};

#endif
