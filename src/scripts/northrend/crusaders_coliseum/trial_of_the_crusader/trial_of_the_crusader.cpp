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
SDComment: Spammca
SDCategory: trial_of_the_crusader
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"
#include "ObjectMgr.h"

npc_toc_announcerAI::npc_toc_announcerAI(Creature* pCreature) : ScriptedAI(pCreature)
{
    m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    m_dDifficulty = pCreature->GetMap()->GetDifficulty();
    isHeroic = pCreature->GetMap()->IsHeroicRaid();
    is10Man = pCreature->GetMap()->IsRaid10Man();

    pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    pCreature->SetVisibility(VISIBILITY_ON);

    encounterCreature = NULL;
    encounterCreature2 = NULL;
    Reset();
}

void npc_toc_announcerAI::Reset()
{
    if (!m_pInstance)
        m_creature->ForcedDespawn();

    m_creature->SetRespawnDelay(DAY);

    if (m_creature->GetVisibility() == VISIBILITY_OFF)
    {
        m_creature->SetVisibility(VISIBILITY_ON);
        uint32 traveltime = 2000;
        if (GameObject* go = m_pInstance->GetGameObject(GO_GATE_EAST))
            traveltime = uint32(go->GetPosition().GetDistance2d(SpawnLoc[LOC_ANNOUNCER])/0.0025f);

        m_creature->SendMonsterMove(SpawnLoc[LOC_ANNOUNCER].x, SpawnLoc[LOC_ANNOUNCER].y, SpawnLoc[LOC_ANNOUNCER].z, SPLINETYPE_FACINGANGLE, SPLINEFLAG_WALKMODE, traveltime, NULL, 5.0614f);
        m_pInstance->instance->CreatureRelocation(m_creature, SpawnLoc[LOC_ANNOUNCER], 5.0614f);
    }

    m_TimerMgr->RemoveTimer(TIMER_PHASE_HANDLING);
    m_TimerMgr->SetUpdatable(false);
    currentEncounter = -1;
    encounterStage = 0;
    customValue = 0;

    encounterCreature = NULL;
    encounterCreature2 = NULL;
}

void npc_toc_announcerAI::AttackStart(Unit* /*who*/) { return; }

void npc_toc_announcerAI::MovementInform(uint32 uiType, uint32 uiPointId)
{
    if (uiType == POINT_MOTION_TYPE && uiPointId == POINT_MOVE)
        m_creature->SetVisibility(VISIBILITY_OFF);
}

Creature* npc_toc_announcerAI::DoSpawnTocBoss(uint32 id, Coords coord, float ori, bool update_z)
{
    const CreatureInfo* cinfo = GetCreatureTemplateStore(id);
    uint32 corpseDespTime = cinfo->lootid ? 300000 : 60000;
    Creature* pTemp = m_creature->SummonCreature(id, coord, ori, TEMPSUMMON_CORPSE_TIMED_DESPAWN, corpseDespTime, update_z);
    if (pTemp)
        pTemp->SetRespawnDelay(7*DAY);

    return pTemp;
}

void npc_toc_announcerAI::SummonToCBoss(uint32 id, uint32 id2, uint32 dooropen)
{
    Coords coord = SpawnLoc[LOC_BACKDOOR];
    bool isFlying = false;
    if (id2)
    {
        coord.x += 6.f;
        coord.z += 5.f;
        isFlying = true;

        Coords coord2;
        coord2 = SpawnLoc[LOC_BACKDOOR];
        coord2.x -= 6.f;
        coord2.z += 8.f;

        encounterCreature2 = DoSpawnTocBoss(id2, coord2, M_PI_F*1.5f, !isFlying);
    }
    encounterCreature = DoSpawnTocBoss(id, coord, M_PI_F*1.5f, !isFlying);

    AddNonCastTimer(TIMER_DOOR_HANDLER, dooropen, 5000);
}

