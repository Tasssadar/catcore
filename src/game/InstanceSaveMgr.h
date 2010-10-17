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

class InstanceSave
{
    public:
        typedef std::set<uint64> PlrListSaves;
        InstanceSave(uint32 MapId, uint32 InstanceId, Difficulty difficulty, bool perm, uint32 encountersMask = 0);
        ~InstanceSave();

        uint64 const& GetGUID() const { return m_instanceGuid.GetRawValue(); }
        ObjectGuid const& GetObjectGuid() const { return m_instanceId; }
        uint32 GetMapId() const { return m_mapId;}
        uint32 GetResetTime() const { return resetTime; }
        Difficulty GetDifficulty() const { return m_diff; }
        void SetPermanent(bool yes) { if(yes && !perm) perm = true; }
        bool IsExtended(uint64 guid) const { return (m_extended.find(guid) != m_extended.end()); }
        bool IsPermanent() const { return perm; }
        bool HasPlayers() const { return (!m_players.empty()); }
        uint32 GetEncounterMask() const { return m_encountersMask; }

        bool LoadPlayers();
        void SaveToDb(bool players = false);
        void DeleteFromDb();
        void RemoveAndDelete();
        void AddPlayer(uint64 guid);
        void RemovePlayer(uint64 guid);
        void UpdateId(uint32 id);

    private:
        uint32 m_mapId;
        ObjectGuid m_instanceGuid;
        Difficulty m_diff;
        bool perm;
        PlrListSaves m_players;
        PlrListSaves m_extended;
        uint32 resetTime; //timestamp
        uint32 m_encountersMask;
};

class MANGOS_DLL_DECL InstanceSaveManager : public MaNGOS::Singleton<InstanceSaveManager, MaNGOS::ClassLevelLockable<InstanceSaveManager, ACE_Thread_Mutex> >
{
    public:
        typedef std::map<uint32, InstanceSave*> InstanceSaveMap;
        InstanceSaveManager();
        ~InstanceSaveManager();

        void CheckResetTime();
        void LoadSavesFromDb();
        void PackInstances();

        InstanceSave* CreateInstanceSave(uint16 mapId, uint32 id, Difficulty difficulty, bool perm);
        void DeleteSave(uint32 id)
        {
            InstanceSaveMap::iterator itr = m_saves.find(id);
            if(itr == m_saves.end())
                return;
            itr->second->RemoveAndDelete();
            m_saves.erase(itr);
        }
        InstanceSave* GetInstanceSave(uint32 id)
        {
            InstanceSaveMap::iterator itr = m_saves.find(id);
            if(itr == m_saves.end())
                return NULL;
            return itr->second;
        }
        //Called every hour or at startup
        void CheckResetTimes();

    private:
        InstanceSaveMap m_saves;
        
};

#define sInstanceSaveMgr MaNGOS::Singleton<InstanceSaveManager>::Instance()
#endif
