/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef _KEXI_GLOBAL_
#define _KEXI_GLOBAL_

/* 
    Global app definitions
*/

#include <klocale.h>

#ifdef OOPL_VERSION
# include "oopl_global.h"
#else //default
# define KEXI_APP_NAME I18N_NOOP("Kexi")
# define KEXI_VERSION VERSION
#endif

/*
 * this is the version a part has to be only increase it if the interface isn't binary compatible anymore
 */
#define KEXI_PART_VERSION 1

#endif

