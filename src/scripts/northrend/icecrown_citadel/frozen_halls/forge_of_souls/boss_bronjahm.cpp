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
SDName: boss_bronjahm
SD%Complete: 0%
SDComment:
SDCategory: The Forge of Souls
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO               = -1632000,
    SAY_SOULSTORM           = -1632001,
    SAY_CORRUPT_SOUL        = -1632002,
    SAY_KILL1               = -1632003,
    SAY_KILL2               = -1632004,
    SAY_DEATH               = -1632005,

    // Boss Spells
    SPELL_MAGICS_BANE       = 68793,
    SPELL_MAGICS_BANE_H     = 69050,

    SPELL_SHADOW_BOLT       = 70043,
    SPELL_CORRUPT_SOUL      = 68839,
    SPELL_CONSUME_SOUL      = 68858,
    SPELL_CONSUME_SOUL_H    = 69047,

    SPELL_TELEPORT          = 68988,

    SPELL_SOULSTORM         = 68872,
    SPELL_SOULSTORM_CAST    = 69008,
    SPELL_SOULSTORM_VISUAL1 = 68906,
    SPELL_SOULSTORM_VISUAL2 = 68907,

    SPELL_FEAR              = 68950,
};

/*######
## boss_bronjahm
######*/

struct MANGOS_DLL_DECL boss_bronjahmAI : public ScriptedAI
{
    boss_bronjahmAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiMagicsBaneTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiCorruptSoulTimer;
    uint32 m_uiSoulstormTimer;
    uint32 m_uiFearTimer;

    bool phase2;

    std::set<Creature*> m_souls;

    void Reset()
    {
        m_uiMagicsBaneTimer = 8000;
        m_uiShadowBoltTimer = 10000;
        m_uiCorruptSoulTimer = 15000;
        m_uiSoulstormTimer = 2000;
        m_uiFearTimer = 4000;
        DespawnSouls();
        m_souls.clear();
        phase2 = false;
        SetCombatMovement(true);
    }

    void DespawnSouls()
    {
        if(!m_souls.empty())
        {
            std::set<Creature*> tmp = m_souls;
            for(std::set<Creature*>::iterator itr = tmp.begin(); itr != tmp.end(); ++itr)
                (*itr)->ForcedDespawn();
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0,1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        m_souls.insert(pSummoned);
        pSummoned->GetMotionMaster()->MoveChase(m_creature, 0.0f);
        pSummoned->CastSpell(pSummoned, 55845, true);
    }

    void SummonedCreatureDespawn(Creature* pDespawned)
    {
        m_souls.erase(pDespawned);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(phase2)
        {
            if(m_uiSoulstormTimer <= uiDiff)
            {
                DoCast(m_creature, SPELL_SOULSTORM_VISUAL1, true);
                DoCast(m_creature, SPELL_SOULSTORM_VISUAL2, true);
                DoCast(m_creature, SPELL_SOULSTORM);
                DoScriptText(SAY_SOULSTORM, m_creature);
                m_uiSoulstormTimer = 600000;
            }else m_uiSoulstormTimer -= uiDiff;

            if(m_uiFearTimer <= uiDiff)
            {
                Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);
                if(plr)
                    DoCast(plr, SPELL_FEAR);
                m_uiFearTimer = 6000;
            }else m_uiFearTimer -= uiDiff;
            
            return;
        }

        if(m_creature->GetHealthPercent() <= 35.0f)
        {
            phase2 = true;
            SetCombatMovement(false);
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();
            DoCast(m_creature, SPELL_TELEPORT);
            DespawnSouls();
        }
        
        if(!m_creature->IsWithinDistInMap(m_creature->getVictim(), 6.66f))
        {
            if(m_uiShadowBoltTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_BOLT);
                m_uiShadowBoltTimer = 1000;
            }else m_uiShadowBoltTimer -= uiDiff;
        }
        
        if(m_uiMagicsBaneTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_MAGICS_BANE : SPELL_MAGICS_BANE_H);
            m_uiMagicsBaneTimer = 8000;
        }else m_uiMagicsBaneTimer -= uiDiff;

        if(m_uiCorruptSoulTimer <= uiDiff)
        {
            Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1);
            if(plr)
                DoCast(plr, SPELL_CORRUPT_SOUL);
            DoScriptText(SAY_CORRUPT_SOUL, m_creature);
            m_uiCorruptSoulTimer = 15000;
        }else m_uiCorruptSoulTimer -= uiDiff;

        if(!m_souls.empty())
        {
            std::set<Creature*> tmp = m_souls;
            for(std::set<Creature*>::iterator itr = tmp.begin(); itr != tmp.end(); ++itr)
            {
                if(!(*itr) || !(*itr)->IsInWorld())
                    continue;
                
                if((*itr)->IsWithinDistInMap(m_creature, 1.0f))
                {
                    (*itr)->ForcedDespawn();
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_CONSUME_SOUL : SPELL_CONSUME_SOUL_H, true);
                }
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_bronjahm(Creature* pCreature)
{
    return new boss_bronjahmAI(pCreature);
}

void AddSC_boss_bronjahm()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_bronjahm";
    newscript->GetAI = &GetAI_boss_bronjahm;
    newscript->RegisterSelf();
}

