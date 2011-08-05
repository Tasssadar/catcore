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
SDName: trial_of_the_crusader
SD%Complete: 80%
SDComment: by /dev/rsa
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

// boss spells
enum Jaraxxus
{
    // Fel Fireball
    SPELL_FEL_FIREBALL/*_0*/        = 66532,
    //SPELL_FEL_FIREBALL_1        = 66963,
    //SPELL_FEL_FIREBALL_2        = 66964,
    //SPELL_FEL_FIREBALL_3        = 66965,

    // Fel Lightning
    SPELL_FEL_LIGHTNING/*_0*/       = 66528,
    //SPELL_FEL_LIGHTNING_1       = 67029,
    //SPELL_FEL_LIGHTNING_2       = 67030,
    //SPELL_FEL_LIGHTNING_3       = 67031,

    // Incinerate Flesh
    SPELL_INCINERATE_FLESH/*_0*/    = 66237,
    //SPELL_INCINERATE_FLESH_1    = 67049,
    //SPELL_INCINERATE_FLESH_2    = 67050,
    //SPELL_INCINERATE_FLESH_3    = 67051,

    // Legion Flame
    SPELL_LEGION_FLAME/*_0*/        = 66197,
    //SPELL_LEGION_FLAME_1        = 68123,
    //SPELL_LEGION_FLAME_2        = 68124,
    //SPELL_LEGION_FLAME_3        = 68125,

    // Infernal Eruption
    SPELL_INFERNAL_ERUPTION/*_0*/   = 66258,
    //SPELL_INFERNAL_ERUPTION_1   = 67901,
    //SPELL_INFERNAL_ERUPTION_2   = 67902,
    //SPELL_INFERNAL_ERUPTION_3   = 67903,

    // Nether Portal
    SPELL_NETHER_PORTAL/*_0*/       = 66269,
    //SPELL_NETHER_PORTAL_1       = 67898,
    //SPELL_NETHER_PORTAL_2       = 67899,
    //SPELL_NETHER_PORTAL_3       = 67900,

    // Nether Power
    SPELL_NETHER_POWER          = 67009,

    // Fel Lightning - gnom instakill
    SPELL_FEL_LIGHTNING_IK      = 67888

};

/*const uint32 m_idFelFireball[4] = {SPELL_FEL_FIREBALL_0, SPELL_FEL_FIREBALL_1, SPELL_FEL_FIREBALL_2, SPELL_FEL_FIREBALL_3};
const uint32 m_idFelLightning[4] = {SPELL_FEL_LIGHTNING_0, SPELL_FEL_LIGHTNING_1, SPELL_FEL_LIGHTNING_2, SPELL_FEL_LIGHTNING_3};
const uint32 m_idIncinerateFlesh[4] = {SPELL_INCINERATE_FLESH_0, SPELL_INCINERATE_FLESH_1, SPELL_INCINERATE_FLESH_2, SPELL_INCINERATE_FLESH_3};
const uint32 m_idLegionFlame[4] = {SPELL_LEGION_FLAME_0, SPELL_LEGION_FLAME_1, SPELL_LEGION_FLAME_2, SPELL_LEGION_FLAME_3};
const uint32 m_idInfernalEruption[4] = {SPELL_INFERNAL_ERUPTION_0, SPELL_INFERNAL_ERUPTION_1, SPELL_INFERNAL_ERUPTION_2, SPELL_INFERNAL_ERUPTION_3};
const uint32 m_idNetherPortal[4] = {SPELL_NETHER_PORTAL_0, SPELL_NETHER_PORTAL_1, SPELL_NETHER_PORTAL_2, SPELL_NETHER_PORTAL_3};*/

enum Adds
{
    // Infernal
    SPELL_FEL_STREAK            = 66494,
    SPELL_FEL_STREAK_M          = 66493,
    SPELL_FEL_INFERNO           = 67046,

    // Mistress
    SPELL_SHIVAN_SLASH/*_0*/        = 66378,
    //SPELL_SHIVAN_SLASH_1        = 67097,
    //SPELL_SHIVAN_SLASH_2        = 67098,
    //SPELL_SHIVAN_SLASH_3        = 67099,

