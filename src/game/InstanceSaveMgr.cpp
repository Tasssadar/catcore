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
    mapEntry = sMapStore.LookupEntry(m_mapId);
    m_instanceGuid = MAKE_NEW_GUID(InstanceId, 0, HIGHGUID_INSTANCE);
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

void InstanceSave::SetPermanent(bool yes)
{
    if(yes && !m_perm && (m_diff != DUNGEON_DIFFICULTY_NORMAL || mapEntry->IsRaid()))
    {
        m_perm = true;
        CharacterDatabase.PQuery("UPDATE instance SET perm = '%u' WHERE id = '%u'", 1, m_instanceGuid.GetCounter());
    }
}

void InstanceSave::AddEncounter(uint32 mask)
{
    if(m_encountersMask & mask)
        return;

    m_encountersMask |= mask;
    CharacterDatabase.PQuery("UPDATE instance SET encountersMask = '%u' WHERE id = '%u'", m_encountersMask, m_instanceGuid.GetCounter());
}

bool InstanceSave::LoadPlayers()
{
    QueryResult *result = CharacterDatabase.PQuery("SELECT guid, extended FROM character_instance WHERE instance = '%u'", m_instanceGuid.GetCounter());
    if(!result)
        return false;
    do
    {
        Field *fields = result->Fetch();
        uint64 guid = MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);
        m_players.insert(guid);
        if(fields[1].GetBool())
            m_extended.insert(guid);
    } while( result->NextRow() );
    
    return true;
}

void InstanceSave::SaveToDb(bool players, bool data)
{
    std::string data_db = "";
    if(data)
    {
        QueryResult* result = CharacterDatabase.PQuery("SELECT data FROM instance WHERE id = '%u'", m_instanceGuid.GetCounter());
        if (result)
        {
            Field* fields = result->Fetch();
            const char* data1 = fields[0].GetString();
            if (data1)
                data_db = data1;
            delete result;
        }
    }
    CharacterDatabase.escape_string(data_db);
    CharacterDatabase.PQuery("DELETE FROM instance WHERE id = '%u'", m_instanceGuid.GetCounter());
    CharacterDatabase.PQuery("INSERT INTO instance (id, map, difficulty, resettime, perm, data) VALUES ('%u','%u','%u','%u','%u','%s');",
        uint32(m_instanceGuid.GetCounter()), uint32(m_mapId), uint8(m_diff), uint32(resetTime), uint8(m_perm), data_db.c_str());
    
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
    CharacterDatabase.PQuery("UPDATE character_instance SET extended = 1 WHERE guid = '%u' AND instance='%u'", GUID_LOPART(guid), m_instanceGuid.GetCounter());
}

void InstanceSave::RemoveExtended(uint64 guid)
{
    m_extended.erase(guid);
    CharacterDatabase.PQuery("UPDATE character_instance SET extended = 0 WHERE guid = '%u' AND instance='%u'", GUID_LOPART(guid), m_instanceGuid.GetCounter());
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
  //  Player *plr = sObjectMgr.GetPlayer(guid);
//    if(plr)
//        plr->UnbindInstance(m_mapId, m_diff);
}

