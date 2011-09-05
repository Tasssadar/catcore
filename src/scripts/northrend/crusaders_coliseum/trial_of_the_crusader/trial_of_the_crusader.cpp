/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

/* ScriptData
SDName: Trial Of the crusader
SD%Complete: 60%
SDComment: event script by /dev/rsa
SDCategory: trial_of_the_crusader
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Says
{
    SAY_STAGE_0_01            = -1649070,
    SAY_STAGE_0_02            = -1649071,
    SAY_STAGE_0_03a           = -1649072,
    SAY_STAGE_0_03h           = -1649073,
    SAY_STAGE_0_04            = -1649074,
    SAY_STAGE_0_05            = -1649075,
    SAY_STAGE_0_06            = -1649076,
    SAY_STAGE_0_WIPE          = -1649077,
    SAY_STAGE_1_01            = -1649080,
    SAY_STAGE_1_02            = -1649081,
    SAY_STAGE_1_03            = -1649082,
    SAY_STAGE_1_04            = -1649083,
    SAY_STAGE_1_05            = -1649030, //INTRO Jaraxxus
    SAY_STAGE_1_06            = -1649084,
    SAY_STAGE_1_07            = -1649086,
    SAY_STAGE_1_08            = -1649087,
    SAY_STAGE_1_09            = -1649088,
    SAY_STAGE_1_10            = -1649089,
    SAY_STAGE_1_11            = -1649090,
    SAY_STAGE_2_01            = -1649091,
    SAY_STAGE_2_02a           = -1649092,
    SAY_STAGE_2_02h           = -1649093,
    SAY_STAGE_2_03            = -1649094,
    SAY_STAGE_2_04a           = -1649095,
    SAY_STAGE_2_04h           = -1649096,
    SAY_STAGE_2_05a           = -1649097,
    SAY_STAGE_2_05h           = -1649098,
    SAY_STAGE_2_06            = -1649099,
    SAY_STAGE_3_01            = -1649100,
    SAY_STAGE_3_02            = -1649101,
    SAY_STAGE_3_03a           = -1649102,
    SAY_STAGE_3_03h           = -1649103,
    SAY_STAGE_4_01            = -1649104,
    SAY_STAGE_4_02            = -1649105,
    SAY_STAGE_4_03            = -1649106,
    SAY_STAGE_4_04            = -1649107,
    SAY_STAGE_4_05            = -1649108,
    SAY_STAGE_4_06            = -1649109,
    SAY_STAGE_4_07            = -1649110
};

#define REALLY_BIG_COOLDOWN 3600000

npc_toc_announcerAI::npc_toc_announcerAI(Creature* pCreature) : ScriptedAI(pCreature)
{
    m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    isHeroic = pCreature->GetMap()->IsHeroicRaid();
    encounterCreature = NULL;
    encounterCreature2 = NULL;
    Reset();
}

void npc_toc_announcerAI::Reset()
{
    if (!m_pInstance)
        m_creature->ForcedDespawn();

    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    m_creature->SetRespawnDelay(DAY);
    m_creature->SetVisibility(VISIBILITY_ON);
    m_pInstance->instance->CreatureRelocation(m_creature, SpawnLoc[0], 5.0614f);
    m_TimerMgr->SetUpdatable(false);
    currentEncounter = -1;
    encounterStage = 0;
    if (encounterCreature && encounterCreature->isAlive())
        encounterCreature->ForcedDespawn();
    if (encounterCreature2 && encounterCreature2->isAlive())
        encounterCreature2->ForcedDespawn();
    encounterCreature = NULL;
    encounterCreature2 = NULL;
}

void npc_toc_announcerAI::AttackStart(Unit* /*who*/) { return; }

void npc_toc_announcerAI::MovementInform(uint32 uiType, uint32 uiPointId)
{
    if (uiType == POINT_MOTION_TYPE && uiPointId == POINT_PORT)
    {
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_pInstance->instance->CreatureRelocation(m_creature, SpawnLoc[0], 5.0614f);
    }
}

