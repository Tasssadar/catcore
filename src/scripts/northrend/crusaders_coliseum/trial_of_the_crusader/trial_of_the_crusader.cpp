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

enum
{
    TIMER_PHASE_HANDLING = 0,
    TIMER_DOOR_HANDLER,

    POINT_PORT = 100,

    NUM_MESSAGES = 5,

    SPELL_WILFRED_PORTAL        = 68424,
    SPELL_JARAXXUS_CHAINS       = 67924,
    SPELL_EMERGE_ACIDMAW        = 66947,

    SPELL_BERSERK               = 26662
};

#define REALLY_BIG_COOLDOWN 3600000

struct MANGOS_DLL_DECL npc_toc_announcerAI : public ScriptedAI
{
    npc_toc_announcerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        isHeroic = pCreature->GetMap()->IsHeroicRaid();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool isHeroic;

    int32       currentEncounter;
    uint16      encounterStage;
    Creature*   encounterCreature;
    Creature*   encounterCreature2;

    void Reset()
    {
        if (!m_pInstance)
            m_creature->ForcedDespawn();

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetRespawnDelay(DAY);
        m_creature->SetVisibility(VISIBILITY_ON);
        m_TimerMgr->SetUpdatable(false);
        currentEncounter = -1;
        encounterStage = 0;
        encounterCreature = NULL;
        encounterCreature2 = NULL;
    }

    void AttackStart(Unit* /*who*/) { return; }

    void ChooseEvent(uint8 encounterId)
    {
        if (m_pInstance->GetData(encounterId) == DONE)
            return;

        currentEncounter = encounterId;
        uint32 startTimer = 0;
        switch (encounterId)
        {
            case TYPE_BEASTS:
                startTimer = 1000;
                break;
            default:
                break;
        }

        if (startTimer)
        {
            AddNonCastTimer(TIMER_PHASE_HANDLING, startTimer, 0);
            m_TimerMgr->SetUpdatable(true);
        }
    }

    void DataSet(uint32 type, uint32 data)
    {
        switch (type)
        {
            case TYPE_BEASTS:
            {
                if (data == FAIL)
                    DoScriptText(SAY_STAGE_0_WIPE, m_pInstance->GetCreature(NPC_TIRION));
                else if (data == DONE)
                    DoScriptText(SAY_STAGE_0_06, m_pInstance->GetCreature(NPC_TIRION));
                break;
            }
        }

        if (data == NOT_STARTED || data == FAIL || data == DONE)
            Reset();

        else if (data == IN_PROGRESS)
        {
            if (GameObject* go = m_pInstance->GetGameObject(GO_GATE_EAST))
            {
                Coords coord = go->GetPosition();
                m_creature->GetMotionMaster()->MovePoint(POINT_PORT, coord.x, coord.y, coord.z);
            }
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType == POINT_MOTION_TYPE && uiPointId == POINT_PORT)
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
            m_pInstance->instance->CreatureRelocation(m_creature, SpawnLoc[0], 5.0614f);
        }
    }

    Creature* DoSpawnTocBoss(uint32 id, Coords coord, float ori)
    {
        Creature* pTemp = m_creature->SummonCreature(id, coord, ori, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000, true);
        if (pTemp)
            pTemp->SetRespawnDelay(7*DAY);

        return pTemp;
    }

    void SummonToCBoss(uint32 id, uint32 id2 = 0)
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

        AddNonCastTimer(TIMER_DOOR_HANDLER, 500, 10000);
    }

    Player* GetRandomPlayerInMap()
    {
        Map::PlayerList const &players = m_creature->GetMap()->GetPlayers();
        if (players.isEmpty())
            return NULL;

        Map::PlayerList::const_iterator i = players.begin();
        std::advance(i, urand(0, players.getSize()-1));
        return i->getsource;
    }

    bool isAllianceRaid()
    {
        if (Player* plr = GetRandomPlayerInMap())
            return plr->GetTeam() == ALLIANCE;

        return false;
    }

    void UpdateAI(const uint32 /*diff*/)
    {
        // open and closes doors
        if (SpellTimer* doorTimer = m_TimerMgr->TimerFinished(TIMER_DOOR_HANDLER))
        {
            uint32 doorGuid = m_pInstance->GetData(GO_MAIN_GATE_DOOR);
            if (!doorTimer->GetValue(TIMER_VALUE_CUSTOM))
            {
                m_pInstance->OpenDoor(doorGuid);
                doorTimer->SetValue(TIMER_VALUE_CUSTOM, true);
            }
            else
            {
                m_pInstance->CloseDoor(doorGuid);
                doorTimer->SetValue(TIMER_VALUE_DELETE_AT_FINISH);
            }
        }

        // custom event step handling
        if (SpellTimer* stepTimer = m_TimerMgr->TimerFinished(TIMER_PHASE_HANDLING))
        {
            uint32 cooldown = 0;
            if (currentEncounter == TYPE_BEASTS)
            {
                switch(encounterStage)
                {
                case 0:
                    DoScriptText(SAY_STAGE_0_01, m_pInstance->GetCreature(NPC_TIRION));
                    cooldown = 21000;
                    break;
                case 1:
                    DoScriptText(SAY_STAGE_0_02, m_pInstance->GetCreature(NPC_TIRION));
                    cooldown = 10000;
                    break;
                case 2:
                    SummonToCBoss(NPC_GORMOK);
                    uint32 textId = isAllianceRaid() ? SAY_STAGE_0_03a : SAY_STAGE_0_03h;
                    DoScriptText(textId, m_pInstance->GetCreature(isAllianceRaid() ? NPC_TIRION : NPC_GARROSH));
                    cooldown = 1000;
                    break;
                case 3:
                    encounterCreature->AI()->AttackStart(GetRandomPlayerInMap());
                    cooldown = isHeroic ? 179000 : REALLY_BIG_COOLDOWN;
                    break;
                case 4:
                    SummonToCBoss(NPC_DREADSCALE);
                    DoScriptText(SAY_STAGE_0_04, m_pInstance->GetCreature(NPC_TIRION));
                    cooldown = 1000;
                    break;
                case 5:
                    encounterCreature->AI()->AttackStart(GetRandomPlayerInMap());
                    cooldown = 5000;
                    break;
                case 6:
                    Player* randPlr = GetRandomPlayerInMap();
                    if (randPlr)
                        if (encounterCreature2 = DoSpawnTocBoss(NPC_ACIDMAW, randPlr->GetPosition(), 0))
                            encounterCreature2->CastSpell(encounterCreature2, SPELL_EMERGE_ACIDMAW, true);
                    cooldown = isHeroic ? 174000 : REALLY_BIG_COOLDOWN;
                    break;
                case 7:
                    SummonToCBoss(NPC_ICEHOWL);
                    DoScriptText(SAY_STAGE_0_05, m_pInstance->GetCreature(NPC_TIRION));
                    cooldown = 1000;
                case 8:
                    encounterCreature->AI()->AttackStart(GetRandomPlayerInMap());
                    cooldown = isHeroic ? 179000 : REALLY_BIG_COOLDOWN;
                    break;
                case 9:
                    encounterCreature->GetTimerMgr()->AddSpellToQueue(SPELL_BERSERK, UNIT_SELECT_SELF);
                    break;
                default:
                    break;
                }

            }

            ++encounterStage;
            if (cooldown)
                stepTimer->Cooldown(cooldown);
            else
                stepTimer->SetValue(TIMER_VALUE_DELETE_AT_FINISH, true);
        }
    }
};

