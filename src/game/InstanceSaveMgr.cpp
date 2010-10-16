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

InstanceSave::InstanceSave(uint16 MapId, uint64 InstanceId, Difficulty difficulty, bool perm)
{
}

InstanceSave::~InstanceSave()
{
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
    //                                                    0   1    2           
    QueryResult *result = CharacterDatabase.Query("SELECT id, map, difficulty FROM instance");

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

        InstanceSave *save = new InstanceSave(fields[1].GetUInt16(), fields[0].GetUInt64(), Difficulty(fields[3].GetUInt8()));
        
        DungeonInfo *info = new DungeonInfo();
        info->ID                      = fields[0].GetUInt32();
        info->name                    = fields[1].GetCppString();
        info->lastBossId              = fields[2].GetUInt32();
        info->start_map               = fields[3].GetUInt32();
        info->start_x                 = fields[4].GetFloat();
        info->start_y                 = fields[5].GetFloat();
        info->start_z                 = fields[6].GetFloat();
        info->start_o                 = fields[7].GetFloat();
        info->locked                  = fields[8].GetBool();
       
        if (!sLFGDungeonStore.LookupEntry(info->ID))
        {
            sLog.outErrorDb("Entry listed in 'lfg_dungeon_info' has non-exist LfgDungeon.dbc id %u, skipping.", info->ID);
            delete info;
            continue;
        }
        if (!sObjectMgr.GetCreatureTemplate(info->lastBossId) && info->lastBossId != 0)
        {
            sLog.outErrorDb("Entry listed in 'lfg_dungeon_info' has non-exist creature_template entry %u, skipping.", info->lastBossId);
            delete info;
            continue;   
        }
        m_dungeonInfoMap.find(info->ID)->second = info;
        ++count;
    } while( result->NextRow() );

    delete result;

    sLog.outString();
    sLog.outString( ">> Loaded %u instance saves.", count );
}

void InstanceSaveManager::Update()
{
}