    SPELL_SPINNING_PAIN_SPIKE   = 66283,

    SPELL_MISTRESS_KISS         = 66336
    //SPELL_MISTRESS_KISS_2       = 67077,
    //SPELL_MISTRESS_KISS_3       = 67078,

};

/*const uint32 m_idShivanSlash[4] = {SPELL_SHIVAN_SLASH_0, SPELL_SHIVAN_SLASH_1, SPELL_SHIVAN_SLASH_2, SPELL_SHIVAN_SLASH_3 };*/
/*const uint32 m_idMistressKiss[4] = {0,0, SPELL_MISTRESS_KISS_2, SPELL_MISTRESS_KISS_3 };*/

// should be all handled in core

enum Helpers
{
    // Legion Flame
    SPELL_LEGION_FLAME_AURA     = 66201,

    // Infernal Eruption
    SPELL_INFERNAL_ERUPTION_A/*_0*/ = 66252,
    //SPELL_INFERNAL_ERUPTION_A_1 = 67067,
    //SPELL_INFERNAL_ERUPTION_A_2 = 67068,
    //SPELL_INFERNAL_ERUPTION_A_3 = 67069,

    // from aura trigger : 66253
    // triggers missile: 66255

    // Nether Portal
    SPELL_NETHER_PORTAL_AURA/*_0*/  = 66263
    //SPELL_NETHER_PORTAL_AURA_1  = 67103,
    //SPELL_NETHER_PORTAL_AURA_2  = 67104,
    //SPELL_NETHER_PORTAL_AURA_3  = 67105
};

//const uint32 m_idInfernalEruptionAura[4] = {SPELL_INFERNAL_ERUPTION_A_0, SPELL_INFERNAL_ERUPTION_A_1, SPELL_INFERNAL_ERUPTION_A_2, SPELL_INFERNAL_ERUPTION_A_3};
//const uint32 m_idNetherPortalAura[4] = {SPELL_NETHER_PORTAL_AURA_0, SPELL_NETHER_PORTAL_AURA_1, SPELL_NETHER_PORTAL_AURA_2, SPELL_NETHER_PORTAL_AURA_3};

enum TimerNames
{
    // boss
    TIMER_FEL_FIRABALL=0,
    TIMER_FEL_LIGHTNING,
    TIMER_INCINERATE_FLESH,
    TIMER_LEGION_FLAME,
    TIMER_INFERNAL_ERUPTION,
    TIMER_NETHER_PORTAL,
    TIMER_NETHER_POWER,

    // infernal
    TIMER_FEL_STREAK,
    TIMER_FEL_INFERNO,

    // mistress
    TIMER_SHIVAN_SLASH,
    TIMER_SPINNING_PAIN_SPIKE,
    TIMER_MISTRESS_KISS
};

struct MANGOS_DLL_DECL boss_jaraxxusAI : public ScriptedAI
{
    boss_jaraxxusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;

    void Reset()
    {
        m_TimerMgr->Add(TIMER_FEL_FIRABALL, SPELL_FEL_FIREBALL, urand(4000,8000), urand(10000,15000));
        m_TimerMgr->Add(TIMER_FEL_LIGHTNING, SPELL_FEL_LIGHTNING, urand(6000,10000), urand(13000,17000));
        m_TimerMgr->Add(TIMER_INCINERATE_FLESH, SPELL_INCINERATE_FLESH, urand(13000,15000), urand(18000,22000));
        m_TimerMgr->Add(TIMER_LEGION_FLAME, SPELL_LEGION_FLAME, urand(10000,15000), urand(25000,30000));
        m_TimerMgr->Add(TIMER_INFERNAL_ERUPTION, SPELL_INFERNAL_ERUPTION, urand(70000,90000), urand(110000,120000));
        m_TimerMgr->Add(TIMER_NETHER_PORTAL, SPELL_NETHER_PORTAL, urand(15000,25000), urand(110000,120000));
        m_TimerMgr->Add(TIMER_NETHER_POWER, SPELL_NETHER_POWER, 2000, 42000);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        //m_pInstance->SetData(TYPE_NORTHREND_BEASTS, GORMOK_DONE);
    }

