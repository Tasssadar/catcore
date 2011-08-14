/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_algalon
SD%Complete: 0%
SDComment:
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"
#include "TemporarySummon.h"
#include "escort_ai.h"

enum spells
{
    //Algalon's abillities
    SPELL_QUANTUMSTRIKE = 64395,
    SPELL_BIGBANG = 64443,
    SPELL_PHASE_PUNCH = 64412,
    SPELL_PHASE_SHIFT = 64417,
    SPELL_ALGALONS_BERSERK = 47008,
    SPELL_COSMIC_SMASH_MISSILE = 62304,
    SPELL_COSMIC_SMASH_VISUAL = 62300,
    SPELL_COSMIC_SMASH_DAMAGE = 62311,
    SPELL_DUAL_WIELD = 42459,

    //Black hole's abillities
    SPELL_BLACK_HOLE_EXPLOSION = 64122,
    SPELL_CONSTELLATION_PHASE_EFFECT = 65509,

    //living constellation abillities
    SPELL_ARCANE_BARRAGE = 64599
};

enum NPCs
{
    NPC_COLLAPSING_STAR = 32955,
    NPC_BLACK_HOLE = 32953,
    NPC_VOID_ZONE = 34100,
    NPC_LIVING_CONSTELLATION = 33052,
    NPC_UNLEASHED_DARK_MATTER = 34097,
    NPC_MOB_ALGALON_STALKER_ASTEROID_TARGET_01 = 33104,
    NPC_MOB_ALGALON_STALKER_ASTEROID_TARGET_02 = 33105,
    NPC_BRANN_BRONZEBEARD = 34064
};

enum GameObjects
{
    GO_SIGIL_DOOR_02 = 194911
};

enum Says
{
    SAY_INTRO_1                         = -1603106,
    SAY_INTRO_2                         = -1603107,
    SAY_INTRO_3                         = -1603108,

    SAY_ENGAGE                          = -1603109,
    SAY_AGGRO                           = -1603110,
    SAY_SLAY_1                          = -1603111,
    SAY_SLAY_2                          = -1603112,
    SAY_SUMMON_STAR                     = -1603113,
    SAY_BIG_BANG_1                      = -1603114,
    SAY_BIG_BANG_2                      = -1603115,
    SAY_PHASE_2                         = -1603116,
    SAY_BERSERK                         = -1603117,

    SAY_DESPAWN_1                       = -1603118,
    SAY_DESPAWN_2                       = -1603119,
    SAY_DESPAWN_3                       = -1603120,

    SAY_OUTRO_1                         = -1603121,
    SAY_OUTRO_2                         = -1603122,
    SAY_OUTRO_3                         = -1603123,
    SAY_OUTRO_4                         = -1603124,
    SAY_OUTRO_5                         = -1603125,
};

/************************************************ Algalon ***************************************************/

struct MANGOS_DLL_DECL boss_algalonAI : public ScriptedAI
{
    instance_ulduar* m_pInstance;

    //Creatures lists
    GUIDList m_lLivingConstelationsGUIDs;
    GUIDList m_lCollapsingStarsGUIDs;
    GUIDList m_lDarkMattersGUIDs;
    std::list<Creature*> lBlackHoles;
    std::list<Creature*> lVoidZones;

    //Algalon's abillities timers
    bool m_uiPhaseTwo;

    uint32 m_uiQuantumStrikeTimer;
    uint32 m_uiBigBangTimer;
    uint32 m_uiBigBangPhaseOutEffectTimer;
    uint32 m_uiPhasePunchTimer;
    uint32 m_uiAlgalonsBerserkTimer;
    uint32 m_uiCosmicSmashTimer;
    uint32 m_uiDualWieldTimer;

    uint32 m_uiSummonCollapsingStarTimer;
    uint32 m_uiSummonLivingConstellationTimer;

    //Black hole's abillities timers
    uint32 m_uiSummonUnleashedDarkMatterTimer;

