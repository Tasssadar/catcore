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
SDName: boss_devourer_of_souls
SD%Complete: 0%
SDComment:
SDCategory: The Forge of Souls
EndScriptData */

#include "precompiled.h"

enum
{
    SAY_AGGRO               = -1574000,
    SAY_FROSTTOMB           = -1574001,
    SAY_SKELETONS           = -1574002,
    SAY_KILL                = -1574003,
    SAY_DEATH               = -1574004,

    SPELL_WELL_OF_SOULS     = 68854,
    SPELL_PHANTOM_BLAST     = 68982,
    SPELL_PHANTOM_BLAST_H   = 70322,
    SPELL_UNLEASHED_SOULS   = 68939,

    SPELL_MIRRORED_SOULS    = 69051,

    NPC_WELL_OF_SOULS       = 36536,
};

/*######
## boss_bronjahm
######*/

struct MANGOS_DLL_DECL boss_devourer_of_soulsAI : public ScriptedAI
{
    boss_devourer_of_soulsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiPhantomBlastTimer;
    uint32 m_uiMirroredSoulsTimer;
    uint32 m_uiUnleashedSoulsTimer;
    uint32 m_uiWellOfSoulsTimer;

    Player *mirrorPlr;
    uint8 mirrorStage;

    void Reset()
    {
        m_uiPhantomBlastTimer = 3000;
        m_uiMirroredSoulsTimer = 10000;
        m_uiUnleashedSoulsTimer = 15000;
        m_uiWellOfSoulsTimer = 6000;
        mirrorPlr = NULL;
        mirrorStage = 0;
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
        DoScriptText(SAY_KILL, m_creature);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if(mirrorStage != 2 || !mirrorPlr || !mirrorPlr->IsInWorld() || !mirrorPlr->isAlive())
            return;
        uint32 dmg = m_bIsRegularMode ? uiDamage*0.3f : uiDamage*0.45f;
        pDoneBy->DealDamage(mirrorPlr, dmg, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->setFaction(m_creature->getFaction());
        pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pSummoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pSummoned->ForcedDespawn(10000);

        Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);
        if(plr)
        {
            pSummoned->AI()->AttackStart(plr);
            pSummoned->AddThreat(plr, 1000.0f);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiPhantomBlastTimer <= uiDiff)
        {
            Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);
            if(plr)
                DoCastSpellIfCan(plr, m_bIsRegularMode ? SPELL_PHANTOM_BLAST : SPELL_PHANTOM_BLAST_H);
            m_uiPhantomBlastTimer = 3000;
        }else m_uiPhantomBlastTimer -= uiDiff;

        if(m_uiMirroredSoulsTimer <= uiDiff)
        {
            switch(mirrorStage)
            {
                case 0:
                {
                    Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);
                    if(plr)
                    {
                        DoCast(plr, SPELL_MIRRORED_SOULS);
                        mirrorPlr = plr;
                    }
                    m_uiMirroredSoulsTimer = 2000;
                    ++mirrorStage;
                    break;
                }
                case 1:
                {
                    ++mirrorStage;
                    m_uiMirroredSoulsTimer = 8000;
                    break;
                }
                case 2:
                {
                    mirrorStage = 0;
                    m_uiMirroredSoulsTimer = 10000;
                }
            }
        }else m_uiMirroredSoulsTimer-= uiDiff;

        if(m_uiUnleashedSoulsTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_UNLEASHED_SOULS);
            m_uiUnleashedSoulsTimer = 17000;
        }else m_uiUnleashedSoulsTimer -= uiDiff;

        if(m_uiWellOfSoulsTimer <= uiDiff)
        {
            Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);
            if(plr)
            {
                float x, y, z;
                plr->GetPosition(x, y, z);

                Creature *well = m_creature->SummonCreature(NPC_WELL_OF_SOULS, x, y, z, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 15000);
                if(well)
                    well->CastSpell(m_creature, SPELL_WELL_OF_SOULS, true);
                
                plr->GetNearPoint(m_creature, x, y, z, m_creature->GetObjectBoundingRadius(), 1.0f, M_PI_F);
                m_creature->NearTeleportTo(x, y, z, 0);
            }
            m_uiWellOfSoulsTimer = 6000;
        }else m_uiWellOfSoulsTimer -= uiDiff;
        

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_devourer_of_souls(Creature* pCreature)
{
    return new boss_devourer_of_soulsAI(pCreature);
}

void AddSC_boss_devourer_of_souls()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_devourer_of_souls";
    newscript->GetAI = &GetAI_boss_devourer_of_souls;
    newscript->RegisterSelf();
}

