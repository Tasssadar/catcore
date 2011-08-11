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
SDName: boss_krick_and_ick
SD%Complete: 0%
SDComment:
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "pit_of_saron.h"
#include "Vehicle.h"

enum
{
    SAY_AGGRO               = -1632000,
    SAY_SOULSTORM           = -1632001,
    SAY_CORRUPT_SOUL        = -1632002,
    SAY_KILL1               = -1632003,
    SAY_KILL2               = -1632004,
    SAY_DEATH               = -1632005,

    // Boss Spells
    SPELL_PUSTULANT_FLESH   = 69581,
    SPELL_PUSTULANT_FLESH_H = 70273,
    SPELL_TOXIC_WASTE       = 69024,
    SPELL_POISON_NOVA       = 68989,
    SPELL_POISON_NOVA_H     = 70434,
    SPELL_PURSUIT           = 68987,
    SPELL_EXPLOSIVE_BARRAGE_CHANNEL = 69012,
    SPELL_ORB_SUMMON        = 69015,

    SPELL_ORB_EXPLODE       = 69019,
    SPELL_ORB_EXPLODE_H     = 70433,
    SPELL_EXPLODING_ORB_VISUAL = 69017,

    ACTION_ICK_DEAD         = 1,
};

struct MANGOS_DLL_DECL boss_krickAI : public ScriptedAI
{
    boss_krickAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        if(m_pInstance)
            m_pInstance->SetData64(NPC_KRICK, m_creature->GetGUID());
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool ickDead;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        ickDead = false;
    }

    void AttackStart(Unit* pWho)
    {
        if(ickDead)
            return;
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

    void DoAction(uint32 action)
    {
        switch(action)
        {
            case ACTION_ICK_DEAD:
                m_creature->InterruptNonMeleeSpells(false);
                ickDead = true;
                break;
        }
    }
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};

struct MANGOS_DLL_DECL boss_ickAI : public ScriptedAI
{
    boss_ickAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiToxicWasteTimer;
    uint32 m_uiPestulantFleshTimer;
    uint32 m_uiPoisionNovaTimer;
    uint32 m_uiPursuitTimer;
    uint8 m_uiPursuitPhase;
    uint32 m_uiOrbsTimer;
    uint8 m_uiOrbsCount;
    bool m_uiOrbs;