void InstanceSave::UpdateId(uint32 id)
{
    CharacterDatabase.PQuery("UPDATE instance SET id = '%u' WHERE id = '%u'", id, m_instanceGuid.GetCounter()); 
    CharacterDatabase.PQuery("UPDATE character_instance SET instance = '%u' WHERE instance = '%u'", id, m_instanceGuid.GetCounter());
    CharacterDatabase.PQuery("UPDATE corpse SET instance = '%u' WHERE instance = '%u'", id, m_instanceGuid.GetCounter());
    WorldDatabase.PQuery("UPDATE creature_respawn SET instance = '%u' WHERE instance = '%u'", id, m_instanceGuid.GetCounter()); 
    WorldDatabase.PQuery("UPDATE gameobject_respawn SET instance = '%u' WHERE instance = '%u'", id, m_instanceGuid.GetCounter()); 
    m_instanceGuid = MAKE_NEW_GUID(id, 0, HIGHGUID_INSTANCE);
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
        {
           Player *plr = sObjectMgr.GetPlayer(*itr);
           if(plr)
               plr->UnbindInstance(m_mapId, m_diff);
           else
               RemovePlayer(*itr);
        }
    }

    bool hasExtended = !m_extended.empty();
    m_extended.clear();
    if(hasExtended)
    {
        uint32 period = GetMapDifficultyData(m_mapId, m_diff)->resetTime;
        resetTime += period ? period : DAY;
        CharacterDatabase.PQuery("UPDATE instance SET resettime = '%u' WHERE id = '%u'", resetTime, m_instanceGuid.GetCounter()); 
        CharacterDatabase.PQuery("UPDATE character_instance SET extended = 0 WHERE instance = '%u'", m_instanceGuid.GetCounter()); 
        WorldDatabase.PQuery("UPDATE creature_respawn SET respawntime = '%u' WHERE instance = '%u'", resetTime, m_instanceGuid.GetCounter());
        WorldDatabase.PQuery("UPDATE gameobject_respawn SET respawntime = '%u' WHERE instance = '%u'", resetTime, m_instanceGuid.GetCounter());
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
    //                                                    0   1    2           3          4     5
    QueryResult *result = CharacterDatabase.Query("SELECT id, map, difficulty, resettime, perm, encountersMask FROM instance");

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

        InstanceSave *save = new InstanceSave(fields[1].GetUInt32(), fields[0].GetUInt32(), Difficulty(fields[2].GetUInt8()), fields[4].GetBool(), fields[5].GetUInt32());
        if(!save->LoadPlayers())
        {
            save->DeleteFromDb();
            sLog.outError("Instance save %u (map %u) has 0 players, deleting...", fields[0].GetUInt32(), fields[1].GetUInt32());
            continue;
        }
        // if its 0, then leave recalculated time
        if(fields[3].GetUInt32() == 0)
            save->SaveToDb(false,true);
        else
            save->SetResetTime(fields[3].GetUInt32());
        m_saves.insert(std::make_pair<uint32, InstanceSave*>(save->GetGUID(), save));
        ++count;
    } while( result->NextRow() );

    delete result;

    sLog.outString();
    sLog.outString( ">> Loaded %u instance saves.", count );
}

void InstanceSaveManager::PackInstances()
{
    // this routine renumbers player instance associations in such a way so they start from 1 and go up
    // TODO: this can be done a LOT more efficiently

    barGoLink bar( m_saves.size() + 1);
    bar.step();

    uint32 InstanceNumber = 1;
    // we do assume std::set is sorted properly on integer value
    InstanceSaveMap map = m_saves;
    for(InstanceSaveMap::iterator itr = map.begin(); itr != map.end();++itr)
    {
        if (itr->second->GetGUID() != InstanceNumber)
        {
            itr->second->UpdateId(InstanceNumber);
            m_saves.erase(itr->first);
            m_saves.insert(std::make_pair<uint32, InstanceSave*>(InstanceNumber, itr->second));
        }

        ++InstanceNumber;
        bar.step();
    }

    sObjectMgr.SetMaxInstanceId(InstanceNumber);
    sLog.outString( ">> Instance numbers remapped, next instance id is %u", InstanceNumber );
    sLog.outString();
}

//Only for new save
InstanceSave* InstanceSaveManager::CreateInstanceSave(uint16 mapId, uint32 id, Difficulty difficulty, bool perm)
{
    InstanceSave* save = GetInstanceSave(id);
    if(id && save)
        return save;

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

        // delete saves without players
        if(!itr->second->HasPlayers())
        {
            DeleteSave(itr->first);
            continue;
        }

        if(itr->second->GetResetTime() > now) // Ok, not expired
            continue;

        bool hasExtened = itr->second->RemoveOrExtendPlayers();

        //Teleport players out
        Map *map = sMapMgr.FindMap(itr->second->GetMapId(), itr->first);
        if(map)
        {
            if(map->HavePlayers())
            {
                const Map::PlayerList &players_map = map->GetPlayers();
                std::list<Player*> players;
                for(Map::PlayerList::const_iterator itr2 = players_map.begin(); itr2 != players_map.end(); ++itr2)
                {
                    if(itr2->getSource())
                        players.push_back(itr2->getSource());
                }
                for(std::list<Player*>::iterator plr = players.begin(); plr != players.end(); ++plr)
                    (*plr)->RepopAtGraveyard();
                players.clear();
            }
            if(!hasExtened)
                ((InstanceMap*)map)->Reset(INSTANCE_RESET_RESPAWN_DELAY);
        }
        if(!hasExtened)
            DeleteSave(itr->first);
    }
}
