/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __CALENDARMGR_H
#define __CALENDARMGR_H

#include "Common.h"
#include "Policies/Singleton.h"
#include "Utilities/EventProcessor.h"

#include "ace/Recursive_Thread_Mutex.h"

class MANGOS_DLL_SPEC CalendarMgr
{
    public:
        typedef std::map<
        /* Construction */
        CalendarMgr();
        ~CalendarMgr();

        void Update(uint32 diff);

    private:
    
};

#define sCalendarMgr MaNGOS::Singleton<CalendarMgr>::Instance()
#endif