CreatureAI* GetAI_npc_toc_announcer(Creature* pCreature)
{
    return new npc_toc_announcerAI(pCreature);
}

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

struct MANGOS_DLL_DECL boss_lich_king_tocAI : public ScriptedAI
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
        
        if (!MovementStarted)
            StartMovement();

        if (IsWalking && WalkTimer)
        {
            if (WalkTimer <= diff)
            {
                if (WayPoint != WayPointList.end())
                {
                    m_creature->GetMotionMaster()->MovePoint(WayPoint->mapid, WayPoint->x(), WayPoint->y(), WayPoint->z());
                    WalkTimer = 0;
                }
            }else WalkTimer -= diff;
        }

        UpdateTimer = m_pInstance->GetData(TYPE_EVENT_TIMER);

        if (UpdateTimer <= diff)
        {
            switch (m_pInstance->GetData(TYPE_EVENT))
            {
                case 5010:
                {
                    DoScriptText(-1713550,m_creature);
                    UpdateTimer = 3000;
                    m_pInstance->SetData(TYPE_EVENT,5020);
                    break;
                }
                case 5030:
                {
                    DoScriptText(-1713552,m_creature);
                    m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
                    UpdateTimer =  10000;
                    m_pInstance->SetData(TYPE_EVENT,5040);
                    break;
                }
                case 5040:
                {
                    m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                    UpdateTimer =  1000;
                    m_pInstance->SetData(TYPE_EVENT,5050);
                    break;
                }
                case 5050:
                {
                   m_creature->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                   UpdateTimer =  3000;
                   m_pInstance->SetData(TYPE_EVENT,5060);
                   break;
                }
                case 5060:
                {
                    if (Event)
                    {
                        DoScriptText(-1713553,m_creature);
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                        UpdateTimer =  2500;
                        m_pInstance->SetData(TYPE_EVENT,5070);
                    }
                    break;
                }
                case 5070:
                {
                    m_creature->CastSpell(m_creature,68198,false);
                    UpdateTimer = 1500;
                    m_pInstance->SetData(TYPE_EVENT,5080);
                    break;
                }
                case 5080:
                {
                    if (GameObject* pGoFloor = m_pInstance->instance->GetGameObject(m_pInstance->GetData64(GO_ARGENT_COLISEUM_FLOOR)))
                    {
                        pGoFloor->SetUInt32Value(GAMEOBJECT_DISPLAYID,9060);
                        pGoFloor->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NODESPAWN);
                        pGoFloor->SetUInt32Value(GAMEOBJECT_BYTES_1,8449);
                    }
                    m_creature->CastSpell(m_creature,69016,false);
                    if (m_pInstance)
                        m_pInstance->SetData(TYPE_LICH_KING,DONE);

                   m_pInstance->SetData(TYPE_ANUBARAK,IN_PROGRESS);
                   m_creature->SummonCreature(NPC_ANUBARAK, SpawnLoc[19].x, SpawnLoc[19].y, SpawnLoc[19].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                   if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_ANUBARAK)))
                   {
                       pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[20].x, SpawnLoc[20].y, SpawnLoc[20].z);
                       pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                       pTemp->SetInCombatWithZone();
                   }
                   m_pInstance->SetData(TYPE_STAGE,9);
                   Event=false;
                   m_creature->ForcedDespawn();
                   pPortal->ForcedDespawn();
                   m_pInstance->SetData(TYPE_EVENT,0);
                   UpdateTimer = 20000;
                   break;
                }
            }
        }else UpdateTimer -= diff;

        m_pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (m_pInstance && id == 2)
            Event = true;
        
        if (type != POINT_MOTION_TYPE || WayPoint->mapid != id)
            return;

        ++WayPoint;
        WalkTimer = 200;
    }
};

