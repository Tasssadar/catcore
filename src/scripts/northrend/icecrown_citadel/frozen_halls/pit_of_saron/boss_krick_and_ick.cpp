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

enum
{
    SAY_AGGRO               = -1632000,
    SAY_SOULSTORM           = -1632001,
    SAY_CORRUPT_SOUL        = -1632002,
    SAY_KILL1               = -1632003,
    SAY_KILL2               = -1632004,
    SAY_DEATH               = -1632005,

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

struct MANGOS_DLL_DECL boss_krickAI : public ScriptedAI
{
    boss_krickAI(Creature* pCreature) : ScriptedAI(pCreature)
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
        if(!m_creature->isVehicle())
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
            m_creature->ForcedDespawn(1000);
            if(m_pInstance->GetData(TYPE_ICK_AND_KRICK) == DONE)
                return;
            float x, y, z;
            m_creature->GetPosition(x, y, z);
            Vehicle *pIck = pCreature->SummonVehicle(NPC_ICK, x, y, z, m_creature->GetOrientation(), 522, NULL, 0);
            pIck->SetRespawnDelay(86400);
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        m_pInstance->SetData(TYPE_GARFROST, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0,1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_krick(Creature* pCreature)
{
    return new boss_krickAI(pCreature);
}

void AddSC_boss_ick_and_krick()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_krick";
    newscript->GetAI = &GetAI_boss_krick;
    newscript->RegisterSelf();
}
