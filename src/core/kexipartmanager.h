/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

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

#ifndef KEXIPARTMANAGER_H
#define KEXIPARTMANAGER_H

#include <qobject.h>
#include <qdict.h>
#include <qvaluelist.h>
#include <qptrlist.h>
#include <kservice.h>

#include "kexipartinfo.h"

namespace KexiDB
{
	class Connection;
}

namespace KexiPart
{
	class Part;
	class Info;


	struct Missing
	{
		QString name;
		QString mime;
		QString url;
	};

	typedef QDict<Info> PartList;
	typedef QDictIterator<Info> PartListIterator;
	typedef QValueList<Missing> MissingList;
	typedef QPtrList<Info> Parts;

/**
 * queries parts and dlopens them when needed, they aren't dlopened at startup tough
 */
class Manager : public QObject
{
	Q_OBJECT

	public:
		/**
		 * creates a emty instance
		 */
		Manager(QObject *parent);
		~Manager();

		/**
		 * queries ktrader and creates a list of available parts
		 */
		void		lookup();

		/**
		 * dlopens a part identified by the part mime
		 * @returns a KexiPart::Part or 0 if loading faild
		 */
		Part		*load(const QString &mime);

		/**
		 * dlopens a part unsing KexiPart::Info
		 */
		Part		*load(Info *);

		/**
		 * @returns the info of a coresponding internal mime
		 */
		Info		*info(const QString &mime);

		/**
		 * checks project's kexi__part table, creates one if nessesary
		 * and checks if all parts used in a project are available locally
		 *
		 * use @ref missingParts() to get a list of missing parts
		 */
		void		checkProject(KexiDB::Connection *conn);

		/**
		 * @returns parts metioned in the project meta tables but not available locally
		 */
		MissingList	missingParts() const { return m_missing; }


		/**
		 * @returns a list of the available KexiParts
		 */
		Parts		*partList() { return &m_partlist; }

	private:
		Parts		m_partlist;
		PartList	m_parts;
		MissingList	m_missing;
};

}

#endif

