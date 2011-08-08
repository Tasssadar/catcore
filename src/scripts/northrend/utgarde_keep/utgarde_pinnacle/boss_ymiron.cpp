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
SDName: Boss_Ymiron
SD%Complete: 90%
SDComment:
SDAuthot: ScrappyDoo (c) Andeeria
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"

enum Sounds
{
    SAY_AGGRO                   = -1575031,
    SAY_SUMMON_BJORN            = -1575032,
    SAY_SUMMON_HALDOR           = -1575033,
    SAY_SUMMON_RANULF           = -1575034,
    SAY_SUMMON_TORGYN           = -1575035,
    SAY_SLAY_1                  = -1575036,
    SAY_SLAY_2                  = -1575037,
    SAY_SLAY_3                  = -1575038,
    SAY_SLAY_4                  = -1575039,
    SAY_DEATH                   = -1575040
};

enum Spells
{
    SPELL_BANE                                = 48294,
    H_SPELL_BANE                              = 59301,
    SPELL_DARK_SLASH                          = 48292,
    SPELL_FETID_ROT                           = 48291,
    H_SPELL_FETID_ROT                         = 59300,
    SPELL_SCREAMS_OF_THE_DEAD                 = 51750,
    SPELL_SPIRIT_BURST                        = 48529,
    H_SPELL_SPIRIT_BURST                      = 59305,
    SPELL_SPIRIT_STRIKE                       = 48423,
    H_SPELL_SPIRIT_STRIKE                     = 59304,
    SPELL_ANCESTORS_VENGEANCE                 = 16939,

    SPELL_SUMMON_AVENGING_SPIRIT              = 48592,
    SPELL_SUMMON_SPIRIT_FOUNT                 = 48386,

    SPELL_CHANNEL_SPIRIT_TO_YMIRON            = 48316,
    SPELL_CHANNEL_YMIRON_TO_SPIRIT            = 48307,

    SPELL_SPIRIT_FOUNT                        = 48380,
    H_SPELL_SPIRIT_FOUNT                      = 59320
};

enum Creatures
{
    CREATURE_BJORN                          = 27303,
    CREATURE_BJORN_VISUAL                   = 27304,
    CREATURE_HALDOR                         = 27307,
    CREATURE_HALDOR_VISUAL                  = 27310,
    CREATURE_RANULF                         = 27308,
    CREATURE_RANULF_VISUAL                  = 27311,
    CREATURE_TORGYN                         = 27309,
    CREATURE_TORGYN_VISUAL                  = 27312,
    CREATURE_SPIRIT_FOUNT                   = 27339,
    CREATURE_AVENGING_SPIRIT                = 27386
};

float BoatCorrds[4][2] =
{
    {380.31f,-334.94f},
    {380.91f,-314.12f},
    {404.68f,-314.43f},
    {403.02f,-335.46f},
};

/*######
## boss_ymiron
######*/

struct MANGOS_DLL_DECL boss_ymironAI : public ScriptedAI
{
    boss_ymironAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bIsPause;
    bool m_bIsBjorn;
    bool m_bIsHaldor;
    bool m_bIsRanulf;
    bool m_bIsTorgyn;

    uint64 m_uiOrbGUID;
    uint32 m_uiPauseTimer;
    uint32 m_uiBaneTimer;
    uint32 m_uiFetidRotTimer; 
    uint32 m_uiDarkSlashTimer;
    uint32 m_uiAncestorsVengeanceTimer;
    uint32 m_uiOrbTargetChanger;
    uint32 m_uiAbilityBJORNTimer;
    uint32 m_uiAbilityRANULFTimer;
    uint32 m_uiAbilityTORGYNTimer;
    uint32 m_uiAbilityHALDORTimer;
    uint8  m_uiCurentBoat;
    uint8  m_uiHealthAmountModifier;