    void Reset()
    {
        m_uiPhaseTwo = false;

        m_uiQuantumStrikeTimer = urand(3000, 6000);
        m_uiBigBangTimer = 90000;
        m_uiBigBangPhaseOutEffectTimer = 98100;
        m_uiPhasePunchTimer = 15000;
        m_uiSummonCollapsingStarTimer = 15000;
        m_uiSummonLivingConstellationTimer = 50000;
        m_uiSummonUnleashedDarkMatterTimer = 5000;
        m_uiCosmicSmashTimer = 25000;
        m_uiAlgalonsBerserkTimer = 360000;
        m_uiDualWieldTimer = 900;
    }

    boss_algalonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    void DespawnAll()
    {
        for (GUIDList::const_iterator itr = m_lCollapsingStarsGUIDs.begin(); itr != m_lCollapsingStarsGUIDs.end(); itr++)
            if (Creature* temp_star = m_creature->GetMap()->GetCreature(*itr))
                ((TemporarySummon*)temp_star)->UnSummon();
        m_lCollapsingStarsGUIDs.clear();

        for (GUIDList::const_iterator itr = m_lLivingConstelationsGUIDs.begin(); itr != m_lLivingConstelationsGUIDs.end(); itr++)
            if (Creature* temp_constellation = m_creature->GetMap()->GetCreature(*itr))
                ((TemporarySummon*)temp_constellation)->UnSummon();
        m_lLivingConstelationsGUIDs.clear();

        for (GUIDList::const_iterator itr = m_lDarkMattersGUIDs.begin(); itr != m_lDarkMattersGUIDs.end(); itr++)
            if (Creature* temp_dark_matter = m_creature->GetMap()->GetCreature(*itr))
                ((TemporarySummon*)temp_dark_matter)->UnSummon();
        m_lDarkMattersGUIDs.clear();

        GetCreatureListWithEntryInGrid(lBlackHoles, m_creature, NPC_BLACK_HOLE, 100.0f);
        for (std::list<Creature*>::iterator itr = lBlackHoles.begin(); itr != lBlackHoles.end(); itr++)
            ((TemporarySummon*)(*itr))->UnSummon();
        lBlackHoles.clear();

        GetCreatureListWithEntryInGrid(lVoidZones, m_creature, NPC_VOID_ZONE, 100.0f);
        for (std::list<Creature*>::iterator itr = lVoidZones.begin(); itr != lVoidZones.end(); itr++)
            ((TemporarySummon*)(*itr))->UnSummon();
        lVoidZones.clear();
    }

    void PhaseOutAll()
    {
        Map::PlayerList const& lPlayers = m_pInstance->instance->GetPlayers();
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            if (Player* pPlayer = itr->getSource())
                pPlayer->SetPhaseMask(1, true);
    }