    void JustReachedHome()
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_NORTHREND_BEASTS, FAIL);
        m_creature->ForcedDespawn();
    }

    void Aggro(Unit* pWho)
    {
        //m_pInstance->SetData(TYPE_NORTHREND_BEASTS, GORMOK_IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        m_TimerMgr->Update(uiDiff);

        bool isCasting = m_creature->IsNonMeleeSpellCasted(false);

        // Fel Fireball
        if (m_TimerMgr->IsReady(TIMER_FEL_FIRABALL, isCasting))
        {
            m_creature->CastSpell(m_creature->getVictim(), m_TimerMgr->GetSpellId(TIMER_FEL_FIRABALL), false);
            m_TimerMgr->AddCooldown(TIMER_FEL_FIRABALL);
        }

        // Fel Lightning
        if (m_TimerMgr->IsReady(TIMER_FEL_LIGHTNING, isCasting))
        {
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                m_creature->CastSpell(plr, m_TimerMgr->GetSpellId(TIMER_FEL_LIGHTNING), false);
            m_TimerMgr->AddCooldown(TIMER_FEL_LIGHTNING);
        }

        // Incinerate Flesh
        if (m_TimerMgr->IsReady(TIMER_INCINERATE_FLESH, isCasting))
        {
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                m_creature->CastSpell(plr, m_TimerMgr->GetSpellId(TIMER_INCINERATE_FLESH), false);
            m_TimerMgr->AddCooldown(TIMER_INCINERATE_FLESH);
        }

        // Legion Flame
        if (m_TimerMgr->IsReady(TIMER_LEGION_FLAME, isCasting))
        {
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                m_creature->CastSpell(plr, m_TimerMgr->GetSpellId(TIMER_LEGION_FLAME), false);
            m_TimerMgr->AddCooldown(TIMER_LEGION_FLAME);
        }

        // Infernal Eruption
        if (m_TimerMgr->IsReady(TIMER_INFERNAL_ERUPTION, isCasting))
        {
            m_creature->CastSpell(m_creature, m_TimerMgr->GetSpellId(TIMER_INFERNAL_ERUPTION), false);
            m_TimerMgr->AddCooldown(TIMER_INFERNAL_ERUPTION);
        }

        // Nether Portal
        if (m_TimerMgr->IsReady(TIMER_NETHER_PORTAL, isCasting))
        {
            m_creature->CastSpell(m_creature, m_TimerMgr->GetSpellId(TIMER_NETHER_PORTAL), false);
            m_TimerMgr->AddCooldown(TIMER_NETHER_PORTAL);
        }

        // Nether Power
        if (m_TimerMgr->IsReady(TIMER_NETHER_POWER, isCasting))
        {
            m_creature->CastSpell(m_creature, m_TimerMgr->GetSpellId(TIMER_NETHER_POWER), false);
            m_TimerMgr->AddCooldown(TIMER_NETHER_POWER);
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_felflame_infernalAI : public ScriptedAI
{
    npc_felflame_infernalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;

    void Reset()
    {
        m_TimerMgr->Add(TIMER_FEL_STREAK, SPELL_FEL_STREAK, urand(8000,10000), urand(25000,30000));
        m_TimerMgr->Add(TIMER_FEL_INFERNO, SPELL_FEL_INFERNO, urand(10000,20000), urand(15000,25000));
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        m_TimerMgr->Update(uiDiff);

        bool isCasting = m_creature->IsNonMeleeSpellCasted(false);

        // Fel Streak
        if (m_TimerMgr->IsReady(TIMER_FEL_STREAK, isCasting))
        {
            m_creature->CastSpell(m_creature, SPELL_FEL_STREAK_M, false);
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1))
                m_creature->CastSpell(plr, m_TimerMgr->GetSpellId(TIMER_FEL_STREAK), true);
            m_TimerMgr->AddCooldown(TIMER_FEL_STREAK);
        }

        // Fel Inferno
        if (m_TimerMgr->IsReady(TIMER_FEL_INFERNO, isCasting))
        {
            m_creature->CastSpell(m_creature, m_TimerMgr->GetSpellId(TIMER_FEL_INFERNO), false);
            m_TimerMgr->AddCooldown(TIMER_FEL_INFERNO);
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_mistress_of_painAI : public ScriptedAI
{
    npc_mistress_of_painAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;

    void Reset()
    {
        m_TimerMgr->Add(TIMER_SHIVAN_SLASH, SPELL_SHIVAN_SLASH, 15000, 15000);
        m_TimerMgr->Add(TIMER_SPINNING_PAIN_SPIKE, SPELL_SPINNING_PAIN_SPIKE, 16000, 16000);
        if (m_dDifficulty == RAID_DIFFICULTY_10MAN_HEROIC || m_dDifficulty == RAID_DIFFICULTY_25MAN_HEROIC)
            m_TimerMgr->Add(TIMER_SHIVAN_SLASH, SPELL_SHIVAN_SLASH, 15000, 15000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        m_TimerMgr->Update(uiDiff);

        bool isCasting = m_creature->IsNonMeleeSpellCasted(false);

        // Shivan Slash
        if (m_TimerMgr->IsReady(TIMER_SHIVAN_SLASH, isCasting))
        {
            m_creature->CastSpell(m_creature->getVictim(), m_TimerMgr->GetSpellId(TIMER_SHIVAN_SLASH), false);
            m_TimerMgr->AddCooldown(TIMER_SHIVAN_SLASH);
        }

        // Spinning Pain Spike
        if (m_TimerMgr->IsReady(TIMER_SPINNING_PAIN_SPIKE, isCasting))
        {
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                m_creature->CastSpell(plr, m_TimerMgr->GetSpellId(TIMER_SPINNING_PAIN_SPIKE), false);
            m_TimerMgr->AddCooldown(TIMER_SPINNING_PAIN_SPIKE);
        }

        // Mistress's Kiss
        if (m_TimerMgr->IsReady(TIMER_MISTRESS_KISS, isCasting))
        {
            PlrList fullList = GetAttackingPlayers(false);

            PlrList manaList;
            for(PlrList::iterator itr = fullList.begin(); itr != fullList.end(); ++itr)
                if ((*itr)->getPowerType() == POWER_MANA)
                    manaList.push_back(*itr);

            PlrList::iterator itr = manaList.begin();
            std::advance(itr, urand(0, manaList.size()-1));

            if (Unit* target = *itr)
            {
                m_creature->CastSpell(target, m_TimerMgr->GetSpellId(TIMER_MISTRESS_KISS), false);
                m_TimerMgr->AddCooldown(TIMER_MISTRESS_KISS);
            }
        }

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_jaraxxus(Creature* pCreature)
{
    return new boss_jaraxxusAI(pCreature);
}

CreatureAI* GetAI_felflame_infernal(Creature* pCreature)
{
    return new npc_felflame_infernalAI(pCreature);
}

CreatureAI* GetAI_mistress_of_pain(Creature* pCreature)
{
    return new npc_mistress_of_painAI(pCreature);
}

void AddSC_boss_jaraxxus()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_jaraxxus";
    newscript->GetAI = &GetAI_boss_jaraxxus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_felflame_infernal";
    newscript->GetAI = &GetAI_felflame_infernal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_mistress_of_pain";
    newscript->GetAI = &GetAI_mistress_of_pain;
    newscript->RegisterSelf();
}

/*
UPDATE creature_template SET ScriptName = 'boss_jaraxxus' WHERE entry=34780;
UPDATE creature_template SET ScriptName = 'npc_felflame_infernal' WHERE entry=34815;
UPDATE creature_template SET ScriptName = 'npc_mistress_of_pain' WHERE entry=34826;
*/