void npc_toc_announcerAI::DeleteCreaturesAndRemoveAuras()
{
    CreatureList deleteList;
    GetCreatureListWithEntryInGrid(deleteList, m_creature, NPC_LIGHT_ESSENCE, DEFAULT_VISIBILITY_INSTANCE);
    GetCreatureListWithEntryInGrid(deleteList, m_creature, NPC_DARK_ESSENCE, DEFAULT_VISIBILITY_INSTANCE);
    GetCreatureListWithEntryInGrid(deleteList, m_creature, NPC_CONCENTRATED_LIGHT, DEFAULT_VISIBILITY_INSTANCE);
    GetCreatureListWithEntryInGrid(deleteList, m_creature, NPC_CONCENTRATED_DARKNESS, DEFAULT_VISIBILITY_INSTANCE);
    GetCreatureListWithEntryInGrid(deleteList, m_creature, NPC_MISTRESS_OF_PAIN, DEFAULT_VISIBILITY_INSTANCE);
    GetCreatureListWithEntryInGrid(deleteList, m_creature, NPC_FELFLAME_INFERNAL, DEFAULT_VISIBILITY_INSTANCE);
    // clean up useless corpses
    GetCreatureListWithEntryInGrid(deleteList, m_creature, NPC_GORMOK, DEFAULT_VISIBILITY_INSTANCE);
    GetCreatureListWithEntryInGrid(deleteList, m_creature, NPC_ACIDMAW, DEFAULT_VISIBILITY_INSTANCE);
    GetCreatureListWithEntryInGrid(deleteList, m_creature, NPC_DREADSCALE, DEFAULT_VISIBILITY_INSTANCE);
    for(CreatureList::iterator itr = deleteList.begin(); itr != deleteList.end(); ++itr)
        (*itr)->ForcedDespawn(3500);

    Map::PlayerList const &PlayerList = m_pInstance->instance->GetPlayers();
    if (!PlayerList.isEmpty())
        for (Map::PlayerList::const_iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
            if (Player* plr = iter->getSource())
                for (uint8 d = 0; d < 12; ++d)
                    plr->RemoveAurasDueToSpell(Dispell[d]);
}

void npc_toc_announcerAI::ChooseEvent(uint8 encounterId, Player *chooser)
{
    if (m_pInstance->GetData(encounterId) == DONE)
        return;

    uint32 startTimer = 0;
    uint32 runaway = 0;
    switch (encounterId)
    {
        case TYPE_BEASTS:
        case TYPE_JARAXXUS:
        case TYPE_CRUSADERS:
        case TYPE_VALKIRIES:
        case TYPE_LICH_KING:
            currentEncounter = encounterId;
            encounterStage = 0;
            startTimer = 1000;
            runaway = 2000;
            break;
        case TYPE_ANUBARAK:
            chooser->TeleportTo(WorldLocation(m_creature->GetMapId(), SpawnLoc[LOC_UNDERGROUND], 0));
            break;
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
    // if data already set, dont set again, mainly cause of factioned champions
    // set in SetData
    //if (m_pInstance->GetData(type) == data)
    //    return;

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
                case GORMOK_DONE:
                    if (encounterStage != 5)
                        break;

                    m_TimerMgr->SetValue(TIMER_PHASE_HANDLING, TIMER_VALUE_CUSTOM, m_TimerMgr->GetValue(TIMER_PHASE_HANDLING, TIMER_VALUE_TIMER));
                    m_TimerMgr->Cooldown(TIMER_PHASE_HANDLING, 1000);
                    break;
                case SNAKES_ONE_DOWN:
                    m_TimerMgr->SetValue(TIMER_PHASE_HANDLING, TIMER_VALUE_CUSTOM, getMSTime());
                    break;
                case SNAKES_DONE:
                    if (encounterStage != 8)
                        break;

                    if (getMSTimeDiff(m_TimerMgr->GetValue(TIMER_PHASE_HANDLING, TIMER_VALUE_CUSTOM), getMSTime() < 10000)) // kill under 10 seconds
                        m_pInstance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_JORMUNGAR_10_SEC);

                    m_TimerMgr->SetValue(TIMER_PHASE_HANDLING, TIMER_VALUE_CUSTOM, m_TimerMgr->GetValue(TIMER_PHASE_HANDLING, TIMER_VALUE_TIMER));
                    m_TimerMgr->Cooldown(TIMER_PHASE_HANDLING, 1000);
                    break;
                case DONE:
                    encounterStage = 50;
                    m_TimerMgr->Cooldown(TIMER_PHASE_HANDLING, 6500);
                    break;
                default:
                    break;
            }
            return;
        }
        case TYPE_JARAXXUS:
        {
            switch(data)
            {
                case FAIL:
                    DeleteCreaturesAndRemoveAuras();
                    Reset();
                    break;
                case DONE:
                    encounterStage = 50;
                    m_TimerMgr->Cooldown(TIMER_PHASE_HANDLING, 6500);
                    DeleteCreaturesAndRemoveAuras();
                    break;
                default:
                    break;
            }
            return;
        }
        case TYPE_CRUSADERS:
        {
            switch(data)
            {
                case FAIL:
                    Reset();
                    break;
                case CHAMPION_DIED:
                    if (!m_TimerMgr->GetValue(TIMER_PHASE_HANDLING, TIMER_VALUE_CUSTOM))
                        m_TimerMgr->SetValue(TIMER_PHASE_HANDLING, TIMER_VALUE_CUSTOM, getMSTime());
                    break;
                case DONE:
                    if (m_pInstance->GetInstanceSide() == INSTANCE_SIDE_ALI)
                        DoScriptText(SAY_STAGE_2_05a, m_pInstance->GetCreature(NPC_WRYNN));
                    else
                        DoScriptText(SAY_STAGE_2_05h, m_pInstance->GetCreature(NPC_GARROSH));

                    encounterStage = 50;
                    m_TimerMgr->Cooldown(TIMER_PHASE_HANDLING, 8000);
                    break;
                default:
                    break;
            }
            return;
        }
        case TYPE_VALKIRIES:
        {
            switch(data)
            {
                case FAIL:
                    DeleteCreaturesAndRemoveAuras();
                    Reset();
                    break;
                case DONE:
                    encounterStage = 50;
                    m_TimerMgr->Cooldown(TIMER_PHASE_HANDLING, 6000);
                    DeleteCreaturesAndRemoveAuras();
                    break;
                default:
                    break;
            }
            return;
        }
        case TYPE_LICH_KING:
        {
            switch(data)
            {
                case FAIL:
                    Reset();
                    break;
                default:
                    break;
            }
            return;
        }
        case TYPE_ANUBARAK:
        {
            switch(data)
            {
                case DONE:
                    currentEncounter = TYPE_ANUBARAK;
                    encounterStage = 50;
                    m_TimerMgr->SetUpdatable(true);
                    AddNonCastTimer(TIMER_PHASE_HANDLING, 5000, 0);
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
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
                        cooldown = 4000;
                        break;
                    case 3:
                        SummonToCBoss(NPC_GORMOK, 0, 100);
                        //if (isHeroic)
                        //    AddNonCastTimer(TIMER_CUSTOM, 540000, 5000);
                        cooldown = 1000;
                        break;
                    case 4:
                        if (encounterCreature)
                            encounterCreature->AI()->AttackStart(m_pInstance->GetRandomPlayerInMap());
                        cooldown = 5000;
                        break;
                    case 5:
                        if (m_pInstance->GetInstanceSide() == INSTANCE_SIDE_ALI)
                            DoScriptText(SAY_STAGE_0_03a, m_pInstance->GetCreature(NPC_WRYNN));
                        else
                            DoScriptText(SAY_STAGE_0_03h, m_pInstance->GetCreature(NPC_GARROSH));
                        cooldown = isHeroic ? 174000 : REALLY_BIG_COOLDOWN;
                        break;
                    case 6:
                        SummonToCBoss(NPC_DREADSCALE, NULL, 5100);
                        DoScriptText(SAY_STAGE_0_04, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 6000;
                        break;
                    case 7:
                        if (encounterCreature)
                            encounterCreature->AI()->AttackStart(m_pInstance->GetRandomPlayerInMap());
                        cooldown = 4000;
                        break;
                    case 8:
                    {
                        Player* randPlr = m_pInstance->GetRandomPlayerInMap();
                        if (!randPlr)
                            break;

                        encounterCreature2 = DoSpawnTocBoss(NPC_ACIDMAW, randPlr->GetPosition(), 0);
                        if (!encounterCreature2)
                            break;

                        encounterCreature2->CastSpell(encounterCreature2, SPELL_EMERGE_ACIDMAW, false);
                        cooldown = isHeroic ? 170000 + stepTimer->GetValue(TIMER_VALUE_CUSTOM) : REALLY_BIG_COOLDOWN;
                        break;
                    }
                    case 9:
                        SummonToCBoss(NPC_ICEHOWL, NULL, 3100);
                        DoScriptText(SAY_STAGE_0_05, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 4000;
                    case 10:
                        if (encounterCreature)
                            encounterCreature->AI()->AttackStart(m_pInstance->GetRandomPlayerInMap());
                        cooldown = isHeroic ? 176000 + stepTimer->GetValue(TIMER_VALUE_CUSTOM) : REALLY_BIG_COOLDOWN;
                        break;
                    case 11:
                        if (encounterCreature)
                            encounterCreature->GetTimerMgr()->AddSpellToQueue(SPELL_BERSERK);
                        cooldown = REALLY_BIG_COOLDOWN;
                        break;
                    case 51: // outro
                        DoScriptText(SAY_STAGE_0_06, m_pInstance->GetCreature(NPC_TIRION));
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
                        encounterCreature2 = m_pInstance->GetCreature(NPC_JARAXXUS);
                        if (encounterCreature2 && m_pInstance->GetData(TYPE_JARAXXUS) == FAIL)
                        {
                            encounterCreature2->RemoveAurasDueToSpell(SPELL_JARAXXUS_CHAINS);
                            encounterCreature2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            encounterCreature2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                            encounterCreature2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                            ((ScriptedAI*)encounterCreature2->AI())->EnableAttack(true);
                            DoScriptText(SAY_STAGE_1_AGGRO, encounterCreature2);
                            encounterCreature2->AI()->AttackStart(m_pInstance->GetRandomPlayerInMap());
                            m_pInstance->SetData(TYPE_JARAXXUS, IN_PROGRESS);
                            cooldown = REALLY_BIG_COOLDOWN;
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
                            break;

                        encounterCreature->AddSplineFlag(SPLINEFLAG_WALKMODE);
                        encounterCreature->GetMotionMaster()->MovePoint(POINT_MOVE, SpawnLoc[LOC_FIZZLE_END], false);

                        uint32 timeToPoint = uint32(encounterCreature->GetPosition().GetDistance2d(SpawnLoc[LOC_FIZZLE_END])/0.0025f);

                        m_TimerMgr->Cooldown(TIMER_DOOR_HANDLER, timeToPoint);
                        cooldown = timeToPoint + 1000;
                        break;
                    }
                    case 3:
                        DoScriptText(SAY_STAGE_1_02, encounterCreature);
                        cooldown = 11000;
                        break;
                    case 4:
                        DoScriptText(SAY_STAGE_1_03, encounterCreature);
                        encounterCreature->SummonCreature(NPC_WILFRED_PORTAL_GROUND, SpawnLoc[LOC_CENTER], 1.5f*M_PI_F, TEMPSUMMON_TIMED_DESPAWN, 8000);
                        cooldown = 2000;
                        break;
                    case 5:
                    {
                        Coords coord = SpawnLoc[LOC_CENTER];
                        coord.z += 5.f;
                        if (!(encounterCreature2 = encounterCreature->SummonCreature(NPC_TRIGGER, coord, 1.5f*M_PI_F, TEMPSUMMON_TIMED_DESPAWN, 6000)))
                            break;

                        encounterCreature2->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.f);
                        encounterCreature2->CastSpell(encounterCreature2, SPELL_WILFRED_PORTAL, false);
                        cooldown = 3500;
                        break;
                    }
                    case 6:
                        DoScriptText(SAY_STAGE_1_04, encounterCreature);
                        if (!(encounterCreature2 = DoSpawnTocBoss(NPC_JARAXXUS, encounterCreature2->GetPosition(), encounterCreature->GetOrientation())))
                            break;

                        cooldown = 500;
                        break;
                    case 7:
                        encounterCreature2->GetMotionMaster()->MovePoint(0, SpawnLoc[LOC_JAROXXUS_END], false);
                        cooldown = 2300;
                        break;
                    case 8:
                        encounterCreature2->SetFacingToObject(encounterCreature);
                        encounterCreature2->SetSummonPoint(SpawnLoc[LOC_JAROXXUS_END].x, SpawnLoc[LOC_JAROXXUS_END].y, SpawnLoc[LOC_JAROXXUS_END].z, encounterCreature->GetOrientation());
                        ((ScriptedAI*)encounterCreature2->AI())->SetCombatMovement(false);
                        cooldown = 8500;
                        break;
                    case 9:
                        DoScriptText(SAY_STAGE_1_05, encounterCreature2);
                        cooldown = 7000;
                        break;
                    case 10:
                        DoScriptText(SAY_STAGE_1_06, encounterCreature);
                        cooldown = 800;
                        break;
                    case 11:
                        encounterCreature2->CastSpell(encounterCreature, SPELL_FEL_LIGHTNING_IK, false);
                        cooldown = 2000;
                        break;
                    case 12:
                        DoScriptText(SAY_STAGE_1_07, m_pInstance->GetCreature(NPC_TIRION));
                        ((ScriptedAI*)encounterCreature2->AI())->SetCombatMovement(true);
                        cooldown = 5000;
                        break;
                    case 13:
                        encounterCreature2->RemoveAurasDueToSpell(SPELL_JARAXXUS_CHAINS);
                        encounterCreature2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        encounterCreature2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                        encounterCreature2->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                        ((ScriptedAI*)encounterCreature2->AI())->EnableAttack(true);
                        m_pInstance->SetData(TYPE_JARAXXUS, IN_PROGRESS);
                        DoScriptText(SAY_STAGE_1_AGGRO, encounterCreature2);
                        encounterCreature2->AI()->AttackStart(m_pInstance->GetRandomPlayerInMap());
                        cooldown = REALLY_BIG_COOLDOWN;
                        break;
                    case 51: //outro
                        DoScriptText(SAY_STAGE_1_08, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 15000;
                        break;
                    case 52:
                        DoScriptText(SAY_STAGE_1_09, m_pInstance->GetCreature(NPC_GARROSH));
                        cooldown = 10000;
                        break;
                    case 53:
                        DoScriptText(SAY_STAGE_1_10, m_pInstance->GetCreature(NPC_WRYNN));
                        cooldown = 7000;
                        break;
                    case 54:
                        DoScriptText(SAY_STAGE_1_11, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 5000;
                        break;
                    case 55:
                        break;
                }
                break;
            }
            case TYPE_CRUSADERS:
            {
                switch(encounterStage)
                {
                    case 1:
                        DoScriptText(SAY_STAGE_2_01, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 9000;
                        break;
                    case 2:
                        if (m_pInstance->GetInstanceSide() == INSTANCE_SIDE_ALI)
                            DoScriptText(SAY_STAGE_2_02h, m_pInstance->GetCreature(NPC_GARROSH));
                        else
                            DoScriptText(SAY_STAGE_2_02a, m_pInstance->GetCreature(NPC_WRYNN));
                        cooldown = 17000;
                        break;
                    case 3:
                        DoScriptText(SAY_STAGE_2_03, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 5000;
                        break;
                    case 4:
                    {
                        uint32 spawnMask = 0;
                        if (!(spawnMask = m_pInstance->GetData(TYPE_CHAMPION_SPAWN_MASK)))
                        {
                            typedef std::list<uint8> ChampionList;
                            ChampionList healerList, dpsList, finalList;

                            for(uint8 i = 0; i < CHAMPION_COUNT; ++i)
                            {
                                if (i == CHAMPION_R_DRUID  || i == CHAMPION_H_PALADIN || i == CHAMPION_R_SHAMAN || i == CHAMPION_D_PRIEST)
                                    healerList.push_back(i);
                                else
                                    dpsList.push_back(i);
                            }

                            for(uint8 h = 0; h < 2; ++h)
                            {
                                uint8 neededCount = h ? (is10Man ? 2 : 3) : (is10Man ? 4 : 7);
                                ChampionList& refList = h ? healerList : dpsList;
                                for (uint8 i = 0; i < neededCount; ++i)
                                {
                                    ChampionList::iterator itr = refList.begin();
                                    std::advance(itr, urand(0, refList.size()-1));
                                    finalList.push_back(*itr);
                                    refList.erase(itr);
                                }
                            }

                            for(ChampionList::iterator itr = finalList.begin(); itr != finalList.end(); ++itr)
                                spawnMask |= (1 << *itr);

                            m_pInstance->SetData(TYPE_CHAMPION_SPAWN_MASK, spawnMask);
                        }
                        encounterCreature = NULL;
                        encounterCreature2 = NULL;
                        AddNonCastTimer(TIMER_CUSTOM, 500, 2000);
                        cooldown = REALLY_BIG_COOLDOWN;
                        break;
                    }
                    case 5:
                    {
                        uint32 textId, npcId, faction;
                        if (m_pInstance->GetInstanceSide() == INSTANCE_SIDE_ALI)
                        {
                            textId = SAY_STAGE_2_04h;
                            npcId = NPC_GARROSH;
                            faction = FACTION_HORDE;
                        }
                        else
                        {
                            textId = SAY_STAGE_2_04a;
                            npcId = NPC_WRYNN;
                            faction = FACTION_ALLIANCE;
                        }

                        DoScriptText(textId, m_pInstance->GetCreature(npcId));
                        CreatureList ChampionList;
                        for(uint8 i = 0; i < CHAMPION_COUNT; ++i)
                            GetCreatureListWithEntryInGrid(ChampionList, m_creature, FChampIDs[i][faction], DEFAULT_VISIBILITY_INSTANCE);

                        stepTimer->SetValue(TIMER_VALUE_CUSTOM, 0);

                        for(CreatureList::iterator itr = ChampionList.begin(); itr != ChampionList.end(); ++itr)
                        {
                            Creature* champ = *itr;
                            if (!champ)
                                continue;

                            champ->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            ((ScriptedAI*)champ->AI())->EnableAttack(true);
                        }

                        cooldown = REALLY_BIG_COOLDOWN;
                        break;
                    }
                    case 51: // outro
                        DoScriptText(SAY_STAGE_2_06, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 5000;
                        break;
                    case 52:
                    {
                        uint32 chestId = 0;
                        m_pInstance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_CHAMPIONS);
                        if (getMSTimeDiff(stepTimer->GetValue(TIMER_VALUE_CUSTOM), getMSTime() < 73000)) // kill under 60 seconds
                            m_pInstance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_CHAMPIONS_MINUTE);

                        switch(m_dDifficulty)
                        {
                            case RAID_DIFFICULTY_10MAN_NORMAL: chestId = GO_CRUSADERS_CACHE_10; break;
                            case RAID_DIFFICULTY_25MAN_NORMAL: chestId = GO_CRUSADERS_CACHE_25; break;
                            case RAID_DIFFICULTY_10MAN_HEROIC: chestId = GO_CRUSADERS_CACHE_10_H; break;
                            case RAID_DIFFICULTY_25MAN_HEROIC: chestId = GO_CRUSADERS_CACHE_25_H; break;
                            default: break;
                        }
                        if (chestId)
                            m_creature->SummonGameobject(chestId, SpawnLoc[LOC_CENTER], M_PI_F*1.5f, 604800);
                        break;
                    }
                }
                break;
            }
            case TYPE_VALKIRIES:
            {
                switch(encounterStage)
                {
                    case 1:
                        DoScriptText(SAY_STAGE_3_01, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 12000;
                        break;
                    case 2:
                        SummonToCBoss(NPC_LIGHTBANE, NPC_DARKBANE);
                        for(uint8 i = 0; i < 4; ++i)
                            DoSpawnTocBoss(i/2 ? NPC_LIGHT_ESSENCE : NPC_DARK_ESSENCE, SpawnLoc[LOC_D_ESSENCE_1+i], 0);

                        cooldown = 2000;
                        break;
                    case 3:
                    {
                        if (!encounterCreature || !encounterCreature2)
                            break;

                        DoScriptText(SAY_STAGE_3_02, m_pInstance->GetCreature(NPC_TIRION));

                        uint32 travelTime[2];
                        for(uint8 second = 0; second < 2; ++second)
                        {
                            Creature* crt = second ? encounterCreature2 : encounterCreature;
                            ((ScriptedAI*)crt->AI())->EnableAttack(false);

                            PointPath path;
                            path.resize(3);
                            path.set(0, crt->GetPosition());
                            path.set(1, second ? SpawnLoc[LOC_D_VALKYR_1] : SpawnLoc[LOC_L_VALKYR_1]);
                            path.set(2, second ? SpawnLoc[LOC_D_VALKYR_2] : SpawnLoc[LOC_L_VALKYR_2]);
                            //path.set(3, second ? SpawnLoc[LOC_D_VALKYR_3] : SpawnLoc[LOC_L_VALKYR_3]);

                            travelTime[second] = path.GetTotalLength()/(crt->GetSpeed(MOVE_RUN)*0.001f);

                            crt->GetMotionMaster()->Clear(false, true);
                            crt->ChargeMonsterMove(path, SPLINETYPE_FACINGANGLE, crt->GetSplineFlags(), travelTime[second], M_PI_F*1.5f);
                        }
                        cooldown = (travelTime[0] > travelTime[1] ? travelTime[0] : travelTime[1]) + 5000;
                        break;
                    }
                    case 4:
                    {
                        for(uint8 second = 0; second < 2; ++second)
                        {
                            Creature* crt = second ? encounterCreature2 : encounterCreature;
                            ((ScriptedAI*)crt->AI())->EnableAttack(true);
                            crt->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        }
                        cooldown = REALLY_BIG_COOLDOWN;
                        break;
                    }
                    case 51: // outro
                        if (m_pInstance->GetInstanceSide() == INSTANCE_SIDE_ALI)
                            DoScriptText(SAY_STAGE_3_03a, m_pInstance->GetCreature(NPC_WRYNN));
                        else
                            DoScriptText(SAY_STAGE_3_03h, m_pInstance->GetCreature(NPC_GARROSH));
                        cooldown = 5000;
                        break;
                    case 52:
                        break;
                }
                break;
            }
            case TYPE_LICH_KING:
            {
                switch(encounterStage)
                {
                    case 1:
                        DoScriptText(SAY_STAGE_4_01, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 19000;
                        break;
                    case 2:
                        encounterCreature = DoSpawnTocBoss(NPC_LICH_KING, SpawnLoc[LOC_BACKDOOR], M_PI_F*1.5f);
                        encounterCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, 0.1f);
                        m_pInstance->SetData(TYPE_LICH_KING, IN_PROGRESS);
                        cooldown = 500;
                        break;
                    case 3:
                        DoScriptText(SAY_STAGE_4_02, encounterCreature);
                        cooldown = 7000;
                        break;
                    case 4:
                        if (!(encounterCreature2 = DoSpawnTocBoss(NPC_TRIGGER, SpawnLoc[LOC_LICH_KING_S], M_PI_F*1.5f, false)))
                            break;

                        encounterCreature2->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.8f);
                        encounterCreature2->CastSpell(encounterCreature2, SPELL_LK_GATE, false);
                        cooldown = 4000;
                        break;
                    case 5:
                        encounterCreature->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.f);
                        m_pInstance->instance->CreatureRelocation(encounterCreature, SpawnLoc[LOC_LICH_KING_S], M_PI_F*1.5f);
                        cooldown = 500;
                        break;
                    case 6:
                    {
                        PointPath path;
                        path.resize(2);
                        path.set(0, SpawnLoc[LOC_LICH_KING_S]);
                        path.set(1, SpawnLoc[LOC_LICH_KING_E]);

                        encounterCreature->GetMotionMaster()->Clear(false, true);
                        encounterCreature->GetMotionMaster()->MoveIdle();
                        encounterCreature->ChargeMonsterMove(path, SPLINETYPE_NORMAL, SPLINEFLAG_WALKMODE, path.GetTotalLength()/0.0025f);
                        cooldown = 3000;
                        break;
                    }
                    case 7:
                        DoScriptText(SAY_STAGE_4_03, m_pInstance->GetCreature(NPC_TIRION));
                        cooldown = 8000;
                        break;
                    case 8:
                        DoScriptText(SAY_STAGE_4_04, encounterCreature);
                        encounterCreature->HandleEmote(EMOTE_ONESHOT_LAUGH);
                        cooldown = 13000;
                        break;
                    case 9:
                        encounterCreature->HandleEmote(EMOTE_ONESHOT_EXCLAMATION);
                        cooldown = 3000;
                        break;
                    case 10:
                        encounterCreature->HandleEmote(EMOTE_ONESHOT_KNEEL);
                        cooldown = 1000;
                        break;
                    case 11:
                        encounterCreature->CastSpell(encounterCreature, SPELL_LK_NOVA, false);
                        cooldown = 500;
                        break;
                    case 12:
                    {
                        m_pInstance->instance->CreatureRelocation(encounterCreature, SpawnLoc[LOC_BACKDOOR], M_PI_F*1.5f);
                        encounterCreature->SendMonsterMove(SpawnLoc[LOC_BACKDOOR].x, SpawnLoc[LOC_BACKDOOR].y, SpawnLoc[LOC_BACKDOOR].z, SPLINETYPE_NORMAL, encounterCreature->GetSplineFlags(), 1);

                        Map::PlayerList const &PlayerList = m_pInstance->instance->GetPlayers();
                        if (!PlayerList.isEmpty())
                            for (Map::PlayerList::const_iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
                                if (Player* plr = iter->getSource())
                                    plr->KnockBackFrom(plr, 0, 20);

                        if (GameObject* pFloor = m_pInstance->GetGameObject(GO_ARGENT_COLISEUM_FLOOR))
                        {
                            pFloor->TakenDamage(100, m_creature);
                            pFloor->TakenDamage(100, m_creature);
                        }
                        cooldown = 2000;
                        break;
                    }
                    case 13:
                        DoScriptText(SAY_STAGE_4_05, encounterCreature);
                        cooldown = 10000;
                        break;
                    case 14:
                    {
                        m_pInstance->SetData(TYPE_LICH_KING, DONE);
                        encounterCreature->ForcedDespawn();
                        if (GameObject* pFloor = m_pInstance->GetGameObject(GO_ARGENT_COLISEUM_FLOOR))
                            pFloor->Rebuild(m_creature);
                        break;
                    }
                }
                break;
            }
            case TYPE_ANUBARAK:
            {
                switch(encounterStage)
                {
                    case 51:
                        if (!(encounterCreature = DoSpawnTocBoss(NPC_OUTRO_TIRION, SpawnLoc[LOC_O_TIRION_S], 0)))
                            break;
                        if (!(encounterCreature2 = DoSpawnTocBoss(NPC_OUTRO_ARGENT_MAGE, SpawnLoc[LOC_O_MAGE_S], 0)))
                            break;

                        encounterCreature->GetMotionMaster()->MovePoint(0, SpawnLoc[LOC_O_TIRION_E], false);
                        encounterCreature2->GetMotionMaster()->MovePoint(0, SpawnLoc[LOC_O_MAGE_E], false);
                        cooldown = 10000;
                        break;
                    case 52:
                        DoScriptText(SAY_STAGE_4_06, encounterCreature);
                        cooldown = 15000;
                        break;
                    case 53:
                        if (!isHeroic || m_pInstance->GetData(TYPE_COUNTER) >= 50)
                            break;

                        DoScriptText(SAY_STAGE_4_07, encounterCreature);
                        cooldown = 5000;
                        break;
                    case 54:
                    {
                        uint32 chestId = 0;
                        uint32 wipes = m_pInstance->GetData(TYPE_COUNTER);
                        if (wipes == 0)
                            chestId = is10Man ? GO_TRIBUTE_CHEST_10H_0 : GO_TRIBUTE_CHEST_25H_0;
                        else if (isInRange(1, wipes, 5))
                            chestId = is10Man ? GO_TRIBUTE_CHEST_10H_5 : GO_TRIBUTE_CHEST_25H_5;
                        else if (isInRange(6, wipes, 25))
                            chestId = is10Man ? GO_TRIBUTE_CHEST_10H_25 : GO_TRIBUTE_CHEST_25H_25;
                        else if (isInRange(26, wipes, 49))
                            chestId = is10Man ? GO_TRIBUTE_CHEST_10H_49 : GO_TRIBUTE_CHEST_25H_49;
                        if (chestId)
                            m_creature->SummonGameobject(chestId, SpawnLoc[LOC_UNDERGROUND], 0, 604800);
                        break;
                    }
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
            Reset();
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
            m_creature->GetMotionMaster()->MovePoint(POINT_MOVE, coord.x, coord.y, coord.z, false);
        }
        runawayTimer->SetValue(TIMER_VALUE_DELETE_AT_FINISH, true);
    }

    // handling of custom timer in the event
    if (SpellTimer* customTimer = m_TimerMgr->TimerFinished(TIMER_CUSTOM))
    {
        /*if (currentEncounter == TYPE_BEASTS)
        {
            if (encounterCreature && !encounterCreature->HasAura(SPELL_BERSERK))
                encounterCreature->GetTimerMgr()->AddSpellToQueue(SPELL_BERSERK, UNIT_SELECT_SELF);
            if (encounterCreature2 && !encounterCreature2->HasAura(SPELL_BERSERK))
                encounterCreature2->GetTimerMgr()->AddSpellToQueue(SPELL_BERSERK, UNIT_SELECT_SELF);
        }*/
        if (currentEncounter == TYPE_CRUSADERS)
        {
            uint32 timerCustom = customTimer->GetValue(TIMER_VALUE_CUSTOM);
            uint32 spawnMask = m_pInstance->GetData(TYPE_CHAMPION_SPAWN_MASK);
            uint8 faction = m_pInstance->GetInstanceSide() == INSTANCE_SIDE_ALI ? FACTION_HORDE : FACTION_ALLIANCE;

            Coords Spawn1 = faction == FACTION_ALLIANCE ? SpawnLoc[LOC_FCH_A_SPAWN_1] : SpawnLoc[LOC_FCH_H_SPAWN_1];
            Coords Spawn2 = faction == FACTION_ALLIANCE ? SpawnLoc[LOC_FCH_A_SPAWN_2] : SpawnLoc[LOC_FCH_H_SPAWN_2];
            Coords Jump1 = faction == FACTION_ALLIANCE ? SpawnLoc[LOC_FCH_A_JUMP_1] : SpawnLoc[LOC_FCH_H_JUMP_1];
            Coords Jump2 = faction == FACTION_ALLIANCE ? SpawnLoc[LOC_FCH_A_JUMP_2] : SpawnLoc[LOC_FCH_H_JUMP_2];
            Coords Location = faction == FACTION_ALLIANCE ? SpawnLoc[LOC_FCH_A_MOVE] : SpawnLoc[LOC_FCH_H_MOVE];

            if (encounterCreature2)
            {
                int32 champOrder = customValue-2;
                int32 x_coef = champOrder%2 ? 1 : -1;
                int32 y_coef = champOrder/2 - (is10Man ? 1 : 2);
                if (faction == FACTION_HORDE)
                {
                    x_coef *= -1;
                    y_coef *= -1;
                }
                Location.x += x_coef*2.5f;
                Location.y += y_coef*5.f;

                PointPath path;
                path.resize(2);
                //path.set(0, champOrder%2 ? Jump2 : Jump1);
                path.set(0, encounterCreature2->GetPosition());
                path.set(1, Location);

                uint32 travelTime = path.GetTotalLength()/0.0025f;
                encounterCreature2->GetMotionMaster()->Clear(false, true);
                encounterCreature2->ChargeMonsterMove(path, SPLINETYPE_FACINGANGLE, SPLINEFLAG_WALKMODE,
                travelTime, faction == FACTION_ALLIANCE ? M_PI_F : 0);
                encounterCreature2 = NULL;

                // if is last one set phase handler to travelTime
                if (!timerCustom == CHAMPION_COUNT && !encounterCreature)
                {
                    customTimer->SetValue(TIMER_VALUE_DELETE_AT_FINISH, true);
                    m_TimerMgr->Cooldown(TIMER_PHASE_HANDLING, travelTime);
                    return;
                }
            }
            if (encounterCreature)
            {
                uint32 champOrder = customValue-1;
                Coords& jump = champOrder%2 ? Jump2 : Jump1;
                encounterCreature->GetMotionMaster()->Clear(false, true);
                encounterCreature->GetMotionMaster()->MoveIdle();
                encounterCreature->TrajMonsterMove(jump.x, jump.y, jump.z, false, 80, 1000);
                encounterCreature2 = encounterCreature;
                encounterCreature = NULL;
            }
            for(uint8 i = timerCustom; i < CHAMPION_COUNT; ++i)
            {
                customTimer->SetValue(TIMER_VALUE_CUSTOM, i+1);
                if (spawnMask & (1 << i))
                {
                    Coords& spawn = customValue%2 ? Spawn2 : Spawn1;
                    encounterCreature = DoSpawnTocBoss(FChampIDs[i][faction], spawn, 0);
                    ((ScriptedAI*)encounterCreature->AI())->EnableAttack(false);
                    break;
                }
            }
            ++customValue;
        }
        else
            customTimer->SetValue(TIMER_VALUE_DELETE_AT_FINISH, true);
    }
}

CreatureAI* GetAI_npc_toc_announcer(Creature* pCreature)
{
    return new npc_toc_announcerAI(pCreature);
}

bool GossipHello_npc_toc_announcer(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    if (!m_pInstance)
        return false;

    //bool isHeroic = pCreature->GetMap()->IsHeroicRaid();
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

bool GossipSelect_npc_toc_announcer(Player* pPlayer, Creature* pCreature, uint32 /*uiSender*/, uint32 uiAction)
{
    ScriptedInstance* m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    if (!m_pInstance || !pPlayer)
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
                    ((npc_toc_announcerAI*)pCreature->AI())->ChooseEvent(i, pPlayer);
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

bool GossipHello_npc_toc_argentmage(Player* pPlayer, Creature* pCreature)
{
    char const* _message = "Please port me up to the surface!";
    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _message, GOSSIP_SENDER_MAIN,GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_toc_argentmage(Player* pPlayer, Creature* , uint32 , uint32 uiAction)
{
    pPlayer->CLOSE_GOSSIP_MENU();

    if (uiAction != GOSSIP_ACTION_INFO_DEF+1)
        return true;

    pPlayer->TeleportTo(WorldLocation(pPlayer->GetMapId(), SpawnLoc[LOC_CENTER], 0));
    return true;
}

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
    NewScript->Name = "npc_toc_argentmage";
    NewScript->pGossipHello = &GossipHello_npc_toc_argentmage;
    NewScript->pGossipSelect = &GossipSelect_npc_toc_argentmage;
    NewScript->RegisterSelf();
}
