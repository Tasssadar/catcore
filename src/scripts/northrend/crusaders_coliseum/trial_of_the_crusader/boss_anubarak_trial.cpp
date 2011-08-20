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
SDName: boss_anubarak_trial
SD%Complete: 70%
SDComment: by /dev/rsa
SDCategory:
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Timers
{
    TIMER_SLASH = 0,
    TIMER_COLD,
    TIMER_PHASE,


    TIMER_SPIKES
};

enum Spells
{
    // anubarak
    BOSS_ANUBARAK           = 34564,
    SPELL_FREEZING_SLASH    = 66012, // phase 1,3 ---- just timer, target victim
    SPELL_PENETRATING_COLD  = 66013, // phase 1,3 ---- handled in core, just timer, target self
    SPELL_LEECHING_SWARM    = 66118, // phase 3 -- just timer, casted on self, queued on start of phase 3
    //66170 -- effect teleport, implicit script, implicit to spike npc, used to keep up boss with spikes (atleast i hope so)
    //SPELL_LEECHING_HEAL     = 66125,
    //SPELL_LEECHING_DAMAGE   = 66240,

    SPELL_PURSUING_SPIKES_TEL=66170,

    SPELL_ROLLING_THROW     = 67730,
    SPELL_ROLLING_THROW_VEH = 67731,
    SPELL_SUBMERGE_BOSS     = 53421,
    SPELL_EMERGE_BOSS       = 53500,

    SPELL_BERSERK           = 26662,

    // npc
    // frost sphere
    NPC_FROST_SPHERE        = 34606,

    SPELL_PERMAFROST        = 66193,

    // burrower
    NPC_BURROWER            = 34607,

    SPELL_NERUBIAN_BURROWER = 66332, // phase 1,2, hc + phase 3
    SPELL_NERUB_BURR_PROC   = 66333,

    SPELL_EXPOSE_WEAKNESS   = 67847,
    SPELL_SPIDER_FRENZY     = 66129,
    SPELL_SUBMERGE_1        = 67322,
    SPELL_SHADOW_STRIKE     = 66134, // hero 10, hero 25
    SPELL_SUMMON_PLAYER     = 21150, // hero 25

    // scarab
    NPC_SCARAB              = 34605,

    SPELL_SUMMON_SCARAB     = 66339, // phase 2
    SPELL_SUMMON_SCARAB_PROC= 66340,

    SPELL_ACID_MANDIBLE     = 65775,
    SPELL_DETERMINATION     = 66092,
    SPELL_SCARAB_ACHI_10    = 68186, // 10 man
    SPELL_SCARAB_ACHI_25    = 68515, // 25 man

    // spike
    NPC_SPIKE               = 34660,
    SPELL_SPIKE_CALL        = 66169,

    SPELL_PURSUED           = 67574, // phase 2 --- casted by "Anubarak 2", handled in burrow
    SPELL_PURSUING_SPIKES_1 = 65920, // phase 2 --- casted by "Anubarak 2", handled in burrow
    SPELL_PURSUING_SPIKES_2 = 65922,
    SPELL_PURSUING_SPIKES_3 = 65923,
    SPELL_PURSUING_SPIKES_D = 65921,

};