    void Reset()
    {
        m_bIsBjorn                   = false;
        m_bIsHaldor                  = false;
        m_bIsRanulf                  = false;
        m_bIsTorgyn                  = false;
        m_bIsPause                   = false;             
        m_uiFetidRotTimer            = urand(8000,13000);
        m_uiBaneTimer                = urand(18000,23000);
        m_uiDarkSlashTimer           = urand(28000,33000);
        m_uiAncestorsVengeanceTimer  = 50000;
        m_uiOrbTargetChanger         = 5000;
        m_uiOrbGUID                  = 0;
        m_uiCurentBoat               = 0;
        m_uiHealthAmountModifier     = 1;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 3))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
            case 3: DoScriptText(SAY_SLAY_4, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bIsPause)
        {
            m_creature->StopMoving();
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();

            if (m_uiPauseTimer < uiDiff)
            {
                if (m_creature->getVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                m_bIsPause = false;
            }else m_uiPauseTimer -= uiDiff;
            return;
        }

        if (m_uiOrbTargetChanger < uiDiff)
        {
            Creature* pSpirit = (Creature*)Unit::GetUnit(*m_creature, m_uiOrbGUID);
            if (pSpirit && pSpirit->isAlive())
                if (Unit* pPlayer = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSpirit->GetMotionMaster()->MoveChase(pPlayer);
            m_uiOrbTargetChanger = 15000;
        }else m_uiOrbTargetChanger -= uiDiff;

        if (m_uiBaneTimer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_BANE : H_SPELL_BANE, true);
            m_uiBaneTimer = 20000;
        }else m_uiBaneTimer -= uiDiff;

        if (m_uiFetidRotTimer < uiDiff)
        {
            if (Unit* pPlayer = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                m_creature->CastSpell(pPlayer, m_bIsRegularMode ? SPELL_FETID_ROT : H_SPELL_FETID_ROT, true);
            m_uiFetidRotTimer = urand(10000,15000);
        }else m_uiFetidRotTimer -= uiDiff;

        //need to hack this
        if (m_uiDarkSlashTimer < uiDiff)
        {
            if (Unit* pPlayer = m_creature->getVictim())
            {
                //m_creature->DealDamage(target, target->GetMaxHealth()*((rand() % 15+70)*0.01), NULL, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NATURE, NULL, false);
                m_creature->DealDamage(m_creature->getVictim(),pPlayer->GetMaxHealth()*0.5, NULL, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                m_creature->CastSpell(m_creature->getVictim(), SPELL_DARK_SLASH, true); //not working
            }
            m_uiDarkSlashTimer = urand(30000,35000);
        }else m_uiDarkSlashTimer -= uiDiff;

        if (m_uiAncestorsVengeanceTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_ANCESTORS_VENGEANCE);
            m_uiAncestorsVengeanceTimer =  (m_bIsRegularMode ? urand(60000,65000) : urand(45000,50000));
        }else m_uiAncestorsVengeanceTimer -= uiDiff;

        if (m_bIsHaldor && m_uiAbilityHALDORTimer < uiDiff)
        {
            if (m_creature->getVictim())
                m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SPIRIT_STRIKE : H_SPELL_SPIRIT_STRIKE, true);
            m_uiAbilityHALDORTimer = 5000; // overtime
        } else m_uiAbilityHALDORTimer -= uiDiff;

        if (m_bIsTorgyn && m_uiAbilityTORGYNTimer < uiDiff)
        {
            for (uint8 i = 0; i < 4; ++i)
                if (Creature* pSpirit = m_creature->SummonCreature(CREATURE_AVENGING_SPIRIT, m_creature->GetPositionX()+urand(1,10), m_creature->GetPositionY()+urand(1,10), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
                    if (Unit* pPlayer = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        pSpirit->AI()->AttackStart(pPlayer);
            m_uiAbilityTORGYNTimer = 10000;
        }else m_uiAbilityTORGYNTimer -= uiDiff;

        if (m_bIsRanulf && m_uiAbilityRANULFTimer < uiDiff)
        {
            m_creature->CastSpell(m_creature, m_bIsRegularMode ? SPELL_SPIRIT_BURST : H_SPELL_SPIRIT_BURST, true);
            m_uiAbilityRANULFTimer = 10000;
        } else m_uiAbilityRANULFTimer -= uiDiff;

        if (m_bIsBjorn && m_uiAbilityBJORNTimer < uiDiff)
        {
            if (Creature* pSpirit = m_creature->SummonCreature(CREATURE_SPIRIT_FOUNT, m_creature->GetPositionX()+urand(1,10), m_creature->GetPositionY()+urand(1,10), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
            {
                pSpirit->CastSpell(pSpirit, m_bIsRegularMode ? SPELL_SPIRIT_FOUNT : H_SPELL_SPIRIT_FOUNT, true);
                m_uiOrbGUID = pSpirit->GetGUID();
                m_uiOrbTargetChanger = 1000;
            }
            m_bIsBjorn = false;
        } else m_uiAbilityBJORNTimer -= uiDiff;

        if ((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < (100-((m_bIsRegularMode ? 33 : 20) * m_uiHealthAmountModifier)))
        {
            if (Creature* pSpirit = (Creature*)Unit::GetUnit(*m_creature, m_uiOrbGUID))
            {
                pSpirit->setFaction(35);
                pSpirit->ForcedDespawn();
            }

            if (m_creature->getVictim())
                m_creature->CastSpell(m_creature->getVictim(), SPELL_SCREAMS_OF_THE_DEAD, true);

            m_creature->CastSpell(m_creature, SPELL_CHANNEL_YMIRON_TO_SPIRIT, true);

            if (m_uiCurentBoat == 0)
                m_uiCurentBoat = urand(1,4);
            else 
            {
                if (m_uiCurentBoat == 4)
                    m_uiCurentBoat = 1;
                else
                    ++m_uiCurentBoat;
            }

            m_bIsBjorn  = false;
            m_bIsHaldor = false;
            m_bIsRanulf = false;
            m_bIsTorgyn = false;

            uint32 ID = 0;
            switch(m_uiCurentBoat)
            {
                case 1: DoScriptText(SAY_SUMMON_BJORN, m_creature);  ID = CREATURE_BJORN_VISUAL;  m_uiAbilityBJORNTimer  = 8000; m_bIsBjorn  = true; break;
                case 2: DoScriptText(SAY_SUMMON_HALDOR, m_creature); ID = CREATURE_HALDOR_VISUAL; m_uiAbilityHALDORTimer = 8000; m_bIsHaldor = true; break;
                case 3: DoScriptText(SAY_SUMMON_RANULF, m_creature); ID = CREATURE_RANULF_VISUAL; m_uiAbilityRANULFTimer = 8000; m_bIsRanulf = true; break;
                case 4: DoScriptText(SAY_SUMMON_TORGYN, m_creature); ID = CREATURE_TORGYN_VISUAL; m_uiAbilityTORGYNTimer = 8000; m_bIsTorgyn = true; break;
                default: break;
            }

            if (Creature* pBoat = m_creature->SummonCreature(ID, BoatCorrds[m_uiCurentBoat-1][0], BoatCorrds[m_uiCurentBoat-1][1], m_creature->GetPositionZ()+2, 0, TEMPSUMMON_TIMED_DESPAWN, 30000))
            {
                pBoat->CastSpell(m_creature, SPELL_CHANNEL_SPIRIT_TO_YMIRON, true);
                pBoat->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pBoat->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            m_bIsPause = true;
            m_uiPauseTimer = 6000;
            ++m_uiHealthAmountModifier;
            //Reset bane timer
            m_uiBaneTimer = 20000;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ymiron(Creature* pCreature)
{
    return new boss_ymironAI(pCreature);
}

void AddSC_boss_ymiron()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ymiron";
    newscript->GetAI = &GetAI_boss_ymiron;
    newscript->RegisterSelf();
}
