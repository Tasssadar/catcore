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
SDName: boss_scourgelord_tyrannus
SD%Complete: 0%
SDComment:
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "pit_of_saron.h"
#include "vehicle.h"

enum
{
    SAY_INTRO1         = -1658302,
    SAY_INTRO2         = -1658304,
    
    ACTION_START_INTRO = 1,
    ACTION_START_FIGHT = 2,
};

struct MANGOS_DLL_DECL boss_tyrannusAI : public ScriptedAI
{
    boss_tyrannusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        if(m_pInstance)
            m_pInstance->SetData64(NPC_TYRANNUS, m_creature->GetGUID());
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint8 introPhase;
    uint32 m_uiIntroTimer;
    bool m_intro;

    Vehicle *pRimefang;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        SetCombatMovement(false);
        m_creature->SetRespawnDelay(86400);
        introPhase = 0;
        m_intro = false;
        pRimefang = NULL;
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if (IsCombatMovement())
                m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void Aggro(Unit* pWho)
    {
    }

    void JustDied(Unit* pKiller)
    {
    }

    void KilledUnit(Unit* pVictim)
    {
    }

    void DoAction(uint32 action)
    {
        switch(action)
        {
            case ACTION_START_INTRO:
                m_intro = true;
                DoScriptText(SAY_INTRO1, m_creature);
                m_uiIntroTimer = 15000;
                pRimefang = m_creature->GetMap()->GetVehicle(m_pInstance->GetData64(NPC_RIMEFANG));
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_intro)
        {
            if(m_uiIntroTimer <= uiDiff)
            {
                switch(introPhase)
                {
                    case 0:
                        DoScriptText(SAY_INTRO2, m_creature);
                        m_uiIntroTimer = 16000;
                        break;
                    case 1:
                        pRimefang->AI()->DoAction(ACTION_START_FIGHT);
                        m_uiIntroTimer = 2000;
                        break;
                    case 2:
                        SetCombatMovement(true);
                        if(m_creature->getVictim())
                            m_creature->AttackStart(m_creature->getVictim());
                        m_intro = false;
                        break;
                }
                ++introPhase;
            }else m_uiIntroTimer -= uiDiff;
            return;
        }
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL boss_rimefang_posAI : public ScriptedAI
{
    boss_rimefang_posAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        if(m_pInstance && m_creature->isVehicle())
            m_pInstance->SetData64(NPC_RIMEFANG, m_creature->GetGUID());
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    Creature *pTyrannus;
    bool m_intro;
    float m_z;
    
    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        SetCombatMovement(false);
        if(!m_creature->isVehicle())
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
            m_creature->ForcedDespawn(1000);
        }
        pTyrannus= NULL;
        m_intro = false;
        m_z = m_creature->GetPositionZ();
    }

    void Aggro(Unit* pWho)
    {
        pTyrannus = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_TYRANNUS));
        if(pTyrannus)
        {
            m_intro = true; 
            pTyrannus->AI()->DoAction(ACTION_START_INTRO);
        }
    }

    void JustDied(Unit* pKiller)
    {
    }

    void DoAction(uint32 action)
    {
        switch(action)
        {
            case ACTION_START_FIGHT:
                m_intro = false;
                ((Vehicle*)m_creature)->RemoveAllPassengers();
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_intro || !m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};

CreatureAI* GetAI_boss_tyrannus(Creature* pCreature)
{
    return new boss_tyrannusAI(pCreature);
}

CreatureAI* GetAI_boss_rimefang_pos(Creature* pCreature)
{
    return new boss_ickAI(pCreature);
}

void AddSC_boss_scourgelord_tyrannus()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_tyrannus";
    newscript->GetAI = &GetAI_boss_tyrannus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_rimefang_pos";
    newscript->GetAI = &GetAI_boss_rimefang_pos;
    newscript->RegisterSelf();
}