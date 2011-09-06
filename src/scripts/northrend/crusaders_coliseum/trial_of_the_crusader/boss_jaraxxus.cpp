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

enum Say
{
    //SAY_INTRO               = -1649030,
    //SAY_AGGRO               = -1649031,
    SAY_DEATH               = -1649032,
    EMOTE_INCINERATE        = -1649033,
    SAY_INCINERATE          = -1649034,
    EMOTE_LEGION_FLAME      = -1649035,
    EMOTE_NETHER_PORTAL     = -1649036,
    SAY_NETHER_PORTAL       = -1649037,
    EMOTE_INFERNAL_ERUPTION = -1649038,
    SAY_INFERNAL_ERUPTION   = -1649039
};

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

// boss spells
enum Jaraxxus
{
    SPELL_FEL_FIREBALL          = 66532, // Fel Fireball
    SPELL_FEL_LIGHTNING         = 66528, // Fel Lightning
    SPELL_INCINERATE_FLESH      = 66237, // Incinerate Flesh
    SPELL_LEGION_FLAME          = 66197, // Legion Flame
    SPELL_INFERNAL_ERUPTION     = 66258, // Infernal Eruption
    SPELL_NETHER_PORTAL         = 66269, // Nether Portal
    SPELL_NETHER_POWER          = 67009, // Nether Power
};

enum Adds
{
    // Infernal
    SPELL_FEL_STREAK            = 66494,
    SPELL_FEL_STREAK_M          = 66493,
    SPELL_FEL_INFERNO           = 67046,

    // Mistress
    SPELL_SHIVAN_SLASH          = 66378,
    SPELL_SPINNING_PAIN_SPIKE   = 66283,
    SPELL_MISTRESS_KISS         = 66336
};

