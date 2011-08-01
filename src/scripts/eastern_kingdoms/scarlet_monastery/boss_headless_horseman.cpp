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
SDName: boss_headless_horseman
SD%Complete: 0
SDComment: Place Holder
SDCategory: Scarlet Monastery
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_ENTRANCE           = -1189022,
    SAY_REJOINED           = -1189023,
    SAY_BODY_DEFEAT        = -1189024,
    SAY_LOST_HEAD          = -1189025,
    SAY_CONFLAGRATION      = -1189026,
    SAY_SPROUTING_PUMPKINS = -1189027,
    SAY_SLAY               = -1189028,
    SAY_DEATH              = -1189029,

    EMOTE_LAUGH            = -1189030,

    SPELL_SMASH            = 59706,
    SPELL_QUAKE            = 55101,
    SPELL_FLAMESTRIKE      = 72170,

    SAY_PLAYER1            = -1189031,
    SAY_PLAYER2            = -1189032,
    SAY_PLAYER3            = -1189033,
    SAY_PLAYER4            = -1189034,

    NPC_SOLDIER            = 27936,
};

struct MANGOS_DLL_DECL boss_headless_horsemanAI : public ScriptedAI
{
    boss_headless_horsemanAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool m_bIsAir;
    bool under;
    uint32 m_airTimer;
    uint32 m_summonTimer;
    uint32 m_smashTimer;
    uint32 m_quakeTimer;
    uint32 m_flameTimer;

    void Reset()
    {
        m_bIsAir = false;
        under = false;
        m_creature->RemoveSplineFlag(SPLINEFLAG_UNKNOWN7);
        m_airTimer = 27000;
        m_summonTimer = 2000;
        m_smashTimer = 10000;
        m_quakeTimer = 5000;
        m_flameTimer = 5000;
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void AttackStart(Unit *pWho)
    {
        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            if(!m_bIsAir)
                m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void Aggro(Unit* pWho)
    {
        m_creature->SetInCombatWithZone();
        DoScriptText(SAY_ENTRANCE, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_SLAY, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(!under && m_creature->GetHealthPercent() <= 50.0f)
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            under = true;
            m_bIsAir = true;
            m_creature->GetMotionMaster()->Clear(false);
            SetCombatMovement(false);
            float x,y,z;
            m_creature->AddSplineFlag(SPLINEFLAG_UNKNOWN7);
            m_creature->GetRespawnCoord(x,y,z);
            m_creature->GetMotionMaster()->MovePoint(0, x,y,z+4);
            m_creature->RemoveAllAuras();
        }

        if(m_bIsAir)
        {
            if(m_airTimer <= uiDiff)
            {
                m_creature->RemoveSplineFlag(SPLINEFLAG_UNKNOWN7);
                m_creature->GetMotionMaster()->Clear(false);
                SetCombatMovement(true);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());   
                m_bIsAir = false;
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }else m_airTimer -= uiDiff;

            if(m_summonTimer <= uiDiff)
            {
                DoScriptText(SAY_SPROUTING_PUMPKINS, m_creature);
                float x,y,z;
                m_creature->GetRespawnCoord(x,y,z);
                float angle = 0;
                for(uint8 i = 0; i < 5; ++i)
                {
                    float x1, y1;
                    angle = float(urand(0,627))/100.f;
                    x1 = x + (cos(angle) * urand(1,3));
                    y1 = y + (sin(angle) * urand(1,3));
                    Creature *creature = m_creature->SummonCreature(NPC_SOLDIER, x1, y1, z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                    creature->AI()->AttackStart(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0));
                }
                m_summonTimer = 40000;
            }else m_summonTimer -= uiDiff; 

            if(m_flameTimer <= uiDiff)
            {
                DoCast(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0), SPELL_FLAMESTRIKE);
                m_flameTimer = 8000;
            }else m_flameTimer -= uiDiff;
        }
        else
        {
            if(m_smashTimer <= uiDiff)
            {
                DoCast(m_creature, SPELL_SMASH);
                m_smashTimer = 10000;
            }else m_smashTimer -= uiDiff;

            if(m_quakeTimer <= uiDiff)
            {
                DoCast(m_creature, SPELL_QUAKE);
                m_quakeTimer = urand(10000, 17000);
            }else m_quakeTimer -= uiDiff;
        }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_headless_horseman(Creature* pCreature)
{
    return new boss_headless_horsemanAI(pCreature);
}

void AddSC_boss_headless_horseman()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_headless_horseman";
    NewScript->GetAI = GetAI_boss_headless_horseman;
    NewScript->RegisterSelf();
}