struct MANGOS_DLL_DECL boss_anubarak_toc : public ScriptedAI
{
    boss_anubarak_toc(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        isHc = pCreature->GetMap()->IsHeroicRaid();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;
    bool isHC;

    uint8 currentPhase;
    bool m_bIsSubmerged;

    void Reset()
    {
        currentPhase = 0;

    }

    void Aggro(Unit* pWho)
    {
        if (!pWho)
            return;

        currentPhase = 1;
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (currentPhase == 2)
        {
            // am i sure ?
            //uiDamage = 0;
        }
        else if (currentPhase == 1 &&
                 m_creature->GetHealth()-uiDamage < m_creature->GetMaxHealth()*0.3f)
        {
            SwitchPhase(true);
        }
    }

    void MergeSwitch()
    {
        if (m_bIsSubmerged)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE_BOSS);
            m_creature->CastSpell(SPELL_EMERGE_BOSS);
            m_bIsSubmerged = false;
        }
        else
        {
            m_creature->RemoveAurasDueToSpell(SPELL_EMERGE_BOSS);
            m_creature->CastSpell(SPELL_SUBMERGE_BOSS);
            m_bIsSubmerged = true;
        }
    }

    void SwitchPhase(bool three = false)
    {
        if (three)
            currentPhase = 3;

        // this shout switch bethween phases 1 and 2 preety easily
        currentPhase = currentPhase%2+1;

        switch(currentPhase)
        {
            case 1:
            {
                // timers for phase 1
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                MergeSwitch();
                break;
            }
            case 2:
            {
                // timers for phase 2
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                m_TimerMgr->AddSpellToQueue(SPELL_SUMMON_SCARAB, UNIT_SELECT_SELF);

                MergeSwitch();
                break;
            }
            case 3:
            {
                // timers for phase 3
                // add to queue leeching swarm
                m_TimerMgr->AddSpellToQueue(SPELL_LEECHING_SWARM, UNIT_SELECT_SELF);
                break;
            }
            default:
                return;
        }
    }
};

struct MANGOS_DLL_DECL mob_anubarak_spikeAI : public ScriptedAI
{
    mob_anubarak_spikeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;
    uint8 speedLevel;
    uint32 timeOnTarget;

    void Reset()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        speedLevel = 0;
        timeOnTarget = 0;
        m_creature->DeleteThreatList();
        m_creature->GetMotionMaster()->Clear();

        if (SpellTimer* tSpikes = m_TimerMgr[TIMER_SPIKES])
            tSpikes->Reset(TIMER_VALUE_ALL);
        else
            m_TimerMgr->AddTimer(TIMER_SPIKES, pursuingId(), 1000, 3000, UNIT_SELECT_SELF, CAST_TYPE_FORCE);

        if (Creature* anub = GetClosestCreatureWithEntry(m_creature, BOSS_ANUBARAK, DEFAULT_VISIBILITY_INSTANCE))
            anub->CastSpell(m_creature, SPELL_PURSUING_SPIKES_TEL, false);
    }

    uint32 pursuingId()
    {
        switch(speedLevel)
        {
            case 0: return SPELL_PURSUING_SPIKES_D;
            case 1: return SPELL_PURSUING_SPIKES_1;
            case 2: return SPELL_PURSUING_SPIKES_2;
            case 3: return SPELL_PURSUING_SPIKES_3;
            default:return SPELL_PURSUING_SPIKES_3;
        }
    }

    void SetTarget(Unit* pWho)
    {
        m_uiTargetGUID = pWho->GetGUID();
        DoCast(pWho, SPELL_MARK);
        m_creature->SetSpeed(MOVE_RUN, 0.5f);
        AttackStart(pWho);
    }

    void DamageTaken(Unit* /*pWho*/, uint32& uiDamage)
    {
        uiDamage = 0;
    }

    /*void SpellHitTarget(Unit* pWho, const SpellEntry *spellInfo)
    {
        if (spellInfo->Id == SPELL_MARK)
            SetTarget(pWho);
    }*/

    void CastFinished(const SpellEntry *spellInfo)
    {
        if (spellInfo->Id == SPELL_PERMAFROST &&
            timeOnTarget > 4000)
            Reset();
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho, 99999.f);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_TimerMgr(TIMER_SPIKES))
        {
            ++speedLevel;

            SpellTimer* tSpikes = m_TimerMgr[TIMER_SPIKES];
            if (tSpikes)
            {
                if (tSpikes->GetValue(TIMER_VALUE_SPELLID) == SPELL_PURSUING_SPIKES_D)
                {
                    if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                        SetTarget(plr);
                }
                else
                    tSpikes->Cooldown(7000, true);

                tSpikes->SetValue(TIMER_VALUE_SPELLID, pursuingId());
            }
        }

        timeOnTarget += uiDiff;
    }
};

void AddSC_boss_anubarak_trial()
{
    Script* newscript;

}