enum Helpers
{
    SPELL_LEGION_FLAME_AURA     = 66201, // Legion Flame
    SPELL_INFERNAL_ERUPTION_AURA= 66252, // Infernal Eruption
    SPELL_NETHER_PORTAL_AURA    = 66263  // Nether Portal

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
        AddTimer(TIMER_FEL_FIRABALL, SPELL_FEL_FIREBALL, urand(4000,8000), urand(10000,15000), UNIT_SELECT_VICTIM);
        AddTimer(TIMER_FEL_LIGHTNING, SPELL_FEL_LIGHTNING, urand(6000,10000), urand(13000,17000), UNIT_SELECT_RANDOM_PLAYER, CAST_TYPE_QUEUE, 0);
        AddTimer(TIMER_INCINERATE_FLESH, SPELL_INCINERATE_FLESH, urand(13000,15000), urand(18000,22000), UNIT_SELECT_RANDOM_PLAYER, CAST_TYPE_QUEUE, 0);
        AddTimer(TIMER_LEGION_FLAME, SPELL_LEGION_FLAME, urand(10000,15000), urand(25000,30000), UNIT_SELECT_RANDOM_PLAYER, CAST_TYPE_QUEUE, 0);
        AddTimer(TIMER_INFERNAL_ERUPTION, SPELL_INFERNAL_ERUPTION, urand(70000,90000), urand(110000,120000), UNIT_SELECT_SELF);
        AddTimer(TIMER_NETHER_PORTAL, SPELL_NETHER_PORTAL, urand(15000,25000), urand(110000,120000), UNIT_SELECT_SELF);
        AddTimer(TIMER_NETHER_POWER, SPELL_NETHER_POWER, 2000, 42000, UNIT_SELECT_SELF);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        EnableAttack(false);
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_JARAXXUS, DONE);
    }

    void JustReachedHome()
    {
        //m_creature->ForcedDespawn();

        if (m_pInstance && m_pInstance->GetData(TYPE_JARAXXUS) == IN_PROGRESS)
        {
            m_pInstance->SetData(TYPE_JARAXXUS, FAIL);

            m_creature->CastSpell(m_creature, SPELL_JARAXXUS_CHAINS, false);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
    }

    void Aggro(Unit*)
    {
        //DoScriptText(SAY_AGGRO, m_creature);

        //if (m_pInstance)
        //    m_pInstance->SetData(TYPE_JARAXXUS, IN_PROGRESS);
    }

    void UpdateAI(const uint32 )
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Fel Fireball
        m_TimerMgr->TimerFinished(TIMER_FEL_FIRABALL);

        // Fel Lightning
        m_TimerMgr->TimerFinished(TIMER_FEL_LIGHTNING);

        // Incinerate Flesh
        if (SpellTimer* timer = m_TimerMgr->TimerFinished(TIMER_INCINERATE_FLESH))
        {
            if (Unit* target = timer->getTarget())
                DoScriptText(EMOTE_INCINERATE, m_creature, target);

            DoScriptText(SAY_INCINERATE, m_creature);
        }


        // Legion Flame
        if (SpellTimer* timer = m_TimerMgr->TimerFinished(TIMER_LEGION_FLAME))
        {
            if (Unit* target = timer->getTarget())
                DoScriptText(EMOTE_LEGION_FLAME, m_creature, target);
        }

        // Infernal Eruption
        if (m_TimerMgr->TimerFinished(TIMER_INFERNAL_ERUPTION))
        {
            DoScriptText(EMOTE_INFERNAL_ERUPTION, m_creature);
            DoScriptText(SAY_INFERNAL_ERUPTION, m_creature);
        }

        // Nether Portal
        if (m_TimerMgr->TimerFinished(TIMER_NETHER_PORTAL))
        {
            DoScriptText(EMOTE_NETHER_PORTAL, m_creature);
            DoScriptText(SAY_NETHER_PORTAL, m_creature);
        }

        // Nether Power
        m_TimerMgr->TimerFinished(TIMER_NETHER_POWER);

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
        AddTimer(TIMER_FEL_STREAK, SPELL_FEL_STREAK, urand(8000,10000), urand(25000,30000), UNIT_SELECT_RANDOM_PLAYER, CAST_TYPE_NONCAST, 1);
        AddTimer(TIMER_FEL_INFERNO, SPELL_FEL_INFERNO, urand(10000,20000), urand(15000,25000), UNIT_SELECT_SELF);
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Fel Streak
        if (m_TimerMgr->TimerFinished(TIMER_FEL_STREAK))
            m_creature->CastSpell(m_creature, SPELL_FEL_STREAK_M, false);

        // Fel Inferno
        m_TimerMgr->TimerFinished(TIMER_FEL_INFERNO);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_mistress_of_painAI : public ScriptedAI
{
    npc_mistress_of_painAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        isHeroic = pCreature->GetMap()->IsHeroicRaid();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;
    bool isHeroic;

    void Reset()
    {
        AddTimer(TIMER_SHIVAN_SLASH, SPELL_SHIVAN_SLASH, 15000, 15000, UNIT_SELECT_VICTIM);
        AddTimer(TIMER_SPINNING_PAIN_SPIKE, SPELL_SPINNING_PAIN_SPIKE, 16000, 16000, UNIT_SELECT_RANDOM_PLAYER);
        if (isHeroic)
            AddTimer(TIMER_MISTRESS_KISS, SPELL_MISTRESS_KISS, 15000, 15000, UNIT_SELECT_NONE, CAST_TYPE_FORCE);
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Shivan Slash
        m_TimerMgr->TimerFinished(TIMER_SHIVAN_SLASH);

        // Spinning Pain Spike
        m_TimerMgr->TimerFinished(TIMER_SPINNING_PAIN_SPIKE);

        // Mistress's Kiss
        if (m_TimerMgr->IsReady(TIMER_MISTRESS_KISS))
        {
            PlrList fullList = GetAttackingPlayers(false);

            PlrList manaList;
            for(PlrList::iterator itr = fullList.begin(); itr != fullList.end(); ++itr)
                if ((*itr)->getPowerType() == POWER_MANA)
                    manaList.push_back(*itr);

            PlrList::iterator itr = manaList.begin();
            std::advance(itr, urand(0, manaList.size()-1));

            m_TimerMgr->FinishTimer(TIMER_MISTRESS_KISS, *itr);
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
