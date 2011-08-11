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
#include "Vehicle.h"

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
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        if(m_creature->getVictim())
                            AttackStart(m_creature->getVictim());
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

const float dismountPos[3] = { 1028.41f, 167.93, 628.2};
const float fightPos[][3] =
{
    { 966.32,  178.91, 670.93 },
    { 1076.13, 138.98, 670.93 },
    { 1035.96, 208.28, 670.93 },
    { 988.35,  113.54, 670.93 },
    { 1012.25, 160.87, 670.93 },
};

#define FIGHT_POS 4

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

    uint32 flyTimer;
    
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
        flyTimer = urand(10000, 20000);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (m_creature->CanInitiateAttack() && pWho->isTargetableForAttack() &&
            m_creature->IsHostileTo(pWho) && pWho->isInAccessablePlaceFor(m_creature))
        {
            if (m_creature->IsWithinDistInMap(pWho, 40.0f) && m_creature->IsWithinLOSInMap(pWho))
            {
                if (!m_creature->getVictim())
                {
                    pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                    AttackStart(pWho);
                }
                else if (m_creature->GetMap()->IsDungeon())
                {
                    pWho->SetInCombatWith(m_creature);
                    m_creature->AddThreat(pWho);
                }
            }
        }
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

    void DoRandomFlight()
    {
        uint8 tmp = urand(0, FIGHT_POS);
        PointPath path;
        path.resize(2);
        path.set(0, PathNode(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()));
        path.set(1, PathNode(fightPos[tmp][0], fightPos[tmp][1], fightPos[tmp][2]));
        uint32 time = m_creature->GetDistance(path[1].x, path[1].y, path[1].z)/(10.0f*0.001f);
        m_creature->GetMotionMaster()->MoveClear(false, true);
        m_creature->GetMotionMaster()->MoveCharge(path, time, 1, 1);
        m_creature->SendMonsterMove(path[1].x, path[1].y, path[1].z, SPLINETYPE_NORMAL , SPLINEFLAG_FLYING, time);
    }

    void DoAction(uint32 action)
    {
        switch(action)
        {
            case ACTION_START_FIGHT:
                m_intro = false;
                ((Vehicle*)m_creature)->RemoveAllPassengers();
                pTyrannus->SendMonsterMove(dismountPos[0], dismountPos[1], dismountPos[2], SPLINETYPE_NORMAL, SPLINEFLAG_FORWARD, 0);
                pTyrannus->GetMap()->CreatureRelocation(pTyrannus, dismountPos[0], dismountPos[1], dismountPos[2], 0);
                DoRandomFlight();
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_intro || !m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(flyTimer <= uiDiff)
        {
            DoRandomFlight();
            flyTimer = urand(10000, 20000);
        }else flyTimer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_tyrannus(Creature* pCreature)
{
    return new boss_tyrannusAI(pCreature);
}

CreatureAI* GetAI_boss_rimefang_pos(Creature* pCreature)
{
    return new boss_rimefang_posAI(pCreature);
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