    void JustDied(Unit* pKiller)
    {
        GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_AZEROTH_GLOBE, INTERACTION_DISTANCE);
        if (pGo)
            pGo->SetGoState(GO_STATE_READY);
        DespawnAll();
        PhaseOutAll();
        m_pInstance->SetData(TYPE_ALGALON, DONE);
    }

    void Aggro(Unit* pWho)
    {
        GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_AZEROTH_GLOBE, 200);
        if (pGo)
            pGo->SetGoState(GO_STATE_ACTIVE);

    /*   pGo = GetClosestGameObjectWithEntry(m_creature, GO_UNIVERSE_FLOOR_ARCHIVUM, 200);
        if (pGo)
            pGo->SetGoState(GO_STATE_READY);

        pGo = GetClosestGameObjectWithEntry(m_creature, GO_UNIVERSE_FLOOR_CELESTIAL, 200);
        if (pGo)
            pGo->SetGoState(GO_STATE_ACTIVE);*/

        m_pInstance->SetData(TYPE_ALGALON, IN_PROGRESS);
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustReachedHome()
    {
        GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_AZEROTH_GLOBE, INTERACTION_DISTANCE);
        if (pGo)
            pGo->SetGoState(GO_STATE_READY);
        DespawnAll();
        PhaseOutAll();
        m_pInstance->SetData(TYPE_ALGALON, FAIL);
    }

    void KilledUnit()
    {
        switch (rand()%2)
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Dual wield
        if (m_uiDualWieldTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_DUAL_WIELD);
            m_uiDualWieldTimer = 1800;
        }
        else
            m_uiDualWieldTimer -=uiDiff;

        //Phase 2
        if (!m_uiPhaseTwo && m_creature->GetHealthPercent() < 20.0f)
        {
            m_uiPhaseTwo = true;
            DoScriptText(SAY_PHASE_2, m_creature);
            DespawnAll();
            Creature* temp_black_hole = NULL;
            Creature* temp_void_zone = NULL;

            float angle = 0;
            for (uint8 i = 0; i < 4; ++i)
            {
                angle += M_PI_F/2;
                float x = 1632.25f + cos(angle)*15;
                float y = -307.548f + sin(angle)*15;
                m_creature->SummonCreature(NPC_BLACK_HOLE, x, y, 417.327f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                m_creature->SummonCreature(NPC_VOID_ZONE, x, y, 417.327f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
            }
        }

        //Berserk
        if (m_uiAlgalonsBerserkTimer < uiDiff)
        {
            DoScriptText(SAY_BERSERK, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_ALGALONS_BERSERK);
            m_uiAlgalonsBerserkTimer = 360000;
        }
        else
            m_uiAlgalonsBerserkTimer -= uiDiff;

        //Quantum strike
        if (m_uiQuantumStrikeTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_QUANTUMSTRIKE);
            m_uiQuantumStrikeTimer = urand(3000, 6000);
        }
        else
             m_uiQuantumStrikeTimer -= uiDiff;

        //Big Bang
        if (m_uiBigBangTimer < uiDiff)
        {
            switch (rand()%2)
            {
                case 0: DoScriptText(SAY_BIG_BANG_1, m_creature); break;
                case 1: DoScriptText(SAY_BIG_BANG_2, m_creature); break;
            }
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_BIGBANG);
            m_uiBigBangTimer = 90000;
        }
        else
            m_uiBigBangTimer -= uiDiff;

        if (m_uiBigBangPhaseOutEffectTimer < uiDiff)
        {
            PhaseOutAll();
            m_uiBigBangPhaseOutEffectTimer = 90000;
        }
        else
            m_uiBigBangPhaseOutEffectTimer -= uiDiff;


        //Phase Punch
        if (m_uiPhasePunchTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_PHASE_PUNCH);
            m_uiPhasePunchTimer = 15000;
        }
        else
            m_uiPhasePunchTimer -= uiDiff;

        //Cosmic Smash
        if (m_uiCosmicSmashTimer < uiDiff)
        {
            Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            m_creature->SummonCreature(NPC_MOB_ALGALON_STALKER_ASTEROID_TARGET_02,
                target->GetPositionX(), target->GetPositionY(), 417.327f, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 5500);
            m_uiCosmicSmashTimer = 25000;
        }
        else
            m_uiCosmicSmashTimer -= uiDiff;

        //Summon Collapsing Stars
        if (!m_uiPhaseTwo)
        {
            if (m_uiSummonCollapsingStarTimer < uiDiff)
            {
                DoScriptText(SAY_SUMMON_STAR, m_creature);
                float angle = 0;
                for (uint32 i= 0; i<4; i++)
                {
                    angle += M_PI_F/2;
                    float x = 1632.25f + cos(angle) * urand(10, 38);
                    float y = -307.548f + sin(angle) * urand(10, 38);

                    Creature* temp_creature = m_creature->SummonCreature(NPC_COLLAPSING_STAR, x, y, 417.327f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    m_lCollapsingStarsGUIDs.push_back(temp_creature->GetObjectGuid());
                }
                m_uiSummonCollapsingStarTimer=60000;
            }
            else
                m_uiSummonCollapsingStarTimer -= uiDiff;
        }

        //Summon Living Constellation
        if (!m_uiPhaseTwo)
        {
            if (m_uiSummonLivingConstellationTimer < uiDiff)
            {
                float angle = 0;
                for (uint8 i = 0; i < 4; ++i)
                {
                    angle += M_PI_F/2;
                    float x = 1632.25f + cos(angle)*38;
                    float y = -307.548f + sin(angle)*38;

                    Creature* temp_constellation = m_creature->SummonCreature(NPC_LIVING_CONSTELLATION, x, y, 417.327f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
                    temp_constellation->AI()->AttackStart(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));
                    m_lLivingConstelationsGUIDs.push_back(temp_constellation->GetObjectGuid());
                }
                m_uiSummonLivingConstellationTimer=60000;
            }
            else
                m_uiSummonLivingConstellationTimer -= uiDiff;
        }


        //Summon Unleashed Dark Matter
        if (m_uiPhaseTwo)
        {
            if (m_uiSummonUnleashedDarkMatterTimer < uiDiff)
            {
                float angle = 0;
                for (uint8 i = 0; i < 4; ++i)
                {
                    angle += M_PI_F/2;
                    float x = 1632.25f + cos(angle)*15;
                    float y = -307.548f + sin(angle)*15;
                    Creature* temp_dark_matter = m_creature->SummonCreature(NPC_UNLEASHED_DARK_MATTER, x, y, 417.327f, 0.0f, TEMPSUMMON_DEAD_DESPAWN, 0);
                    temp_dark_matter->AI()->AttackStart(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));
                    m_lDarkMattersGUIDs.push_back(temp_dark_matter->GetObjectGuid());
                }
                m_uiSummonUnleashedDarkMatterTimer = 30000;
            }
            else
                m_uiSummonUnleashedDarkMatterTimer -= uiDiff;
        }

        //Update threat list
        ThreatList const& tList = m_creature->getThreatManager().getThreatList();
        for (ThreatList::const_iterator itr = tList.begin();itr != tList.end(); ++itr)
        {
            Unit* pUnit = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid());

            if (pUnit && m_creature->getThreatManager().getThreat(pUnit))
                if (pUnit->GetPhaseMask() == 16)
                    m_creature->getThreatManager().modifyThreatPercent(pUnit, -100);
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_algalon(Creature* pCreature) 
{
    return new boss_algalonAI(pCreature);
}