    Creature *pKrick;
    Unit *pPursuitTarget;
    void Reset()
    {
        m_uiPursuitPhase = 0;
        m_uiOrbsCount = 0;
        m_uiOrbs = false;
        SetCombatMovement(true);
        if(!m_creature->isVehicle())
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
            m_creature->ForcedDespawn(1000);
            if(m_pInstance->GetData(TYPE_ICK_AND_KRICK) == DONE)
                return;
            float x, y, z;
            m_creature->GetPosition(x, y, z);
            Vehicle *pIck = m_creature->SummonVehicle(NPC_ICK, x, y, z, m_creature->GetOrientation(), 522, NULL, 0);
            pIck->SetRespawnDelay(86400);
        }
        pKrick = NULL;
        pPursuitTarget = NULL;
        ResetTimers();
    }

    void ResetTimers()
    {
        m_uiToxicWasteTimer = 10000;
        m_uiPestulantFleshTimer = 8000;
        m_uiPoisionNovaTimer = 16000;
        m_uiPursuitTimer = 10000;
        m_uiOrbsTimer = 30000;
    }

    void Aggro(Unit* pWho)
    {
        pKrick = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_KRICK));
        if(!pKrick)
            EnterEvadeMode();
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        m_pInstance->SetData(TYPE_ICK_AND_KRICK, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0,1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void SummonOrbs()
    {
        PlrList pList = GetAttackingPlayers();
        for (PlrList::iterator itr = pList.begin(); itr != pList.end(); ++itr)
        {
            if(!(*itr) || !(*itr)->IsInWorld() || !(*itr)->isAlive())
                continue;
            pKrick->CastSpell(*itr, SPELL_ORB_SUMMON, true);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiOrbsTimer <= uiDiff)
        {
            if(!m_uiOrbs)
            {
                m_uiPursuitTimer = 30000;
                pKrick->CastSpell(pKrick, SPELL_EXPLOSIVE_BARRAGE_CHANNEL, false);
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveIdle();
                m_uiOrbsTimer = 2000;
                m_uiOrbsCount = 0;
                m_uiOrbs = true;
            }
            else if(m_uiOrbs && m_uiOrbsCount <= 8)
            {
                SummonOrbs();
                m_uiOrbsTimer = 2000;
                ++m_uiOrbsCount;
            }
            else
            {
                SetCombatMovement(true);
                m_creature->GetMotionMaster()->Clear(false, true);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                m_uiOrbsTimer = 30000;
                m_uiPursuitTimer = urand(13000, 16000);
                m_uiOrbs = false;
                ResetTimers();
            }
            
        }else m_uiOrbsTimer -= uiDiff;

        if(m_uiOrbs)
            return;
        
        if(m_uiToxicWasteTimer <= uiDiff)
        {
            Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);
            if(plr)
                DoCastSpellIfCan(plr, SPELL_TOXIC_WASTE);
            m_uiToxicWasteTimer = 10000;
        }else m_uiToxicWasteTimer -= uiDiff;

        if(m_uiPestulantFleshTimer <= uiDiff)
        {
            Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);
            if(plr)
                DoCastSpellIfCan(plr, m_bIsRegularMode ? SPELL_PUSTULANT_FLESH : SPELL_PUSTULANT_FLESH_H);
            m_uiPestulantFleshTimer = urand(8000, 12000);
        }else m_uiPestulantFleshTimer -= uiDiff;

        if(m_uiPoisionNovaTimer <= uiDiff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            if(m_uiPursuitTimer < 5000)
                m_uiPursuitTimer = 5000;
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_POISON_NOVA : SPELL_POISON_NOVA_H);
            m_uiPoisionNovaTimer = urand(14000, 20000);
        }else m_uiPoisionNovaTimer -= uiDiff;

        if(m_uiPursuitTimer <= uiDiff)
        {
            switch(m_uiPursuitPhase)
            {
                case 0:
                    pPursuitTarget = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1);
                    if(!pPursuitTarget)
                    {
                        m_uiPursuitTimer = 10000;
                        break;
                    }
                    m_creature->InterruptNonMeleeSpells(false);
                    if(m_uiPoisionNovaTimer < 20000)
                        m_uiPoisionNovaTimer = 20000;
                    DoCast(pPursuitTarget, SPELL_PURSUIT);
                    m_creature->AddThreat(pPursuitTarget, 10000000.0f);
                    ++m_uiPursuitPhase;
                    m_uiPursuitTimer = 17000;
                    m_uiOrbsTimer = 27000;
                    break;
                case 1:
                    if(pPursuitTarget && pPursuitTarget->IsInWorld() && pPursuitTarget->isAlive())
                        m_creature->AddThreat(pPursuitTarget, -10000000.0f);
                    m_uiPursuitTimer = urand(13000, 16000);
                    m_uiPursuitPhase = 0;
                    break;
            }
        }else m_uiPursuitTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_exploding_orbAI : public ScriptedAI
{
    mob_exploding_orbAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiExplodeTimer;

    void Reset()
    {
        m_creature->ForcedDespawn(4000);
        m_uiExplodeTimer = 3000;
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        DoCast(m_creature, SPELL_EXPLODING_ORB_VISUAL, true);
    }

    void AttackStart(Unit* pWho)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiExplodeTimer <= uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_ORB_EXPLODE : SPELL_ORB_EXPLODE_H);
            m_uiExplodeTimer = 50000;
        }else m_uiExplodeTimer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_krick(Creature* pCreature)
{
    return new boss_krickAI(pCreature);
}

CreatureAI* GetAI_boss_ick(Creature* pCreature)
{
    return new boss_ickAI(pCreature);
}

CreatureAI* GetAI_mob_exploding_orb(Creature* pCreature)
{
    return new mob_exploding_orbAI(pCreature);
}

void AddSC_boss_ick_and_krick()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_krick";
    newscript->GetAI = &GetAI_boss_krick;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_ick";
    newscript->GetAI = &GetAI_boss_ick;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_exploding_orb";
    newscript->GetAI = &GetAI_mob_exploding_orb;
    newscript->RegisterSelf();
}