Creature* npc_toc_announcerAI::DoSpawnTocBoss(uint32 id, Coords coord, float ori)
{
    Creature* pTemp = m_creature->SummonCreature(id, coord, ori, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 300000, true);
    if (pTemp)
        pTemp->SetRespawnDelay(7*DAY);

    return pTemp;
}

void npc_toc_announcerAI::SummonToCBoss(uint32 id, uint32 id2)
{
    Coords coord = SpawnLoc[2];
    Coords coord2;
    if (id2)
    {
        coord2 = SpawnLoc[2];
        coord.x -= 8.f;
        coord2.x += 8.f;
    }
    encounterCreature = DoSpawnTocBoss(id, coord, M_PI_F*1.5f);
    if (id2)
        encounterCreature2 = DoSpawnTocBoss(id2, coord2, M_PI_F*1.5f);

    AddNonCastTimer(TIMER_DOOR_HANDLER, 500, 5000);
}

void npc_toc_announcerAI::ChooseEvent(uint8 encounterId)
{
    if (m_pInstance->GetData(encounterId) == DONE)
        return;

    currentEncounter = encounterId;
    encounterStage = 0;
    uint32 startTimer = 0;
    uint32 runaway = 0;
    switch (encounterId)
    {
        case TYPE_BEASTS:
            startTimer = 1000;
            runaway = 1500;
            break;
        case TYPE_JARAXXUS:
            startTimer = 1500;
            runaway = 2000;
        default:
            break;
    }

    if (startTimer)
    {
        m_TimerMgr->SetUpdatable(true);
        AddNonCastTimer(TIMER_PHASE_HANDLING, startTimer, 0);
        if (runaway)
            AddNonCastTimer(TIMER_RUNAWAY, runaway, 0);
    }
}

