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
SDName: boss_forgemaster_gafrost
SD%Complete: 0%
SDComment:
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "pit_of_saron.h"

enum
{
    SAY_AGGRO               = -1658001,
    SAY_KILL1               = -1658004,
    SAY_KILL2               = -1658005,
    SAY_DEATH               = -1658006,
    SAY_FORGE1              = -1658002,
    SAY_FORGE2              = -1658003,

    EMOTE_BOULDER           = -1658500,

    // Boss Spells
    SPELL_PERMAFROST        = 70326,
    SPELL_THROW_SARONITE    = 68788,
    SPELL_FORGE_FROSTBORN_MACE   = 68785,
    SPELL_FORGE_FROSTBORN_MACE_H = 70335,
    SPELL_DEEP_FREEZE       = 70381,
    SPELL_DEEP_FREEZE_H     = 72930,
    SPELL_THUNDERING_STOMP  = 68771,
    SPELL_CHILLING_WAVE     = 68778,
    SPELL_CHILLING_WAVE_H   = 70333,
};

/*######
## boss_bronjahm
######*/

const float forgePos[2][3] =
{
    {645.06f, -207.62, 528.931f},
    {722.83f, -233.21, 527.107f},
};

struct MANGOS_DLL_DECL boss_forgemaster_gafrostAI : public ScriptedAI
{
    boss_forgemaster_gafrostAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_forgePhase;
    uint32 m_forgeTimer;
    
    uint32 m_uiChillingWaveTimer;
    uint32 m_uiDeepFreezeTimer;
    uint32 m_uiThunderingStompTimer;
    uint32 m_uiThrowSaronite;
    uint32 m_uiFearTimer;

    void Reset()
    {
        m_uiChillingWaveTimer = 10000;
        m_uiDeepFreezeTimer = 6000;
        m_uiThunderingStompTimer = 14000;
        m_uiThrowSaronite = 8000;
        m_forgePhase = 0;
        SetCombatMovement(true);
    }

    void Aggro(Unit*)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        DoCast(m_creature, SPELL_PERMAFROST);
    }

    void JustDied(Unit*)
    {
        DoScriptText(SAY_DEATH, m_creature);
        m_pInstance->SetData(TYPE_GARFROST, DONE);
    }

    void KilledUnit(Unit*)
    {
        DoScriptText(urand(0,1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    Unit *getRangedTarget()
    {
        Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
        std::vector<Unit*> targets;
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            if (Player* pPlayer = itr->getSource())
            {
                if(m_creature->GetDistance(pPlayer) >= 10.0f)
                    targets.push_back(pPlayer);
            }
        }
        if(!targets.empty())
            return targets[time(0)%targets.size()];
        else
            return m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_forgePhase%3 != 0)
        {
            if(m_forgeTimer <= uiDiff)
            {
                switch(m_forgePhase%3)
                {
                    case 1:
                        DoCast(m_creature, m_bIsRegularMode ? SPELL_FORGE_FROSTBORN_MACE : SPELL_FORGE_FROSTBORN_MACE_H);
                        m_forgeTimer = 4000;
                        break;
                    case 2:
                        SetCombatMovement(true);
                        m_creature->GetMotionMaster()->Clear(false, true);
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                        m_creature->SetUInt64Value(UNIT_FIELD_TARGET, m_creature->getVictim()->GetGUID());
                        break;
                }
                ++m_forgePhase;
            }else m_forgeTimer -= uiDiff;
            return;
        }

        if((m_forgePhase == 3  && m_creature->GetHealthPercent() <= 33.0f) ||
           (m_forgePhase == 0 && m_creature->GetHealthPercent() <= 66.0f))
        {
            DoScriptText(SAY_FORGE1 - m_forgePhase/3, m_creature);
            DoCast(m_creature, SPELL_THUNDERING_STOMP);
            m_creature->SetUInt64Value(UNIT_FIELD_TARGET, 0);
            ++m_forgePhase;
            SetCombatMovement(false);
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->TrajMonsterMove(forgePos[m_forgePhase/3][0], forgePos[m_forgePhase/3][1], forgePos[m_forgePhase/3][2], false, 60, 1000);
            m_forgeTimer = 1200;
            return;
        }

        if(m_uiChillingWaveTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_CHILLING_WAVE : SPELL_CHILLING_WAVE_H);
            m_uiChillingWaveTimer = urand(10000, 17000);
        }else m_uiChillingWaveTimer -= uiDiff;

        if(m_uiDeepFreezeTimer <= uiDiff)
        {
            Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);
            if(plr)
                DoCastSpellIfCan(plr, m_bIsRegularMode ? SPELL_DEEP_FREEZE : SPELL_DEEP_FREEZE_H);
            m_uiDeepFreezeTimer = urand(15000, 25000);
        }else m_uiDeepFreezeTimer -= uiDiff;

        if(m_uiThrowSaronite <= uiDiff)
        {
            Unit* plr = getRangedTarget();

            if(plr)
            {
                DoScriptText(EMOTE_BOULDER, m_creature, plr);
                DoCast(plr, SPELL_THROW_SARONITE);
            }
            m_uiThrowSaronite = urand(7000, 10000);
        }else m_uiThrowSaronite -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_forgemaster_gafrost(Creature* pCreature)
{
    return new boss_forgemaster_gafrostAI(pCreature);
}

void AddSC_boss_forgemaster_gafrost()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_forgemaster_gafrost";
    newscript->GetAI = &GetAI_boss_forgemaster_gafrost;
    newscript->RegisterSelf();
}