CreatureAI* GetAI_boss_lich_king_toc(Creature* pCreature)
{
    return new boss_lich_king_tocAI(pCreature);
}

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
        
        UpdateTimer = m_pInstance->GetData(TYPE_EVENT_TIMER);
        
        if (UpdateTimer <= diff)
        {
            switch(m_pInstance->GetData(TYPE_EVENT))
            {
                case 1110:
                {
                    m_pInstance->SetData(TYPE_EVENT, 1120);
                    UpdateTimer = 3000;
                    m_pInstance->SetData(TYPE_JARAXXUS,IN_PROGRESS);
                    break;
                }
                case 1120:
                {
                    DoScriptText(-1713511, m_creature);
                    m_pInstance->SetData(TYPE_EVENT, 1130);
                    UpdateTimer = 12000;
                    break;
                }
                case 1130:
                {
                    m_creature->GetMotionMaster()->MovementExpired();
                    m_creature->HandleEmoteCommand(EMOTE_STATE_SPELL_CHANNEL_OMNI);
                    pPortal = m_creature->SummonCreature(NPC_WILFRED_PORTAL, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 5, TEMPSUMMON_MANUAL_DESPAWN, 5000);
                    if (pPortal)
                    {
                        pPortal->SetRespawnDelay(DAY);
                        pPortal->SetDisplayId(22862);
                    }
                    DoScriptText(-1713512, m_creature);
                    m_pInstance->SetData(TYPE_EVENT, 1132);
                    UpdateTimer = 4000;
                    break;
                }
                case 1132:
                {
                    m_creature->GetMotionMaster()->MovementExpired();
                    if (pPortal)
                        pPortal->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.5f);
                    m_pInstance->SetData(TYPE_EVENT, 1134);
                    UpdateTimer = 4000;
                    break;
                }
                case 1134:
                {
                    if (pPortal)
                        pPortal->SetDisplayId(15900);
                    pTrigger =  m_creature->SummonCreature(NPC_TRIGGER, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 5.0f, TEMPSUMMON_MANUAL_DESPAWN, 5000);
                    if (pTrigger)
                    {
                        pTrigger->SetDisplayId(17612);
                        pTrigger->CastSpell(pTrigger, SPELL_WILFRED_PORTAL, false);
                        pTrigger->SetRespawnDelay(DAY);
                    }
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_SPELLCAST_OMNI);
                    UpdateTimer = 4000;
                    m_pInstance->SetData(TYPE_EVENT, 1135);
                    break;
                }
                case 1135:
                {
                    if (pTrigger)
                        pTrigger->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.0f);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_SPELLCAST_OMNI);
                    UpdateTimer = 3000;
                    m_pInstance->SetData(TYPE_EVENT, 1140);
                    break;
                }
                case 1140:
                {
                    m_pInstance->SetData(TYPE_STAGE,4);
                    m_creature->SummonCreature(NPC_JARAXXUS, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_JARAXXUS)))
                    {
                        pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pTemp->CastSpell(pTemp, SPELL_JARAXXUS_CHAINS, false);
                    }
                    m_pInstance->SetData(TYPE_EVENT, 1142);
                    UpdateTimer = 5000;
                    break;
                }
                case 1142:
                {
                    UpdateTimer = 5000;
                    m_pInstance->SetData(TYPE_EVENT, 1144);
                    DoScriptText(-1713513, m_creature);
                    break;
                }
                case 1144:
                {
                    if (pTrigger)
                        pTrigger->ForcedDespawn();
                    m_pInstance->SetData(TYPE_EVENT, 1150);
                    UpdateTimer = 5000;
                    break;
                }
                case 1150:
                {
                      if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_JARAXXUS)))
                      {
                          pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                          pTemp->RemoveAurasDueToSpell(SPELL_JARAXXUS_CHAINS);
                          pTemp->SetInCombatWithZone();
                          m_creature->SetInCombatWith(pTemp);
                          pTemp->AddThreat(m_creature, 1000.0f);
                          pTemp->AI()->AttackStart(m_creature);
                      }
                      DoScriptText(-1713515, m_creature);
                      m_pInstance->SetData(TYPE_EVENT, 1160);
                      UpdateTimer = 3000;
                      break;
                }
                case 1160:
                {
                    m_pInstance->SetData(TYPE_EVENT, 1170);
                    UpdateTimer = 1000;
                    break;
                }
            }
        } else UpdateTimer -= diff;
        m_pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
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
        
        UpdateTimer = m_pInstance->GetData(TYPE_EVENT_TIMER);

        if (UpdateTimer <= diff)
        {
            switch (m_pInstance->GetData(TYPE_EVENT))
            {
                case 110:
                   m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                   DoScriptText(-1713500, m_creature);
                   UpdateTimer = 12000;
                   m_pInstance->SetData(TYPE_EVENT,120);
    //               m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_WEST_PORTCULLIS));
                   break;
                case 140:
                   m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                   DoScriptText(-1713501, m_creature);
                   UpdateTimer = 10000;
                   m_pInstance->SetData(TYPE_EVENT,150);
                   m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_MAIN_GATE_DOOR));
                   break;
                case 150:
                    m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                    if (m_pInstance->GetData(TYPE_BEASTS) != DONE)
                    {
                        m_creature->SummonCreature(NPC_GORMOK, SpawnLoc[26].x, SpawnLoc[26].y, SpawnLoc[26].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_GORMOK)))
                        {
                            pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                            pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                            pTemp->SetInCombatWithZone();
                        }
                    }
                    UpdateTimer = 10000;
                    m_pInstance->SetData(TYPE_EVENT,160);
                    m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_MAIN_GATE_DOOR));
                    m_pInstance->SetData(TYPE_STAGE,1);
                    m_pInstance->SetData(TYPE_BEASTS,IN_PROGRESS);
                    break;

            case 200:
                DoScriptText(-1713503, m_creature);
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,205);
                break;
            case 205:
                UpdateTimer = 8000;
                m_pInstance->SetData(TYPE_EVENT,210);
                m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_MAIN_GATE_DOOR));
                break;

            case 210:
                if (m_pInstance->GetData(TYPE_BEASTS) != DONE)
                {
                    m_creature->SummonCreature(NPC_DREADSCALE, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    m_creature->SummonCreature(NPC_ACIDMAW, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_DREADSCALE)))
                    {
                        pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                        pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                        pTemp->SetInCombatWithZone();
                    }
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_ACIDMAW)))
                    {
                        pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                        pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                        pTemp->SetInCombatWithZone();
                    }
                }
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,220);
                m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_MAIN_GATE_DOOR));
                break;
            case 300:
                DoScriptText(-1713505, m_creature);
                UpdateTimer = 15000;
                m_pInstance->SetData(TYPE_EVENT,305);
                break;
            case 305:
                UpdateTimer = 8000;
                m_pInstance->SetData(TYPE_EVENT,310);
                m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_MAIN_GATE_DOOR));
                break;
            case 310:
                if (m_pInstance->GetData(TYPE_BEASTS) != DONE)
                {
                    m_creature->SummonCreature(NPC_ICEHOWL, SpawnLoc[26].x, SpawnLoc[26].y, SpawnLoc[26].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_ICEHOWL)))
                    {
                        pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                        pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                        pTemp->SetInCombatWithZone();
                    }
                }
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,320);
                m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_MAIN_GATE_DOOR));
                break;
            case 400:
                DoScriptText(-1713509, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,0);
    //               m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_WEST_PORTCULLIS));
                break;
            case 666:
                DoScriptText(-1713709, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,0);
    //               m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_WEST_PORTCULLIS));
                break;
            case 1010:
                DoScriptText(-1713510, m_creature);
                UpdateTimer = 5000;
                m_creature->SummonCreature(NPC_FIZZLEBANG, SpawnLoc[21].x, SpawnLoc[21].y, SpawnLoc[21].z, 2, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                m_pInstance->SetData(TYPE_EVENT,1110);
                break;
            case 1180:
                DoScriptText(-1713516, m_creature);
                UpdateTimer = 3000;
                m_pInstance->SetData(TYPE_EVENT,0);
                break;
            case 2000:
                DoScriptText(-1713526, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,2010);
                break;
            case 2030:
                DoScriptText(-1713529, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,0);
                break;
            case 3000:
                DoScriptText(-1713530, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,3050);
                break;
            case 3001:
                DoScriptText(-1713530, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,3051);
                break;
            case 3060:
                DoScriptText(-1713532, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,3070);
                break;
            case 3061:
                DoScriptText(-1713532, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,3071);
                break;
            //Summoning crusaders
            case 3091:
                m_pInstance->SetData(TYPE_STAGE,6);
                // 25 man
                if (m_pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_NORMAL ||
                    m_pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_HEROIC)
                {
                    crusaderscount = 12;
                    switch (urand(0,3))
                    {                                       // Healers, 3 in 25-mode
                        case 0:
                            crusader[0] = NPC_CRUSADER_1_1;
                            crusader[1] = NPC_CRUSADER_1_12;
                            crusader[2] = NPC_CRUSADER_1_13;
                            break;
                        case 1:
                            crusader[0] = NPC_CRUSADER_1_1;
                            crusader[1] = NPC_CRUSADER_1_2;
                            crusader[2] = NPC_CRUSADER_1_13;
                            break;
                        case 2:
                            crusader[0] = NPC_CRUSADER_1_1;
                            crusader[1] = NPC_CRUSADER_1_2;
                            crusader[2] = NPC_CRUSADER_1_12;
                            break;
                        case 3:
                            crusader[0] = NPC_CRUSADER_1_2;
                            crusader[1] = NPC_CRUSADER_1_12;
                            crusader[2] = NPC_CRUSADER_1_13;
                            break;
                    }
                    switch (urand(0,5))
                    {                                       // Random melee DD, 2 in 25-mode
                        case 0:
                            crusader[3] = NPC_CRUSADER_1_3;
                            crusader[4] = NPC_CRUSADER_1_4;
                            break;
                        case 1:
                            crusader[3] = NPC_CRUSADER_1_3;
                            crusader[4] = NPC_CRUSADER_1_5;
                            break;
                        case 2:
                            crusader[3] = NPC_CRUSADER_1_3;
                            crusader[4] = NPC_CRUSADER_1_6;
                            break;
                        case 3:
                            crusader[3] = NPC_CRUSADER_1_4;
                            crusader[4] = NPC_CRUSADER_1_5;
                            break;
                        case 4:
                            crusader[3] = NPC_CRUSADER_1_4;
                            crusader[4] = NPC_CRUSADER_1_6;
                            break;
                        case 5:
                            crusader[3] = NPC_CRUSADER_1_5;
                            crusader[4] = NPC_CRUSADER_1_6;
                            break;
                    }
                    switch (urand(0,3))
                    {                                       // Random magic DD, 3 in 25-mode
                        case 0:
                            crusader[5] = NPC_CRUSADER_1_7;
                            crusader[6] = NPC_CRUSADER_1_8;
                            crusader[7] = NPC_CRUSADER_1_11;
                            break;
                        case 1:
                            crusader[5] = NPC_CRUSADER_1_7;
                            crusader[6] = NPC_CRUSADER_1_8;
                            crusader[7] = NPC_CRUSADER_1_14;
                            break;
                        case 2:
                            crusader[5] = NPC_CRUSADER_1_8;
                            crusader[6] = NPC_CRUSADER_1_11;
                            crusader[7] = NPC_CRUSADER_1_14;
                            break;
                        case 3:
                            crusader[5] = NPC_CRUSADER_1_7;
                            crusader[6] = NPC_CRUSADER_1_11;
                            crusader[7] = NPC_CRUSADER_1_14;
                            break;
                    }
                    
                    crusader[8]  = NPC_CRUSADER_1_9;  //Hunter+warlock
                    crusader[9]  = NPC_CRUSADER_1_10;
                    crusader[10] = NPC_CRUSADER_0_1;
                    crusader[11] = NPC_CRUSADER_0_2;
                }
                // 10 man
                else
                {
                    crusaderscount = 6;
                    switch (urand(0,5))
                    {                                       // Healers, 2 in 10-mode
                        case 0:
                            crusader[0] = NPC_CRUSADER_1_1;
                            crusader[1] = NPC_CRUSADER_1_12;
                            break;
                        case 1:
                            crusader[0] = NPC_CRUSADER_1_1;
                            crusader[1] = NPC_CRUSADER_1_2;
                            break;
                        case 2:
                            crusader[0] = NPC_CRUSADER_1_2;
                            crusader[1] = NPC_CRUSADER_1_12;
                            break;
                        case 3:
                            crusader[0] = NPC_CRUSADER_1_1;
                            crusader[1] = NPC_CRUSADER_1_13;
                            break;
                        case 4:
                            crusader[0] = NPC_CRUSADER_1_2;
                            crusader[1] = NPC_CRUSADER_1_13;
                            break;
                        case 5:
                            crusader[0] = NPC_CRUSADER_1_12;
                            crusader[1] = NPC_CRUSADER_1_13;
                            break;
                    }
                    switch (urand(0,5))
                    {                                       // Random melee DD, 2 in 10-mode
                        case 0:
                            crusader[3] = NPC_CRUSADER_1_3;
                            crusader[2] = NPC_CRUSADER_1_4;
                            break;
                        case 1:
                            crusader[3] = NPC_CRUSADER_1_3;
                            crusader[2] = NPC_CRUSADER_1_5;
                            break;
                        case 2:
                            crusader[3] = NPC_CRUSADER_1_3;
                            crusader[2] = NPC_CRUSADER_1_6;
                            break;
                        case 3:
                            crusader[3] = NPC_CRUSADER_1_4;
                            crusader[2] = NPC_CRUSADER_1_5;
                            break;
                        case 4:
                            crusader[3] = NPC_CRUSADER_1_4;
                            crusader[2] = NPC_CRUSADER_1_6;
                            break;
                        case 5:
                            crusader[3] = NPC_CRUSADER_1_5;
                            crusader[2] = NPC_CRUSADER_1_6;
                            break;
                    }
                    switch (urand(0,5))
                    {                                       // Random magic DD, 2 in 10-mode
                        case 0:
                            crusader[4] = NPC_CRUSADER_1_7;
                            crusader[5] = NPC_CRUSADER_1_8;
                            break;
                        case 1:
                            crusader[5] = NPC_CRUSADER_1_7;
                            crusader[4] = NPC_CRUSADER_1_14;
                            break;
                        case 2:
                            crusader[5] = NPC_CRUSADER_1_7;
                            crusader[4] = NPC_CRUSADER_1_11;
                            break;
                        case 3:
                            crusader[5] = NPC_CRUSADER_1_8;
                            crusader[4] = NPC_CRUSADER_1_11;
                            break;
                        case 4:
                            crusader[5] = NPC_CRUSADER_1_8;
                            crusader[4] = NPC_CRUSADER_1_14;
                            break;
                        case 5:
                            crusader[5] = NPC_CRUSADER_1_11;
                            crusader[4] = NPC_CRUSADER_1_14;
                            break;
                    }
                }
                for(uint8 i = 0; i < crusaderscount; ++i)
                {
                    m_creature->SummonCreature(crusader[i], SpawnLoc[i+2].x, SpawnLoc[i+2].y, SpawnLoc[i+2].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(crusader[i])))
                    {
                        pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                        pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                    }
                }
                
                m_pInstance->SetData(TYPE_CRUSADERS_COUNT,crusaderscount);
                UpdateTimer = 3000;
                m_pInstance->SetData(TYPE_EVENT,0);
                m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_MAIN_GATE_DOOR));
                m_pInstance->SetData(TYPE_CRUSADERS,IN_PROGRESS);
                break;
                //summoning crusaders
            case 3090:
                m_pInstance->SetData(TYPE_STAGE,6);
                if (m_pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_NORMAL ||
                    m_pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_HEROIC)
                {
                    crusaderscount = 12;
                    switch (urand(0,3))
                    {                                       // Healers, 3 in 25-mode
                        case 0:
                            crusader[0] = NPC_CRUSADER_2_1;
                            crusader[1] = NPC_CRUSADER_2_12;
                            crusader[2] = NPC_CRUSADER_2_13;
                            break;
                        case 1:
                            crusader[0] = NPC_CRUSADER_2_1;
                            crusader[1] = NPC_CRUSADER_2_2;
                            crusader[2] = NPC_CRUSADER_2_13;
                            break;
                        case 2:
                            crusader[0] = NPC_CRUSADER_2_1;
                            crusader[1] = NPC_CRUSADER_2_2;
                            crusader[2] = NPC_CRUSADER_2_12;
                            break;
                        case 3:
                            crusader[0] = NPC_CRUSADER_2_2;
                            crusader[1] = NPC_CRUSADER_2_12;
                            crusader[2] = NPC_CRUSADER_2_13;
                            break;
                        }
                    switch (urand(0,5))
                    {                                       // Random melee DD, 2 in 25-mode
                        case 0:
                            crusader[3] = NPC_CRUSADER_2_3;
                            crusader[4] = NPC_CRUSADER_2_4;
                            break;
                        case 1:
                            crusader[3] = NPC_CRUSADER_2_3;
                            crusader[4] = NPC_CRUSADER_2_5;
                            break;
                        case 2:
                            crusader[3] = NPC_CRUSADER_2_3;
                            crusader[4] = NPC_CRUSADER_2_6;
                            break;
                        case 3:
                            crusader[3] = NPC_CRUSADER_2_4;
                            crusader[4] = NPC_CRUSADER_2_5;
                            break;
                        case 4:
                            crusader[3] = NPC_CRUSADER_2_4;
                            crusader[4] = NPC_CRUSADER_2_6;
                            break;
                        case 5:
                            crusader[3] = NPC_CRUSADER_2_5;
                            crusader[4] = NPC_CRUSADER_2_6;
                            break;
                        }
                    switch (urand(0,3))
                    {                                       // Random magic DD, 3 in 25-mode
                        case 0:
                            crusader[5] = NPC_CRUSADER_2_7;
                            crusader[6] = NPC_CRUSADER_2_8;
                            crusader[7] = NPC_CRUSADER_2_11;
                            break;
                        case 1:
                            crusader[5] = NPC_CRUSADER_2_7;
                            crusader[6] = NPC_CRUSADER_2_8;
                            crusader[7] = NPC_CRUSADER_2_14;
                            break;
                        case 2:
                            crusader[5] = NPC_CRUSADER_2_8;
                            crusader[6] = NPC_CRUSADER_2_11;
                            crusader[7] = NPC_CRUSADER_2_14;
                            break;
                        case 3:
                            crusader[5] = NPC_CRUSADER_2_7;
                            crusader[6] = NPC_CRUSADER_2_11;
                            crusader[7] = NPC_CRUSADER_2_14;
                            break;
                    }
                    crusader[8]  = NPC_CRUSADER_2_9;  //Hunter+warlock
                    crusader[9]  = NPC_CRUSADER_2_10;
                    crusader[10] = NPC_CRUSADER_0_1;
                    crusader[11] = NPC_CRUSADER_0_2;
                }
                else 
                {
                    crusaderscount = 6;
                    switch (urand(0,5))
                    {                                       // Healers, 2 in 10-mode
                        case 0:
                            crusader[0] = NPC_CRUSADER_2_1;
                            crusader[1] = NPC_CRUSADER_2_12;
                            break;
                        case 1:
                            crusader[0] = NPC_CRUSADER_2_1;
                            crusader[1] = NPC_CRUSADER_2_2;
                            break;
                        case 2:
                            crusader[0] = NPC_CRUSADER_2_2;
                            crusader[1] = NPC_CRUSADER_2_12;
                            break;
                        case 3:
                            crusader[0] = NPC_CRUSADER_2_1;
                            crusader[1] = NPC_CRUSADER_2_13;
                            break;
                        case 4:
                            crusader[0] = NPC_CRUSADER_2_2;
                            crusader[1] = NPC_CRUSADER_2_13;
                            break;
                        case 5:
                            crusader[0] = NPC_CRUSADER_2_12;
                            crusader[1] = NPC_CRUSADER_2_13;
                            break;
                    }
                    switch (urand(0,5))
                    {                                       // Random melee DD, 2 in 10-mode
                        case 0:
                            crusader[3] = NPC_CRUSADER_2_3;
                            crusader[2] = NPC_CRUSADER_2_4;
                            break;
                        case 1:
                            crusader[3] = NPC_CRUSADER_2_3;
                            crusader[2] = NPC_CRUSADER_2_5;
                            break;
                        case 2:
                            crusader[3] = NPC_CRUSADER_2_3;
                            crusader[2] = NPC_CRUSADER_2_6;
                            break;
                        case 3:
                            crusader[3] = NPC_CRUSADER_2_4;
                            crusader[2] = NPC_CRUSADER_2_5;
                            break;
                        case 4:
                            crusader[3] = NPC_CRUSADER_2_4;
                            crusader[2] = NPC_CRUSADER_2_6;
                            break;
                        case 5:
                            crusader[3] = NPC_CRUSADER_2_5;
                            crusader[2] = NPC_CRUSADER_2_6;
                            break;
                    }
                    switch (urand(0,5))
                    {                                       // Random magic DD, 2 in 10-mode
                        case 0:
                            crusader[4] = NPC_CRUSADER_2_7;
                            crusader[5] = NPC_CRUSADER_2_8;
                            break;
                        case 1:
                            crusader[5] = NPC_CRUSADER_2_7;
                            crusader[4] = NPC_CRUSADER_2_14;
                            break;
                        case 2:
                            crusader[5] = NPC_CRUSADER_2_7;
                            crusader[4] = NPC_CRUSADER_2_11;
                            break;
                        case 3:
                            crusader[5] = NPC_CRUSADER_2_8;
                            crusader[4] = NPC_CRUSADER_2_11;
                            break;
                        case 4:
                            crusader[5] = NPC_CRUSADER_2_8;
                            crusader[4] = NPC_CRUSADER_2_14;
                            break;
                        case 5:
                            crusader[5] = NPC_CRUSADER_2_11;
                            crusader[4] = NPC_CRUSADER_2_14;
                            break;
                    }
                }
                for(uint8 i = 0; i < crusaderscount; ++i)
                {
                    m_creature->SummonCreature(crusader[i], SpawnLoc[i+2].x, SpawnLoc[i+2].y, SpawnLoc[i+2].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(crusader[i])))
                    {
                        pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                        pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                    }
                }
                m_pInstance->SetData(TYPE_CRUSADERS_COUNT,crusaderscount);
                UpdateTimer = 3000;
                m_pInstance->SetData(TYPE_EVENT,0);
                m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_MAIN_GATE_DOOR));
                m_pInstance->SetData(TYPE_CRUSADERS,IN_PROGRESS);
                break;
            //Crusaders battle end
            case 3100:
                DoScriptText(-1713535, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,0);
                break;
            case 4000:
                DoScriptText(-1713536, m_creature);
                UpdateTimer = 3000;
                m_pInstance->SetData(TYPE_EVENT,4010);
                break;
            case 4010:
                DoScriptText(-1713537, m_creature);
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,4015);
                m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_MAIN_GATE_DOOR));
                break;
            case 4015:
                m_pInstance->SetData(TYPE_STAGE,7);
                m_pInstance->SetData(TYPE_VALKIRIES,IN_PROGRESS);
                m_creature->SummonCreature(NPC_LIGHTBANE, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_LIGHTBANE)))
                {
                    pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                    pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                    pTemp->SetInCombatWithZone();
                }
                m_creature->SummonCreature(NPC_DARKBANE, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_DARKBANE)))
                {
                    pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                    pTemp->AddSplineFlag(SPLINEFLAG_WALKMODE);
                    pTemp->SetInCombatWithZone();
                }
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,4016);
                m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_MAIN_GATE_DOOR));
                break;
            case 4040:
                UpdateTimer = 60000;
                m_pInstance->SetData(TYPE_EVENT,5000);
                break;

            case 5000:
                DoScriptText(-1713549, m_creature);
                UpdateTimer = 8000;
                m_pInstance->SetData(TYPE_EVENT,5005);
                break;
            case 5005:
                UpdateTimer = 8000;
                m_pInstance->SetData(TYPE_EVENT,5010);
                m_pInstance->SetData(TYPE_STAGE,8);
                m_creature->SummonCreature(NPC_LICH_KING_1, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z, 5, TEMPSUMMON_MANUAL_DESPAWN, 0);
                break;
            case 5020:
                DoScriptText(-1713551, m_creature);
                UpdateTimer = 8000;
                m_pInstance->SetData(TYPE_EVENT,5030);
                break;
            case 6000:
                m_creature->NearTeleportTo(SpawnLoc[19].x, SpawnLoc[19].y, SpawnLoc[19].z, 4.0f);
                UpdateTimer = 20000;
                m_pInstance->SetData(TYPE_EVENT,6005);
                break;
            case 6005:
                DoScriptText(-1713565, m_creature);
                UpdateTimer = 20000;
                m_pInstance->SetData(TYPE_EVENT,6010);
                break;
            case 6010:
                if (m_pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_10MAN_HEROIC ||
                    m_pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_HEROIC)
                DoScriptText(-1713566, m_creature);
                UpdateTimer = 60000;
                m_pInstance->SetData(TYPE_EVENT,6020);
                break;
            case 6020:
                m_pInstance->SetData(TYPE_STAGE,10);
                m_creature->ForcedDespawn();
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,6030);
                break;
            }
        } else UpdateTimer -= diff;
        m_pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
    }
};