void npc_toc_announcerAI::DataSet(uint32 type, uint32 data)
{
    switch (type)
    {
        case TYPE_BEASTS:
        {
            switch(data)
            {
                case FAIL:
                    DoScriptText(SAY_STAGE_0_WIPE, m_pInstance->GetCreature(NPC_TIRION));
                    Reset();
                    break;
                case DONE:
                    DoScriptText(SAY_STAGE_0_06, m_pInstance->GetCreature(NPC_TIRION));
                    Reset();
                    break;
                case GORMOK_DONE:
                case SNAKES_DONE:
                {
                    if ((data == GORMOK_DONE && encounterStage == 4) || (data == SNAKES_DONE && encounterStage == 7))
                    {
                        m_TimerMgr->SetValue(TIMER_PHASE_HANDLING, TIMER_VALUE_CUSTOM, m_TimerMgr->GetValue(TIMER_PHASE_HANDLING, TIMER_VALUE_TIMER));
                        m_TimerMgr->Cooldown(TIMER_PHASE_HANDLING, 1000);
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case TYPE_JARAXXUS:
        {
            switch(data)
            {
                case FAIL:
                    Reset();
                    break;
            }
            // 7 sec po smrti "The loss of" http://www.youtube.com/watch?v=O1ws1DyCVs4&feature=player_embedded#!
            break;
        }
    }
}

void npc_toc_announcerAI::UpdateAI(const uint32 /*diff*/)
{
    // custom event step handling
    if (SpellTimer* stepTimer = m_TimerMgr->TimerFinished(TIMER_PHASE_HANDLING))
    {
        uint32 cooldown = 0;
        ++encounterStage;

        switch(currentEncounter)
        {
            case TYPE_BEASTS:
            {
                switch(encounterStage)
                {
                    case 1:
                        DoScriptText(SAY_STAGE_0_01, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 22000;
                        break;
                    case 2:
                        DoScriptText(SAY_STAGE_0_02, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 10000;
                        break;
                    case 3:
                        SummonToCBoss(NPC_GORMOK);
                        if (m_pInstance->GetInstanceSide() == INSTANCE_SIDE_ALI)
                            DoScriptText(SAY_STAGE_0_03a, m_pInstance->GetCreature(NPC_RINN));
                        else
                            DoScriptText(SAY_STAGE_0_03h, m_pInstance->GetCreature(NPC_GARROSH));

                        //if (isHeroic)
                        //    AddNonCastTimer(TIMER_CUSTOM, 540000, 5000);
                        cooldown = 2000;
                        break;
                    case 4:
                        if (encounterCreature)
                            encounterCreature->AI()->AttackStart(m_pInstance->GetRandomPlayerInMap());
                        cooldown = isHeroic ? 178000 : REALLY_BIG_COOLDOWN;
                        break;
                    case 5:
                        SummonToCBoss(NPC_DREADSCALE);
                        DoScriptText(SAY_STAGE_0_04, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 2000;
                        break;
                    case 6:
                        if (encounterCreature)
                            encounterCreature->AI()->AttackStart(m_pInstance->GetRandomPlayerInMap());
                        cooldown = 5000;
                        break;
                    case 7:
                        if (Player* randPlr = m_pInstance->GetRandomPlayerInMap())
                            if (encounterCreature2 = DoSpawnTocBoss(NPC_ACIDMAW, randPlr->GetPosition(), 0))
                                encounterCreature2->CastSpell(encounterCreature2, SPELL_EMERGE_ACIDMAW, false);
                        cooldown = isHeroic ? 173000 + stepTimer->GetValue(TIMER_VALUE_CUSTOM) : REALLY_BIG_COOLDOWN;
                        break;
                    case 8:
                        SummonToCBoss(NPC_ICEHOWL);
                        DoScriptText(SAY_STAGE_0_05, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 2000;
                    case 9:
                        if (encounterCreature)
                            encounterCreature->AI()->AttackStart(m_pInstance->GetRandomPlayerInMap());
                        cooldown = isHeroic ? 178000 + stepTimer->GetValue(TIMER_VALUE_CUSTOM) : REALLY_BIG_COOLDOWN;
                        break;
                    case 10:
                        if (encounterCreature)
                            encounterCreature->GetTimerMgr()->AddSpellToQueue(SPELL_BERSERK, UNIT_SELECT_SELF);
                        break;
                }
                break;
            }
            case TYPE_JARAXXUS:
            {
                switch(encounterStage)
                {
                    case 1:
                    {
                        if (m_pInstance->GetData(TYPE_JARAXXUS) == FAIL)
                        {
                            if (encounterCreature2 = m_pInstance->GetCreature(NPC_JARAXXUS))
                            {
                                encounterCreature2->RemoveAurasDueToSpell(SPELL_JARAXXUS_CHAINS);
                                encounterCreature2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                ((ScriptedAI*)encounterCreature2->AI())->EnableAttack(true);
                                encounterCreature2->AI()->AttackStart(m_pInstance->GetRandomPlayerInMap());
                            }
                        }
                        else
                        {
                            DoScriptText(SAY_STAGE_1_01, m_pInstance->GetCreature(NPC_TIRION));
                            SummonToCBoss(NPC_FIZZLEBANG);
                            cooldown = 2000;
                        }
                        break;
                    }
                    case 2:
                    {
                        if (!encounterCreature)
                            Reset();

                        encounterCreature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                        encounterCreature->GetMotionMaster()->MovePoint(POINT_TO_CENTER, SpawnLoc[27].x, SpawnLoc[27].y, SpawnLoc[27].z, false);

                        float pathLength = encounterCreature->GetDistance2d(SpawnLoc[27].x, SpawnLoc[27].y);
                        uint32 timeToPoint = uint32(pathLength/(encounterCreature->GetSpeed(MOVE_WALK)*0.001f));

                        m_TimerMgr->Cooldown(TIMER_DOOR_HANDLER, timeToPoint);
                        cooldown = timeToPoint + 1000;
                        break;
                    }
                    case 3:
                        DoScriptText(SAY_STAGE_1_02, encounterCreature);
                        cooldown = 10000;
                        break;
                    case 4:
                        DoScriptText(SAY_STAGE_1_03, encounterCreature);
                        encounterCreature->SummonCreature(NPC_WILFRED_PORTAL_GROUND, SpawnLoc[1], 1.5f*M_PI_F, TEMPSUMMON_TIMED_DESPAWN, 8000);
                        cooldown = 2000;
                        break;
                    case 5:
                    {
                        Coords coord = SpawnLoc[1];
                        coord.z += 2.5f;
                        if (!encounterCreature2 = encounterCreature->SummonCreature(NPC_TRIGGER, coord, 1.5f*M_PI_F, TEMPSUMMON_TIMED_DESPAWN, 6000))
                            Reset();

                        encounterCreature2->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.f);
                        encounterCreature2->CastSpell(encounterCreature2, SPELL_WILFRED_PORTAL, false);
                        cooldown = 3500;
                        break;
                    }
                    case 6:
                        DoScriptText(SAY_STAGE_1_04, encounterCreature);
                        encounterCreature2 = DoSpawnTocBoss(NPC_JARAXXUS, encounterCreature2->GetPosition(), encounterCreature->GetOrientation());
                        cooldown = 500;
                        break;
                    case 7:
                        if (!encounterCreature2)
                            Reset();

                        encounterCreature2->GetMotionMaster()->MovePoint(0, SpawnLoc[29].x, SpawnLoc[29].y, SpawnLoc[29].z, false);
                        cooldown = 1500;
                        break;
                    case 8:
                    {
                        encounterCreature2->SetOrientation(encounterCreature2->GetAngle(encounterCreature));
                        Coords curc = encounterCreature2->GetPosition();
                        encounterCreature2->SetSummonPoint(curc.x, curc.y, curc.z, encounterCreature->GetOrientation());

                        WorldPacket heart;
                        encounterCreature2->BuildHeartBeatMsg(&heart);
                        encounterCreature2->SendMessageToSet(&heart, false);
                        cooldown = 4000;
                        break;
                    }
                    case 9:
                        DoScriptText(SAY_STAGE_1_05, encounterCreature2);
                        cooldown = 3000;
                        break;
                    case 10:
                        DoScriptText(SAY_STAGE_0_06, encounterCreature);
                        cooldown = 700;
                        break;
                    case 11:
                        encounterCreature2->CastSpell(encounterCreature, SPELL_FEL_LIGHTNING_IK, false);
                        cooldown = 2000;
                        break;
                    case 12:
                        DoScriptText(SAY_STAGE_1_07, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 5000;
                        break;
                    case 13:
                        encounterCreature2->RemoveAurasDueToSpell(SPELL_JARAXXUS_CHAINS);
                        encounterCreature2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        ((ScriptedAI*)encounterCreature2->AI())->EnableAttack(true);
                        encounterCreature2->AI()->AttackStart(m_pInstance->GetRandomPlayerInMap());
                        break;
                }
                break;
            }
            default:
                break;
        }
        cat_log("toc_announcer: Phase updating: Handling current encounter %u in encounter stage %u and setting cooldown to %u", currentEncounter, encounterStage, cooldown);

        if (cooldown)
            stepTimer->Cooldown(cooldown);
        else
            stepTimer->SetValue(TIMER_VALUE_DELETE_AT_FINISH, true);
    }

    // open and closes doors
    if (SpellTimer* doorTimer = m_TimerMgr->TimerFinished(TIMER_DOOR_HANDLER))
    {
        uint64 doorGuid = m_pInstance->GetData64(GO_MAIN_GATE_DOOR);
        if (!doorTimer->GetValue(TIMER_VALUE_CUSTOM))
        {
            m_pInstance->OpenDoor(doorGuid);
            doorTimer->SetValue(TIMER_VALUE_CUSTOM, true);
        }
        else
        {
            m_pInstance->CloseDoor(doorGuid);
            doorTimer->SetValue(TIMER_VALUE_DELETE_AT_FINISH, true);
        }
    }

    // runaway of announcer
    if (SpellTimer* runawayTimer = m_TimerMgr->TimerFinished(TIMER_RUNAWAY))
    {
        if (GameObject* go = m_pInstance->GetGameObject(GO_GATE_EAST))
        {
            Coords coord = go->GetPosition();
            m_creature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
            m_creature->SetSpeedRate(MOVE_RUN, 1.2f, true);
            m_creature->GetMotionMaster()->MovePoint(POINT_PORT, coord.x, coord.y, coord.z, false);
        }
        runawayTimer->SetValue(TIMER_VALUE_DELETE_AT_FINISH, true);
    }

    // handling of custom timer in the event
    /*if (SpellTimer* customTimer = m_TimerMgr->TimerFinished(TIMER_CUSTOM))
    {
        if (currentEncounter == TYPE_BEASTS)
        {
            if (encounterCreature && encounterCreature->HasAura(SPELL_BERSERK))
                encounterCreature->GetTimerMgr()->AddSpellToQueue(SPELL_BERSERK, UNIT_SELECT_SELF);
            if (encounterCreature2 && encounterCreature2->HasAura(SPELL_BERSERK))
                encounterCreature2->GetTimerMgr()->AddSpellToQueue(SPELL_BERSERK, UNIT_SELECT_SELF);
        }
        else
            customTimer->SetValue(TIMER_VALUE_DELETE_AT_FINISH, true);
    }*/
}

CreatureAI* GetAI_npc_toc_announcer(Creature* pCreature)
{
    return new npc_toc_announcerAI(pCreature);
};

bool GossipHello_npc_toc_announcer(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    if (!m_pInstance)
        return false;

    bool isHeroic = pCreature->GetMap()->IsHeroicRaid();
    char const* _message = "We are ready!";

    if (!pPlayer->getAttackers().empty() ||
        m_pInstance->IsEncounterInProgress())
        return true;

    uint8 i = 0;
    for(; i < NUM_MESSAGES; i++)
    {
        if (m_pInstance->GetData(i) != DONE )
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _message, GOSSIP_SENDER_MAIN,GOSSIP_ACTION_INFO_DEF+1);
            break;
        }
    };

    pPlayer->SEND_GOSSIP_MENU(MSG_BEASTS+i, pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_toc_announcer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    ScriptedInstance* m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    if (!m_pInstance)
        return false;

    pPlayer->CLOSE_GOSSIP_MENU();

    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1: // used for starting event
        {
            for(uint8 i = 0; i < NUM_MESSAGES; i++)
            {
                if (m_pInstance->GetData(i) != DONE )
                {
                    ((npc_toc_announcerAI*)pCreature->AI())->ChooseEvent(i);
                    break;
                }
            }
            break;
        }
        default:
            break;

    }
    return true;
}

/*struct MANGOS_DLL_DECL boss_lich_king_tocAI : public ScriptedAI
{
    boss_lich_king_tocAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        WayPointList.clear();
        JustRespawned();
    }

    ScriptedInstance* m_pInstance;
    uint32 UpdateTimer;
    uint32 event_state_lich_king;
    bool Event;
    bool MovementStarted;
    std::list<WorldLocation> WayPointList;
    std::list<WorldLocation>::iterator WayPoint;
    uint32 WalkTimer;
    bool IsWalking;
    Creature* pPortal;

    void Reset()
    {
        UpdateTimer = 0;
        event_state_lich_king = 0;
        Event = false;
        MovementStarted = false;
        m_creature->SetRespawnDelay(DAY);
        pPortal = m_creature->SummonCreature(NPC_TRIGGER, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
        pPortal->SetRespawnDelay(DAY);
        pPortal->CastSpell(pPortal, 51807, false);
        pPortal->SetDisplayId(17612);
        if (m_pInstance) m_pInstance->SetData(TYPE_LICH_KING,IN_PROGRESS);
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

    void JustRespawned()
    {
        Reset();
    }

    void MoveInLineOfSight(Unit *who)
    {
    }

    void StartMovement()
    {
        if (!WayPointList.empty() || MovementStarted)
            return;

        AddWaypoint(0, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z);
        AddWaypoint(1, SpawnLoc[17].x, SpawnLoc[17].y, SpawnLoc[17].z);
        AddWaypoint(2, SpawnLoc[18].x, SpawnLoc[18].y, SpawnLoc[18].z);
        m_creature->AddSplineFlag(SPLINEFLAG_WALKMODE);
        WayPoint = WayPointList.begin();
        MovementStarted = true;
        IsWalking = true;
        WalkTimer = 200;
        event_state_lich_king = 1;
        UpdateTimer = m_pInstance->GetData(TYPE_EVENT_TIMER);
    }

    void AddWaypoint(uint32 id, float x, float y, float z)
    {
        WorldLocation loc(id, x,y,z);
        WayPointList.push_back(loc);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_EVENT_NPC) != NPC_LICH_KING_1)
            return;
    }
};

CreatureAI* GetAI_boss_lich_king_toc(Creature* pCreature)
{
    return new boss_lich_king_tocAI(pCreature);
};

struct MANGOS_DLL_DECL npc_fizzlebang_tocAI : public ScriptedAI
{
    npc_fizzlebang_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)m_creature->GetInstanceData();
        Reset();
    }

    InstanceData* m_pInstance;
    uint32 UpdateTimer;
    Creature* pPortal;
    Creature* pTrigger;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        m_creature->GetMotionMaster()->MovePoint(1, SpawnLoc[27].x, SpawnLoc[27].y, SpawnLoc[27].z);
        pPortal = NULL;
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(-1713715, m_creature, pKiller);
        m_pInstance->SetData(TYPE_EVENT, 1180);
        if (pPortal)
            pPortal->ForcedDespawn();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_EVENT_NPC) != NPC_FIZZLEBANG)
            return;

    }
};

CreatureAI* GetAI_npc_fizzlebang_toc(Creature* pCreature)
{
    return new npc_fizzlebang_tocAI(pCreature);
}

struct MANGOS_DLL_DECL npc_tirion_tocAI : public ScriptedAI
{
    npc_tirion_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)m_creature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 UpdateTimer;
    uint32 crusader[12];
    uint8 crusaderscount;

    void Reset()
    {
        crusaderscount = 0;
        memset(&crusader, 0, sizeof(crusader));
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_EVENT_NPC) != NPC_TIRION)
            return;
        
     }
};

CreatureAI* GetAI_npc_tirion_toc(Creature* pCreature)
{
    return new npc_tirion_tocAI(pCreature);
};

struct MANGOS_DLL_DECL npc_garrosh_tocAI : public ScriptedAI
{
    npc_garrosh_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)m_creature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 UpdateTimer;

    void Reset()
    {
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_EVENT_NPC) != NPC_GARROSH)
            return;
        
    }
};

CreatureAI* GetAI_npc_garrosh_toc(Creature* pCreature)
{
    return new npc_garrosh_tocAI(pCreature);
};

struct MANGOS_DLL_DECL npc_rinn_tocAI : public ScriptedAI
{
    npc_rinn_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)m_creature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 UpdateTimer;

    void Reset()
    {
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_EVENT_NPC) != NPC_RINN)
            return;
        
    }
};

CreatureAI* GetAI_npc_rinn_toc(Creature* pCreature)
{
    return new npc_rinn_tocAI(pCreature);
};*/

void AddSC_trial_of_the_crusader()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "npc_toc_announcer";
    NewScript->GetAI = &GetAI_npc_toc_announcer;
    NewScript->pGossipHello = &GossipHello_npc_toc_announcer;
    NewScript->pGossipSelect = &GossipSelect_npc_toc_announcer;
    NewScript->RegisterSelf();

    /*NewScript = new Script;
    NewScript->Name = "boss_lich_king_toc";
    NewScript->GetAI = &GetAI_boss_lich_king_toc;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_fizzlebang_toc";
    NewScript->GetAI = &GetAI_npc_fizzlebang_toc;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_tirion_toc";
    NewScript->GetAI = &GetAI_npc_tirion_toc;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_garrosh_toc";
    NewScript->GetAI = &GetAI_npc_garrosh_toc;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_rinn_toc";
    NewScript->GetAI = &GetAI_npc_rinn_toc;
    NewScript->RegisterSelf();*/
}
