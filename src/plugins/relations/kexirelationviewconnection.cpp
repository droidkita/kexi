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

#include <qpainter.h>
#include <qpixmap.h>

#include <kdebug.h>

#include "kexirelationviewtable.h"
#include "kexirelationviewconnection.h"

#include "r1.xpm"
#include "rn.xpm"

KexiRelationViewConnection::KexiRelationViewConnection(KexiRelationViewTableContainer *srcTbl, KexiRelationViewTableContainer *rcvTbl,
  const QString &srcFld, const QString &rcvFld)
{
	kdDebug() << "KexiRelationViewConnection::KexiRelationViewConnection()" << endl;

	m_srcTable = srcTbl;
	if(!srcTbl || !rcvTbl)
	{
		kdDebug() << "KexiRelationViewConnection::KexiRelationViewConnection(): expect sig11" << endl;
		kdDebug() << "KexiRelationViewConnection::KexiRelationViewConnection()" << srcTbl << endl;
		kdDebug() << "KexiRelationViewConnection::KexiRelationViewConnection()" << rcvTbl << endl;
	}

	m_rcvTable = rcvTbl;
	m_srcField = srcFld;
	m_rcvField = rcvFld;
}

void
KexiRelationViewConnection::drawConnection(QPainter *p, QWidget *parent)
{
	int sx = m_srcTable->x() + m_srcTable->width();
	int sy = m_srcTable->globalY(m_srcField);
	int rx = m_rcvTable->x();
	int ry = m_rcvTable->globalY(m_rcvField);

	QPoint side1(0, 0);
	QPoint sideN(0, 0);

	if(m_srcTable->x() < m_rcvTable->x())
	{

		p->drawLine(sx + 6, sy, rx - 8, ry);

		p->drawLine(rx - 8, ry, rx, ry);
		p->drawPoint(rx - 2, ry - 1);
		p->drawPoint(rx - 2, ry + 1);
		p->drawLine(rx - 3, ry - 2, rx - 3, ry + 2);

		p->drawLine(sx, sy - 1, sx + 5, sy - 1);
		p->drawLine(sx, sy, sx + 6, sy);
		p->drawLine(sx, sy + 1, sx + 5, sy + 1);

		side1.setX(sx + 2);
		side1.setY(sy - 7);

		sideN.setX(rx - 10);
		sideN.setY(ry - 6);
	}
	else
	{
		int lx = rx + m_rcvTable->width();
		int rx = sx - m_srcTable->width();

		p->drawLine(lx + 8, ry, rx - 8, sy);

		p->drawLine(lx, ry, lx + 8, ry);
		p->drawPoint(lx + 1, ry - 1);
		p->drawPoint(lx + 1, ry + 1);
		p->drawLine(lx + 2, ry - 2, lx + 2, ry + 2);

		p->drawLine(rx - 7, sy - 1, rx, sy - 1);
		p->drawLine(rx - 7, sy + 1, rx, sy + 1);
		p->drawLine(rx - 8, sy, rx, sy);

		side1.setX(rx - 4);
		side1.setY(sy - 7);

		sideN.setX(lx + 3);
		sideN.setY(ry - 6);
	}

	p->drawPixmap(side1, QPixmap(r1_xpm));
	p->drawPixmap(sideN, QPixmap(rn_xpm));
}

const QRect
KexiRelationViewConnection::connectionRect()
{

	int sx = m_srcTable->x();
	int rx = m_rcvTable->x();
	int ry = m_rcvTable->globalY(m_rcvField);
	int sy = m_srcTable->globalY(m_srcField);

	int width, leftX, rightX;

	if(sx < rx)
	{
		leftX = sx;
		rightX = rx;
		width = m_srcTable->width();
	}
	else
	{
		leftX = rx;
		rightX = sx;
		width = m_rcvTable->width();
	}


	int dx = QABS((leftX + width) - rightX);
	int dy = QABS(sy - ry) + 2;
	
	int top = QMIN(sy, ry);
	int left = leftX + width;


//	return QRect(sx - 1, sy - 1, (rx + m_rcvTable->width()) - sx + 1, ry - sy + 1);
	QRect rect(left - 3, top - 7, dx + 3, dy + 7);
	m_oldRect = rect;
	
	return rect;
}

KexiRelationViewConnection::~KexiRelationViewConnection()
{
}