CreatureAI* GetAI_npc_tirion_toc(Creature* pCreature)
{
    return new npc_tirion_tocAI(pCreature);
}

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
        
        UpdateTimer = m_pInstance->GetData(TYPE_EVENT_TIMER);

        if (UpdateTimer <= diff)
        {
            switch (m_pInstance->GetData(TYPE_EVENT))
            {
                case 120:
                    m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                    DoScriptText(-1713702, m_creature);
                    UpdateTimer = 2000;
                    m_pInstance->SetData(TYPE_EVENT,122);
                    break;
                case 122:
                    m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                    UpdateTimer = 3000;
                    m_pInstance->SetData(TYPE_EVENT,130);
                    break;
                case 2010:
                    DoScriptText(-1713527, m_creature);
                    UpdateTimer = 5000;
                    m_pInstance->SetData(TYPE_EVENT,2020);
                    break;
                case 3050:
                    DoScriptText(-1713531, m_creature);
                    UpdateTimer = 5000;
                    m_pInstance->SetData(TYPE_EVENT,3060);
                    break;
                case 3070:
                    DoScriptText(-1713533, m_creature);
                    UpdateTimer = 5000;
                    m_pInstance->SetData(TYPE_EVENT,3080);
                    break;
                case 3081:
                    DoScriptText(-1713734, m_creature);
                    UpdateTimer = 5000;
                    m_pInstance->SetData(TYPE_EVENT,3091);
                    m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_MAIN_GATE_DOOR));
                    break;
                case 4030:
                    DoScriptText(-1713748, m_creature);
                    UpdateTimer = 5000;
                    m_pInstance->SetData(TYPE_EVENT,4040);
                    break;
            }
        } else UpdateTimer -= diff;
        m_pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
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
        
        UpdateTimer = m_pInstance->GetData(TYPE_EVENT_TIMER);

        if (UpdateTimer <= diff)
        {
            switch (m_pInstance->GetData(TYPE_EVENT))
            {
                case 130:
                    m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                    DoScriptText(-1713502, m_creature);
                    UpdateTimer = 2000;
                    m_pInstance->SetData(TYPE_EVENT,132);
                    break;
                case 132:
                    m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                    UpdateTimer = 3000;
                    m_pInstance->SetData(TYPE_EVENT,140);
                    break;
                case 2020:
                    DoScriptText(-1713528, m_creature);
                    UpdateTimer = 5000;
                    m_pInstance->SetData(TYPE_EVENT,2030);
                    break;
                case 3051:
                    DoScriptText(-1713731, m_creature);
                    UpdateTimer = 5000;
                    m_pInstance->SetData(TYPE_EVENT,3061);
                    break;
                case 3071:
                    DoScriptText(-1713733, m_creature);
                    UpdateTimer = 5000;
                    m_pInstance->SetData(TYPE_EVENT,3081);
                    break;
                case 3080:
                    DoScriptText(-1713534, m_creature);
                    UpdateTimer = 5000;
                    m_pInstance->SetData(TYPE_EVENT,3090);
                    m_pInstance->DoUseDoorOrButton(m_pInstance->GetData64(GO_MAIN_GATE_DOOR));
                    break;
                case 4020:
                    DoScriptText(-1713548, m_creature);
                    UpdateTimer = 5000;
                    m_pInstance->SetData(TYPE_EVENT,4030);
                    break;
            }
        } else UpdateTimer -= diff;
        m_pInstance->SetData(TYPE_EVENT_TIMER,UpdateTimer);
    }
};

CreatureAI* GetAI_npc_rinn_toc(Creature* pCreature)
{
    return new npc_rinn_tocAI(pCreature);
};

void AddSC_trial_of_the_crusader()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "npc_toc_announcer";
    NewScript->GetAI = &GetAI_npc_toc_announcer;
    NewScript->pGossipHello = &GossipHello_npc_toc_announcer;
    NewScript->pGossipSelect = &GossipSelect_npc_toc_announcer;
    NewScript->RegisterSelf();

    NewScript = new Script;
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
    NewScript->RegisterSelf();
}
