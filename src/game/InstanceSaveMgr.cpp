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

#include "Common.h"
#include "Database/SQLStorage.h"

#include "Player.h"
#include "GridNotifiers.h"
#include "Log.h"
#include "GridStates.h"
#include "CellImpl.h"
#include "Map.h"
#include "MapManager.h"
#include "MapInstanced.h"
#include "InstanceSaveMgr.h"
#include "Timer.h"
#include "GridNotifiersImpl.h"
#include "Transports.h"
#include "ObjectMgr.h"
#include "World.h"
#include "Group.h"
#include "InstanceData.h"
#include "ProgressBar.h"

INSTANTIATE_SINGLETON_1( InstanceSaveManager );

//== InstanceSave functions ================================

InstanceSave::InstanceSave(uint32 MapId, uint32 InstanceId, Difficulty difficulty, bool perm, uint32 encountersMask)
{
    m_mapId = MapId;
    m_instanceGuid = (uint64(InstanceId) | (uint64(HIGHGUID_INSTANCE) << 48));
    m_diff = difficulty;
    m_perm = perm;
    m_encountersMask = encountersMask;

    // Calculate reset time
    uint32 now = time(NULL);
    resetTime = sWorld.getConfig(CONFIG_UINT32_INSTANCE_RESET_CONSTANT);
    uint32 resetPeriod = GetMapDifficultyData(m_mapId, m_diff)->resetTime;
    if(!resetPeriod) resetPeriod = DAY;

    while(resetTime < now)
        resetTime += resetPeriod;
}

InstanceSave::~InstanceSave()
{
    m_players.clear();
    m_extended.clear();
}

bool InstanceSave::LoadPlayers()
{
    QueryResult *result = CharacterDatabase.PQuery("SELECT guid, extended FROM character_instance WHERE instance = '%u'", m_instanceGuid.GetCounter());
    if(!result)
        return false;
    do
    {
        Field *fields = result->Fetch();
        m_players.insert(fields[0].GetUInt32());
        if(fields[1].GetBool())
            m_extended.insert(fields[0].GetUInt32());
    } while( result->NextRow() );
    
    return true;
}

void InstanceSave::SaveToDb(bool players)
{
    CharacterDatabase.PQuery("DELETE FROM instance WHERE id = '%u'", m_instanceGuid.GetCounter());
    CharacterDatabase.PQuery("INSERT INTO instance (id, map, difficulty, perm) VALUES ('%u','%u','%u','%u');",
        uint32(m_instanceGuid.GetCounter()), uint32(m_mapId), uint8(m_diff), uint8(m_perm));
    
    if(!players)
        return;

    CharacterDatabase.PQuery("DELETE FROM character_instance WHERE instance = '%u'", m_instanceGuid.GetCounter());
    for(PlrListSaves::iterator itr = m_players.begin(); itr != m_players.end(); ++itr)
    {
        CharacterDatabase.PQuery("INSERT INTO character_instance (guid, instance, extended) VALUES ('%u','%u','%u');",
            GUID_LOPART(*itr), uint32(m_instanceGuid.GetCounter()), uint8(m_extended.find(*itr) != m_extended.end()));
    }
}

void InstanceSave::ExtendFor(uint64 guid)
{
    m_extended.insert(guid);
    CharacterDatabase.PQuery("UPDATE character_instance SET extended = 1 WHERE guid = '%u'", GUID_LOPART(guid));
}

void InstanceSave::RemoveExtended(uint64 guid)
{
    m_extended.erase(guid);
    CharacterDatabase.PQuery("UPDATE character_instance SET extended = 0 WHERE guid = '%u'", GUID_LOPART(guid));
}

void InstanceSave::AddPlayer(uint64 guid)
{
    if(m_players.find(guid) != m_players.end())
        return;
    m_players.insert(guid);
    CharacterDatabase.PQuery("INSERT INTO character_instance (guid, instance, extended) VALUES ('%u','%u','%u');",
        GUID_LOPART(guid), uint32(m_instanceGuid.GetCounter()), uint8(0)); 
}

void InstanceSave::RemovePlayer(uint64 guid)
{
    if(m_players.find(guid) == m_players.end())
        return;
    m_players.erase(guid);
    m_extended.erase(guid);
    CharacterDatabase.PQuery("DELETE FROM character_instance WHERE instance = '%u' AND guid = '%u'", m_instanceGuid.GetCounter(), guid);
}

void InstanceSave::UpdateId(uint32 id)
{
    CharacterDatabase.PQuery("UPDATE instance SET id = '%u' WHERE id = '%u'", id, m_instanceGuid.GetCounter()); 
    CharacterDatabase.PQuery("UPDATE character_instance SET instance = '%u' WHERE instance = '%u'", id, m_instanceGuid.GetCounter());
    CharacterDatabase.PQuery("UPDATE corpse SET instance = '%u' WHERE instance = '%u'", id, m_instanceGuid.GetCounter());
    WorldDatabase.PQuery("UPDATE creature_respawn SET instance = '%u' WHERE instance = '%u'", id, m_instanceGuid.GetCounter()); 
    WorldDatabase.PQuery("UPDATE gameobject_respawn SET instance = '%u' WHERE instance = '%u'", id, m_instanceGuid.GetCounter()); 
    m_instanceGuid = (uint64(id) | (uint64(HIGHGUID_INSTANCE) << 48));
}

