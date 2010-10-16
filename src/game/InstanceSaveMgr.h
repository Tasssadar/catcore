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

#ifndef __InstanceSaveMgr_H
#define __InstanceSaveMgr_H

#include "Platform/Define.h"
#include "Policies/Singleton.h"
#include "ace/Thread_Mutex.h"
#include <list>
#include <map>
#include "Utilities/UnorderedMap.h"
#include "Database/DatabaseEnv.h"
#include "DBCEnums.h"
#include "ObjectGuid.h"

struct InstanceTemplate;
struct MapEntry;
class Player;
class Group;

/*
    Holds the information necessary for creating a new map for an existing instance
    Is referenced in three cases:
    - player-instance binds for solo players (not in group)
    - player-instance binds for permanent heroic/raid saves
    - group-instance binds (both solo and permanent) cache the player binds for the group leader
*/
class InstanceSave
{
    InstanceSave(uint16 MapId, uint64 InstanceId, Difficulty difficulty, bool perm);
    ~InstanceSave();
};

class MANGOS_DLL_DECL InstanceSaveManager : public MaNGOS::Singleton<InstanceSaveManager, MaNGOS::ClassLevelLockable<InstanceSaveManager, ACE_Thread_Mutex> >
{
    public:
        InstanceSaveManager();
        ~InstanceSaveManager();
        void Update();
    private:
        
};

#define sInstanceSaveMgr MaNGOS::Singleton<InstanceSaveManager>::Instance()
#endif
