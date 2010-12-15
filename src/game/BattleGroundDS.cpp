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

#include "Player.h"
#include "BattleGround.h"
#include "BattleGroundDS.h"
#include "Language.h"
#include "Object.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"

BattleGroundDS::BattleGroundDS()
{

    m_StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_1M;
    m_StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_30S;
    m_StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_15S;
    m_StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;
    //we must set messageIds
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_ARENA_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_ARENA_THIRTY_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_ARENA_FIFTEEN_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_ARENA_HAS_BEGUN;

    // 2,5 z out platform
    // on platform 5, radius from 1291 791 is 21
    m_fMinZ = 2.5f;
}

BattleGroundDS::~BattleGroundDS()
{

}

void BattleGroundDS::Update(uint32 diff)
{
    BattleGround::Update(diff);
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    // Knocking out of tubes
    if (m_uiKnockTimer < diff && !m_bTubeIsEmpty)
        KnockOutOfTubes();
    else m_uiKnockTimer -= diff;
    
    // Waterfall
    if (m_uiWaterfall < diff)
        HandleWatterfall();
    else m_uiWaterfall -= diff;
}

void BattleGroundDS::StartingEventCloseDoors()
{
}

void BattleGroundDS::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);
}

void BattleGroundDS::KnockOutOfTubes()
{
    //DespawnEvent(DOORS_EVENT, 0);

    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        Player * plr = sObjectMgr.GetPlayer(itr->first);
        float angle = itr->second.Team == ALLIANCE ? 6.05f : 3.03f; 
        if ((plr->GetDistance2d(1214, 765) <= 50 || plr->GetDistance2d(1369, 817) <= 50) && plr->GetPositionZ() > 10)
            plr->KnockWithAngle(angle, 32.85f, 5.8f);

        // Remove Demonic Circle
        if (plr->getClass() == CLASS_WARLOCK)
            if (GameObject* obj = plr->GetGameObject(48018))
                obj->Delete();
    }
    if (GetStartTime() > 90)
        m_bTubeIsEmpty = true;
}

void BattleGroundDS::KnockbackFromWaterfall()
{
    if (!m_WaterfallCollision)
        return;

    uint32 boundingRadius = m_WaterfallCollision->GetObjectBoundingRadius();
    for(BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
    {
        Player * plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr)
            continue;
        
        float angle = m_WaterfallCollision->GetAngle(plr);
        if (!m_WaterfallCollision->GetDistance2d(plr))
            plr->KnockWithAngle(angle, 5.0f, 7.0f);
    }
}

void BattleGroundDS::HandleWatterfall()
{
    // 0 - is despawned
    // 1 - is spawned effect
    // 2 - is spawned collision
    switch(m_uiWaterfallStage)
    {
        case 0:
            //SpawnEvent(WATERFALL_EVENT, 1, true);
            SpawnWaterfall(true);
            m_uiWaterfall = 5000;
            break;
        case 1:
            //SpawnEvent(WATERFALL_EVENT, 0, true);
            SpawnWaterfall(false);
            KnockbackFromWaterfall();
            m_uiWaterfall = 30000;
            break;
        case 2:
            if (m_WaterfallEffect)
                m_WaterfallEffect->Delete();
            if (m_WaterfallCollision)
                m_WaterfallCollision->Delete();
            m_uiWaterfall = 25000;
            break;
    }

    ++m_uiWaterfallStage;
    if (m_uiWaterfallStage >= 3)
        m_uiWaterfallStage -= 3;

}

Player* BattleGroundDS::GetRandomPlayer()
{
    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player * plr = sObjectMgr.GetPlayer(itr->first))
            return plr;

    return NULL;
}

void BattleGroundDS::SpawnWaterfall(bool effect)
{
    Player* plr = GetRandomPlayer();
    if (!plr)
    {
        sLog.outError("BattleGroundDS: No player in map found!");
        return;
    }

    if (effect)
        m_WaterfallEffect = plr->SummonGameobject(191877, 1291.56f, 790.837f, 7.1f, 3.14238f, 604800);
    else
        m_WaterfallCollision = plr->SummonGameobject(194395, 1291.56f, 790.837f, 7.1f, 3.14238f, 604800);

}

void BattleGroundDS::AddPlayer(Player *plr)
{
    BattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    BattleGroundDSScore* sc = new BattleGroundDSScore;

    m_PlayerScores[plr->GetGUID()] = sc;

    UpdateArenaWorldState();
}

void BattleGroundDS::RemovePlayer(Player * /*plr*/, uint64 /*guid*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void BattleGroundDS::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!killer)
    {
        sLog.outError("BattleGroundDS: Killer player not found");
        return;
    }

    BattleGround::HandleKillPlayer(player,killer);

    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void BattleGroundDS::HandleAreaTrigger(Player *Source, uint32 Trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    switch(Trigger)
    {
        case 5347:
        case 5348:
            break;
        default:
            sLog.outError("WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
}
bool BattleGroundDS::HandlePlayerUnderMap(Player *player)
{
    player->TeleportTo(GetMapId(), 1299.046f, 784.825f, 9.338f, 2.422f, false);
    return true;
}

void BattleGroundDS::FillInitialWorldStates(WorldPacket &data, uint32& count)
{
    FillInitialWorldState(data, count, 0xe1a, 1);
    UpdateArenaWorldState();
}

void BattleGroundDS::Reset()
{
    //call parent's class reset
    BattleGround::Reset();
    m_uiKnockTimer = 10000;
    m_bKnocked = false;
    m_uiWaterfallStage = 0;
    m_bTubeIsEmpty = false;
    m_WaterfallEffect = NULL;
    m_WaterfallCollision = NULL;
    m_uiWaterfall = 25000;
}

bool BattleGroundDS::SetupBattleGround()
{
    return true;
}

bool BattleGroundDS::ObjectInLOS(Unit* caster, Unit* target)
{
    // if colision is not spawned, there is no los
    if (!m_WaterfallCollision || !m_WaterfallCollision->IsInWorld())
        return false;

    float angle = caster->GetAngle(target);
    float x_per_i = cos(angle);
    float y_per_i = sin(angle);
    float distance = caster->GetDistance(target);
    float x = caster->GetPositionX();
    float y = caster->GetPositionY();
    float bounding = m_WaterfallCollision->IsWithinBoundingRadius(x,y);
    for (int32 i = 0; i < distance; ++i)
    {
        x += x_per_i;
        y += y_per_i;
        if (!m_WaterfallCollision->GetDistance2d(x,y))
            return true;
    }
    return false;
}