void InstanceSave::DeleteFromDb()
{
    CharacterDatabase.PQuery("DELETE FROM instance WHERE id = '%u'", m_instanceGuid.GetCounter());
    CharacterDatabase.PQuery("DELETE FROM character_instance WHERE instance = '%u'", m_instanceGuid.GetCounter());
    CharacterDatabase.PQuery("DELETE FROM corpse WHERE instance = '%u'", m_instanceGuid.GetCounter());
    WorldDatabase.PQuery("DELETE FROM creature_respawn WHERE instance = '%u'", m_instanceGuid.GetCounter());
    WorldDatabase.PQuery("DELETE FROM gameobject_respawn WHERE instance = '%u'", m_instanceGuid.GetCounter());
}

void InstanceSave::RemoveAndDelete()
{
    Player *plr = NULL;
    for(PlrListSaves::iterator itr = m_players.begin(); itr != m_players.end(); ++itr)
    {
        plr = sObjectMgr.GetPlayer(*itr);
        if(!plr || !plr->IsInWorld())
            continue;
        plr->UnbindInstance(m_mapId, m_diff);
    }
    DeleteFromDb();
    m_players.clear();
}

bool InstanceSave::RemoveOrExtendPlayers()
{
    PlrListSaves::iterator itr, itr_next;
    for(itr = m_players.begin(); itr != m_players.end(); itr = itr_next)
    {
        itr_next = itr;
        ++itr_next;
        if(!IsExtended(*itr))
            RemovePlayer(*itr);
    }

    bool hasExtended = !m_extended.empty();
    m_extended.clear();
    if(hasExtended)
    {
        uint32 period = GetMapDifficultyData(m_mapId, m_diff)->resetTime;
        resetTime += period ? period : DAY;
    }

    return hasExtended;
}

//== InstanceSaveManager functions =========================

InstanceSaveManager::InstanceSaveManager()
{
}

InstanceSaveManager::~InstanceSaveManager()
{

}

void InstanceSaveManager::LoadSavesFromDb()
{
    uint32 count = 0;
    //                                                    0   1    2           3     4
    QueryResult *result = CharacterDatabase.Query("SELECT id, map, difficulty, perm, encountersMask FROM instance");

    if ( !result )
    {
        barGoLink bar( 1 );
        bar.step();
        sLog.outString();
        sLog.outString( ">> Loaded %u instance saves", count );
        return;
    }
    barGoLink bar( (int)result->GetRowCount() );
    do
    {
        Field *fields = result->Fetch();

        bar.step();

        InstanceSave *save = new InstanceSave(fields[1].GetUInt32(), fields[0].GetUInt32(), Difficulty(fields[2].GetUInt8()), fields[3].GetBool(), fields[4].GetUInt32());
        if(!save->LoadPlayers())
        {
            save->DeleteFromDb();
            sLog.outError("Instance save %u has 0 players, deleting...", fields[0].GetUInt32());
            continue;
        }
        m_saves.insert(std::make_pair<uint32, InstanceSave*>(save->GetGUID(), save));
        ++count;
    } while( result->NextRow() );

    delete result;

    sLog.outString();
    sLog.outString( ">> Loaded %u instance saves.", count );
}

void InstanceSaveManager::PackInstances()
{
    std::list<uint32> freeGuids;
    uint32 lastGuid = 0;
    for(InstanceSaveMap::iterator itr = m_saves.begin(); itr != m_saves.end(); ++itr)
    {
        if(itr->first - lastGuid > 1)
        {
            ++lastGuid;
            for(; lastGuid < itr->first; ++lastGuid)
                freeGuids.push_back(lastGuid);
        }else
            ++lastGuid;
    }
    if(freeGuids.empty())
        return;

    
    for(InstanceSaveMap::reverse_iterator itr = m_saves.rbegin(); itr != m_saves.rend();)
    {
        itr->second->UpdateId(*(freeGuids.begin()));
        m_saves.insert(std::make_pair<uint32, InstanceSave*>(*(freeGuids.begin()), itr->second));
        m_saves.erase(itr->first);
        itr = m_saves.rbegin();
        freeGuids.pop_front();
        if(freeGuids.empty())
            break;
    }
    sObjectMgr.SetMaxInstanceId(m_saves.rbegin()->first);
    sLog.outString( ">> Instance numbers remapped, next instance id is %u", m_saves.rbegin()->first+1 );
}

//Only for new save
InstanceSave* InstanceSaveManager::CreateInstanceSave(uint16 mapId, uint32 id, Difficulty difficulty, bool perm)
{
    error_log("ID: %u", id);
    InstanceSave* save = GetInstanceSave(id);
    if(id && save)
    {
        error_log("mam");
        return save;
    }

    id = sObjectMgr.GenerateLowGuid(HIGHGUID_INSTANCE);
    save = new InstanceSave(mapId, id, difficulty, perm);
    save->SaveToDb();

    m_saves.insert(std::make_pair<uint32, InstanceSave*>(id, save));
    return save;
}

void InstanceSaveManager::CheckResetTimes()
{
    uint32 now = time(NULL);
    InstanceSaveMap::iterator itr, itr_next;
    for(itr = m_saves.begin(); itr != m_saves.end(); itr = itr_next)
    {
        itr_next = itr;
        ++itr_next;

        if(itr->second->GetResetTime() > now) // Ok, not expired
            continue;

        bool hasExtened = itr->second->RemoveOrExtendPlayers();

        //Teleport players out
        Map *map = sMapMgr.FindMap(itr->second->GetMapId(), itr->first);
        if(map)
        {
            if(map->HavePlayers())
            {
                const Map::PlayerList &players = map->GetPlayers();
                for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                {
                    if(itr->getSource())
                        itr->getSource()->RepopAtGraveyard();
                }
            }
            if(!hasExtened)
                ((InstanceMap*)map)->Reset(INSTANCE_RESET_RESPAWN_DELAY);
        }
        if(!hasExtened)
            DeleteSave(itr->first);
    }
}
