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

InstanceSave::InstanceSave(uint32 MapId, uint32 InstanceId, Difficulty difficulty, bool perm, bool extended, bool expired);
{
    m_mapId = MapId;
    m_instanceGuid = (uint64(InstanceId) | (uint64(HIGHGUID_INSTANCE) << 48));
    m_diff = difficulty;
    perm = perm;
    m_extended = extended;
    m_expired = expired;

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
    QueryResult *result = CharacterDatabase.PQuery("SELECT guid, extended FROM character_instance WHERE instance = '%u'", m_instanceId.GetCounter());
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
    CharacterDatabase.PQuery("DELETE FROM instance WHERE id = '%u'", m_instanceId.GetCounter());
    CharacterDatabase.PQuery("INSERT INTO instance (id, map, difficulty, perm) VALUES ('%u','%u','%u','%u');",
        uint32(m_instanceId.GetCounter()), uint32(m_mapId), uint8(m_diff), uint8(perm));
    
    if(!players)
        return;

    CharacterDatabase.PQuery("DELETE FROM character_instance WHERE instance = '%u'", m_instanceId.GetCounter());
    for(PlrListSaves::iterator itr = m_players.begin(); itr != m_players.end(); ++itr)
    {
        CharacterDatabase.PQuery("INSERT INTO character_instance (guid, instance, extended) VALUES ('%u','%u','%u');",
            GUID_LOPART(*itr), uint32(m_instanceId.GetCounter(), uint8(m_extended.find(*itr) != m_extended.end()));
    }
}

void InstanceSave::UpdateId(uint32 id)
{
    CharacterDatabase.PQuery("UPDATE instance SET id = '%u' WHERE id = '%u'", id, m_instanceId.GetCounter()); 
    CharacterDatabase.PQuery("UPDATE character_instance SET instance = '%u' WHERE instance = '%u'", id, m_instanceId.GetCounter());
    CharacterDatabase.PQuery("UPDATE corpse SET instance = '%u' WHERE instance = '%u'", id, m_instanceId.GetCounter());
    WorldDatabase.PQuery("UPDATE creature_respawn SET instance = '%u' WHERE instance = '%u'", id, m_instanceId.GetCounter()); 
    WorldDatabase.PQuery("UPDATE gameobject_respawn SET instance = '%u' WHERE instance = '%u'", id, m_instanceId.GetCounter()); 
    m_instanceGuid = (uint64(id) | (uint64(HIGHGUID_INSTANCE) << 48));
}

void InstanceSave::DeleteFromDb()
{
    CharacterDatabase.PQuery("DELETE FROM instance WHERE id = '%u'", m_instanceId.GetCounter());
    CharacterDatabase.PQuery("DELETE FROM character_instance WHERE instance = '%u'", m_instanceId.GetCounter());
    CharacterDatabase.PQuery("DELETE FROM corpse WHERE instance = '%u'", m_instanceId.GetCounter());
    WorldDatabase.PQuery("DELETE FROM creature_respawn WHERE instance = '%u'", m_instanceId.GetCounter());
    WorldDatabase.PQuery("DELETE FROM gameobject_respawn WHERE instance = '%u'", m_instanceId.GetCounter());
}

void InstanceSave::RemoveAndDelete()
{
    // TODO: Remove from online players
    DeleteFromDb();
    m_players.clear();
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
    //                                                    0   1    2           3
    QueryResult *result = CharacterDatabase.Query("SELECT id, map, difficulty, perm FROM instance");

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

        InstanceSave *save = new InstanceSave(fields[1].GetUInt32(), fields[0].GetUInt32(), Difficulty(fields[2].GetUInt8()), fields[3].GetBool());
        if(!save->LoadPlayers())
        {
            sLog.outError("Instance save %u has 0 players, skipping...", fields[0].GetUInt32());
            continue;
        }
        m_saves.insert(std::make_pair<uint32, InstanceSave*>(save->GetId(), save));
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

    for(InstanceSaveMap::iterator itr = m_saves.rbegin(); itr != m_saves.rend(); ++itr)
    {
        itr->second->UpdateId(*(freeGuids.begin()));
        itr->first = *(freeGuids.begin()); //safe?
        freeGuids.pop_front();
        if(freeGuids.empty())
            break;
    }
    sObjectMgr.SetMaxInstanceId(m_saves.rbegin()->first);
    sLog.outString( ">> Instance numbers remapped, next instance id is %u", m_saves.rbegin()->first+1 );
}

//Only for new save
InstanceSave* InstanceSaveManager::CreateInstanceSave(uint16 mapId, Difficulty difficulty, bool perm)
{
    uint32 id = sObjectMgr.GenerateLowGuid(HIGHGUID_INSTANCE);
    InstanceSave* save = new InstanceSave(mapId, id, difficulty, perm);

    m_saves.insert(std::make_pair<uint32, InstanceSave*>(id, save));
    return save;
}

void InstanceSaveManager::CheckResetTimes()
{
    //TODO: extended locks
    uint32 now = time(NULL);
    InstanceSaveMap::iterator itr, itr_next;
    for(itr = m_saves.begin(); itr != m_saves.end(); itr = itr_next)
    {
        itr_next = itr;
        ++itr_next;

        if(itr->second->GetResetTime() > now) // Ok, not expired
            continue;

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
            //Skip and try at next call if not reseted
            if(!((InstanceMap*)map)->Reset(INSTANCE_RESET_RESPAWN_DELAY))
                continue;
            DeleteSave(itr->first);
        }        
    }
}
