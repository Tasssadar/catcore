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
#include "BattleGroundRV.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "GameObject.h"
#include "Language.h"

BattleGroundRV::BattleGroundRV()
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

    // ele down : floor z 1,5 = actual z ground z = 28.29
    // ele up : floor z 28.28 = actual z ground z = 28.28
    m_fMinZ = 1;
}

BattleGroundRV::~BattleGroundRV()
{

}

void BattleGroundRV::Update(uint32 diff)
{
    BattleGround::Update(diff);

    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!m_bElevatorsUp)
    {
        if (m_bElevatingTimer < diff)
        {
            m_bElevatorsUp = true;
            m_fMinZ = 27.5f;
        }
        else m_bElevatingTimer -= diff;
        
        // elevators are still moving so arena mechanics should not be affected
        return;

    }

    if (m_uiPillarChanging < diff)
        ChangeActivePillars();
    else m_uiPillarChanging -= diff;

    /*if (m_uiTexturesCheck < diff)
    {
        for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
            if (Player* plr = sObjectMgr.GetPlayer(itr->first))
                if (plr->GetPositionZ() < 20.0f)
                    HandlePlayerUnderMap(plr);
        m_uiTexturesCheck = 10*IN_MILLISECONDS;
    }else m_uiTexturesCheck -= diff;*/
}

void BattleGroundRV::StartingEventCloseDoors()
{
}

void BattleGroundRV::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);

    // put Pillars into list
    for(uint8 event2 = BATTLEGROUND_RV_SUBEVENT_PILLARS_FAR; event2 <= BATTLEGROUND_RV_SUBEVENT_PILLARS_NEAR; ++event2)
        for (BGObjects::const_iterator itr = m_EventObjects[MAKE_PAIR32(BATTLEGROUND_RV_EVENT_PILLARS, event2)].gameobjects.begin();
            itr != m_EventObjects[MAKE_PAIR32(BATTLEGROUND_RV_EVENT_PILLARS, event2)].gameobjects.end(); ++itr)
        {
            GameObject *obj = GetBgMap()->GetGameObject(*itr);
            if (!obj)
                continue;
            m_lPillars.push_back(obj);
        }
        
    // teleport players few yards above
    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player* plr = sObjectMgr.GetPlayer(itr->first))
            plr->TeleportTo(plr->GetMapId(), plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ()+5.0f, plr->GetOrientation(), TELE_TO_NOT_LEAVE_TRANSPORT);
}

void BattleGroundRV::ChangeActivePillars()
{
    for(uint8 event1 = BATTLEGROUND_RV_EVENT_PILLARS; event1 <= BATTLEGROUND_RV_EVENT_PILLARS_EQUIPMENT; ++event1)
        for(uint8 event2 = BATTLEGROUND_RV_SUBEVENT_PILLARS_FAR; event2 <= BATTLEGROUND_RV_SUBEVENT_PILLARS_NEAR; ++event2)
            ClickEvent(event1, event2);

    m_uiPillarChanging = urand(25,35)*IN_MILLISECONDS;
}

void BattleGroundRV::ClickEvent(uint8 event1, uint8 event2 /*=0*/)
{
    BGObjects::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.begin();
    for(; itr != m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.end(); ++itr)
        ClickOnPillar(*itr);
}

void BattleGroundRV::AddPlayer(Player *plr)
{
    if (!plr->IsSpectator())
        if (ShouldBePlayerSpectator(plr))
            plr->SetSpectator(true);

    if(!plr->IsSpectator())
    {
        BattleGround::AddPlayer(plr);
        //create score and add it to map, default values are set in constructor
        BattleGroundRVScore* sc = new BattleGroundRVScore;

        m_PlayerScores[plr->GetGUID()] = sc;
    }
    UpdateArenaWorldState();
}

void BattleGroundRV::RemovePlayer(Player * /*plr*/, uint64 /*guid*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

void BattleGroundRV::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!killer)
    {
        sLog.outError("BattleGroundRV: Killer player not found");
        return;
    }

   BattleGround::HandleKillPlayer(player, killer);

    UpdateArenaWorldState();
    CheckArenaWinConditions();
}

bool BattleGroundRV::HandlePlayerUnderMap(Player *player)
{
    player->TeleportTo(GetMapId(), 763.5f, -284, 28.276f, player->GetOrientation(), false);
    return true;
}

void BattleGroundRV::HandleAreaTrigger(Player * Source, uint32 Trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    switch(Trigger)
    {
        case 5224:
        case 5226:
        case 5473:
        case 5474:
            break;
        case 5447:
            HandlePlayerUnderMap(Source);
            sLog.outError("BattleGroundRV:: Player %s is under map due to trigger %u", Source->GetName(), Trigger);
            break;
        default:
            sLog.outError("WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
}

void BattleGroundRV::Reset()
{
    //call parent's class reset
    BattleGround::Reset();
    //m_uiTeleport = 22000;
    m_uiPillarChanging = urand(25,35)*IN_MILLISECONDS;
    //m_uiTexturesCheck = 10*IN_MILLISECONDS;
    m_bElevatorsUp = false;
    m_bElevatingTimer = BATTLEGROUND_RV_ELEVATING_TIME;
}

void BattleGroundRV::FillInitialWorldStates(WorldPacket &data, uint32& count)
{
    FillInitialWorldState(data, count, 0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
    FillInitialWorldState(data, count, 0xe10, GetAlivePlayersCountByTeam(HORDE));
    FillInitialWorldState(data, count, 0xe1a, 1);
}

bool BattleGroundRV::SetupBattleGround()
{
    return true;
}

bool BattleGroundRV::ObjectInLOS(Unit* caster, Unit* target)
{
    // caster or target is on pillar
    if (caster->GetPositionZ() > 30.0f || target->GetPositionZ() > 30.0f)
        return false;

    float angle = caster->GetAngle(target);
    float x_per_i = cos(angle);
    float y_per_i = sin(angle);
    float distance = caster->GetDistance(target);
    float x = caster->GetPositionX();
    float y = caster->GetPositionY();
    for (int32 i = 0; i < distance; ++i)
    {
        x += x_per_i;
        y += y_per_i;
        for(std::list<GameObject*>::iterator itr = m_lPillars.begin(); itr != m_lPillars.end();++itr)
            if ((*itr) && (*itr)->GetGoState() == GO_STATE_ACTIVE && (*itr)->IsWithinBoundingRadius(x,y))
                return true;
    }
    return false;
}

void BattleGroundRV::ClickOnPillar(uint64 const& guid)
{
    GameObject *obj = GetBgMap()->GetGameObject(guid);
    if (obj)
    {
       //change state to be sure they will be opened
        obj->SetLootState(GO_READY);
        obj->UseDoorOrButton(RESPAWN_ONE_DAY);
        for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
            if (Player * plr = sObjectMgr.GetPlayer(itr->first))
                obj->SendCreateUpdateToPlayer(plr);
    }
    else
        sLog.outError("BattleGround: Pillar object not found!");
}

bool BattleGroundRV::IsXYZPositionOK(float x, float y, float z)
{
    // is xyz near active pillar
    for(std::list<GameObject*>::iterator itr = m_lPillars.begin(); itr != m_lPillars.end();++itr)
        if ((*itr) && (*itr)->GetGoState() == GO_STATE_ACTIVE && (*itr)->IsWithinBoundingRadius(x,y))
            return false;

    return true;
}
