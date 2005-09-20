/***************************************************************************
 * kexiappmainwindow.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "kexiappmainwindow.h"

#include "core/keximainwindow.h"
#include "core/kexiproject.h"
#include "kexidb/connection.h"

#include "main/manager.h"

//#include <kdebug.h>

namespace Kross { namespace KexiApp {

    /// \internal
    class KexiAppMainWindowPrivate
    {
        public:
            KexiMainWindow* mainwindow;
    };

}}

using namespace Kross::KexiApp;

KexiAppMainWindow::KexiAppMainWindow(KexiMainWindow* mainwindow)
    : Kross::Api::Class<KexiAppMainWindow>("KexiAppMainWindow")
    , d(new KexiAppMainWindowPrivate())
{
    d->mainwindow = mainwindow;

    addFunction("getConnection", &KexiAppMainWindow::getConnection);
}

KexiAppMainWindow::~KexiAppMainWindow()
{
    delete d;
}


const QString KexiAppMainWindow::getClassName() const
{
    return "Kross::KexiApp::KexiAppMainWindow";
}

Kross::Api::Object::Ptr KexiAppMainWindow::getConnection(Kross::Api::List::Ptr)
{
    KexiProject* project = d->mainwindow->project();
    if(project) {
        ::KexiDB::Connection* connection = project->dbConnection();
        if(connection) {
            Kross::Api::Module* module = Kross::Api::Manager::scriptManager()->loadModule("krosskexidb");
            if(! module)
                throw Kross::Api::Exception::Ptr( new Kross::Api::Exception("Failed to load the krosskexidb module.") );
            return module->get("KexiDBConnection", connection);
        }
    }
    throw Kross::Api::Exception::Ptr( new Kross::Api::Exception("No connection established.") );
}