/****************************************** Collapsing Star *************************************************/

struct MANGOS_DLL_DECL mob_collapsing_starAI : public ScriptedAI
{
    uint32 m_uiSetHealthTimer;

    mob_collapsing_starAI(Creature* pCreature) : ScriptedAI(pCreature)  
    {
        Reset();
    }

    void Reset()
    {
        m_uiSetHealthTimer = 1000;
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->SummonCreature(NPC_BLACK_HOLE, m_creature->GetPositionX(), m_creature->GetPositionY(), 417.327f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        m_creature->SummonCreature(NPC_VOID_ZONE, m_creature->GetPositionX(), m_creature->GetPositionY(), 417.327f, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
        ((TemporarySummon*)m_creature)->UnSummon();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiSetHealthTimer < uiDiff)
        {
            m_creature->DealDamage(m_creature, 882, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            m_uiSetHealthTimer = 1000;
        }
        else
            m_uiSetHealthTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_collapsing_star(Creature* pCreature)
{
    return new mob_collapsing_starAI(pCreature);
}

/****************************************** Living Constellation ********************************************/

struct MANGOS_DLL_DECL mob_living_constellationAI : public ScriptedAI
{
    uint32 m_uiArcaneBarrageTimer;

    mob_living_constellationAI (Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        m_uiArcaneBarrageTimer = urand(5000,10000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiArcaneBarrageTimer < uiDiff)
        {
            Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            if (target)
                DoCastSpellIfCan(target, SPELL_ARCANE_BARRAGE);
            m_uiArcaneBarrageTimer = urand(5000,10000);
        }
        else
            m_uiArcaneBarrageTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_living_constellation(Creature* pCreature)
{
    return new mob_living_constellationAI(pCreature);
}

/****************************************** Black Hole ********************************************************/

struct MANGOS_DLL_DECL mob_black_holeAI : public ScriptedAI
{
    uint32 m_uiConstellationPhaseEffectTimer;

    mob_black_holeAI (Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        DoCastSpellIfCan(m_creature, SPELL_BLACK_HOLE_EXPLOSION);
    }

    void Reset()
    {
        m_uiConstellationPhaseEffectTimer = 1000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiConstellationPhaseEffectTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_CONSTELLATION_PHASE_EFFECT);
            Unit* target = GetClosestCreatureWithEntry(m_creature, NPC_LIVING_CONSTELLATION, 5.0f);
            if (target)
            {
                Unit* tempVoidZone = GetClosestCreatureWithEntry(m_creature, NPC_VOID_ZONE, 5.0f);
                ((TemporarySummon*)target)->UnSummon();
                ((TemporarySummon*)tempVoidZone)->UnSummon();
                ((TemporarySummon*)m_creature)->UnSummon();
            }
            m_uiConstellationPhaseEffectTimer = 1000;
        }
        else
            m_uiConstellationPhaseEffectTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_black_hole(Creature* pCreature)
{
    return new mob_black_holeAI(pCreature);
}

/****************************************** Meteor Stalker 02 *************************************************/

struct MANGOS_DLL_DECL mob_Algalon_Stalker_Asteroid_Target_02_AI : public ScriptedAI
{
    uint32 m_uiCosmicSmashDamageTimer;

    mob_Algalon_Stalker_Asteroid_Target_02_AI (Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        m_uiCosmicSmashDamageTimer = 5000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiCosmicSmashDamageTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_COSMIC_SMASH_DAMAGE);
            m_uiCosmicSmashDamageTimer = 5000;
        }
        else
            m_uiCosmicSmashDamageTimer -= uiDiff;

    }
};

CreatureAI* GetAI_mob_Algalon_Stalker_Asteroid_Target_02_AI(Creature* pCreature)
{
    return new mob_Algalon_Stalker_Asteroid_Target_02_AI(pCreature);
}

/****************************************** Doors ***********************************************************/

bool GOUse_go_Sigil_Door_01(Player* pPlayer, GameObject* pGo)
{
    GameObject* sigilDoor02 = GetClosestGameObjectWithEntry(pGo, GO_SIGIL_DOOR_02, 100.0f);
    sigilDoor02->Use(pPlayer);

    Creature* branBronzebeard = GetClosestCreatureWithEntry(pGo, NPC_BRANN_BRONZEBEARD, 200.0f);
    branBronzebeard->SetVisibility(VISIBILITY_ON);
    branBronzebeard->GetMotionMaster()->MoveWaypoint();

    return false;
}

/****************************************** Brann Bronzebeard ***********************************************/

struct MANGOS_DLL_DECL mob_Brann_Bronzebeard_AI : public ScriptedAI
{
    mob_Brann_Bronzebeard_AI (Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_creature->SetVisibility(VISIBILITY_OFF);
    }

    void Reset()
    {
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        switch (uiPointId)
        {
            case 6:
                m_creature->SummonCreature(NPC_ALGALON, 1632.25f, -307.548f, 417.327f, 1.5f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                break;

            case 11:
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->SetVisibility(VISIBILITY_OFF);
                break;
        }
    }
};

CreatureAI* GetAI_mob_Brann_Bronzebeard_AI(Creature* pCreature)
{
    return new mob_Brann_Bronzebeard_AI(pCreature);
}

/****************************************** Add scripts *****************************************************/

void AddSC_boss_algalon()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_algalon";
    NewScript->GetAI = &GetAI_boss_algalon;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_collapsing_star";
    NewScript->GetAI = &GetAI_mob_collapsing_star;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_living_constellation";
    NewScript->GetAI = &GetAI_mob_living_constellation;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_black_hole";
    NewScript->GetAI = &GetAI_mob_black_hole;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_Algalon_Stalker_Asteroid_Target_02";
    NewScript->GetAI = &GetAI_mob_Algalon_Stalker_Asteroid_Target_02_AI;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "go_sigil_door_01";
    NewScript->pGOUse = &GOUse_go_Sigil_Door_01;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_Brann_Bronzebeard";
    NewScript->GetAI = &GetAI_mob_Brann_Bronzebeard_AI;
    NewScript->RegisterSelf();
}
