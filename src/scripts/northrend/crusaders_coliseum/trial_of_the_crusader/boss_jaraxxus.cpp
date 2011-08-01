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
SDName: trial_of_the_crusader
SD%Complete: 80%
SDComment: by /dev/rsa
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

// boss spells
enum Jaraxxus
{
    // Fel Fireball
    SPELL_FEL_FIREBALL_0        = 66532,
    SPELL_FEL_FIREBALL_1        = 66963,
    SPELL_FEL_FIREBALL_2        = 66964,
    SPELL_FEL_FIREBALL_3        = 66965,

    // Fel Lightning
    SPELL_FEL_LIGHTNING_0       = 66528,
    SPELL_FEL_LIGHTNING_1       = 67029,
    SPELL_FEL_LIGHTNING_2       = 67030,
    SPELL_FEL_LIGHTNING_3       = 67031,

    // Incinerate Flesh
    SPELL_INCINERATE_FLESH_0    = 66237,
    SPELL_INCINERATE_FLESH_1    = 67049,
    SPELL_INCINERATE_FLESH_2    = 67050,
    SPELL_INCINERATE_FLESH_3    = 67051,

    // Legion Flame
    SPELL_LEGION_FLAME_0        = 66197,
    SPELL_LEGION_FLAME_1        = 68123,
    SPELL_LEGION_FLAME_2        = 68124,
    SPELL_LEGION_FLAME_3        = 68125,

    // Infernal Eruption
    SPELL_INFERNAL_ERUPTION_0   = 66258,
    SPELL_INFERNAL_ERUPTION_1   = 67901,
    SPELL_INFERNAL_ERUPTION_2   = 67902,
    SPELL_INFERNAL_ERUPTION_3   = 67903,

    // Nether Portal
    SPELL_NETHER_PORTAL_0       = 66269,
    SPELL_NETHER_PORTAL_1       = 67898,
    SPELL_NETHER_PORTAL_2       = 67899,
    SPELL_NETHER_PORTAL_3       = 67900,

    // Nether Power
    SPELL_NETHER_POWER          = 67009,

    // Fel Lightning - gnom instakill
    SPELL_FEL_LIGHTNING_IK      = 67888,

};

const uint32 m_idFelFireball[4] = {SPELL_FEL_FIREBALL_0, SPELL_FEL_FIREBALL_1, SPELL_FEL_FIREBALL_2, SPELL_FEL_FIREBALL_3};
const uint32 m_idFelLightning[4] = {SPELL_FEL_LIGHTNING_0, SPELL_FEL_LIGHTNING_1, SPELL_FEL_LIGHTNING_2, SPELL_FEL_LIGHTNING_3};
const uint32 m_idIncinerateFlesh[4] = {SPELL_INCINERATE_FLESH_0, SPELL_INCINERATE_FLESH_1, SPELL_INCINERATE_FLESH_2, SPELL_INCINERATE_FLESH_3};
const uint32 m_idLegionFlame[4] = {SPELL_LEGION_FLAME_0, SPELL_LEGION_FLAME_1, SPELL_LEGION_FLAME_2, SPELL_LEGION_FLAME_3};
const uint32 m_idInfernalEruption[4] = {SPELL_INFERNAL_ERUPTION_0, SPELL_INFERNAL_ERUPTION_1, SPELL_INFERNAL_ERUPTION_2, SPELL_INFERNAL_ERUPTION_3};
const uint32 m_idNetherPortal[4] = {SPELL_NETHER_PORTAL_0, SPELL_NETHER_PORTAL_1, SPELL_NETHER_PORTAL_2, SPELL_NETHER_PORTAL_3};

enum Adds
{
    // Infernal
    SPELL_FEL_STREAK            = 66494,
    SPELL_FEL_STREAK_M          = 66493,
    SPELL_FEL_INFERNO           = 67046,

    // Mistress
    SPELL_SHIVAN_SLASH_0        = 66378,
    SPELL_SHIVAN_SLASH_1        = 67097,
    SPELL_SHIVAN_SLASH_2        = 67098,
    SPELL_SHIVAN_SLASH_3        = 67099,

    SPELL_SPINNING_PAIN_SPIKE   = 66283,

    SPELL_MISTRESS_KISS_2       = 67077,
    SPELL_MISTRESS_KISS_3       = 67078,

};

const uint32 m_idShivanSlash[4] = {SPELL_SHIVAN_SLASH_0, SPELL_SHIVAN_SLASH_1, SPELL_SHIVAN_SLASH_2, SPELL_SHIVAN_SLASH_3 };
const uint32 m_idMistressKiss[4] = {0,0, SPELL_MISTRESS_KISS_2, SPELL_MISTRESS_KISS_3 };

// should be all handled in core

enum Helpers
{
    // Legion Flame
    SPELL_LEGION_FLAME_AURA     = 66201,

    // Infernal Eruption
    SPELL_INFERNAL_ERUPTION_A_0 = 66252,
    SPELL_INFERNAL_ERUPTION_A_1 = 67067,
    SPELL_INFERNAL_ERUPTION_A_2 = 67068,
    SPELL_INFERNAL_ERUPTION_A_3 = 67069,

    // from aura trigger : 66253
    // triggers missile: 66255

    // Nether Portal
    SPELL_NETHER_PORTAL_AURA_0  = 66263,
    SPELL_NETHER_PORTAL_AURA_1  = 67103,
    SPELL_NETHER_PORTAL_AURA_2  = 67104,
    SPELL_NETHER_PORTAL_AURA_3  = 67105,
};

const uint32 m_idInfernalEruptionAura[4] = {SPELL_INFERNAL_ERUPTION_A_0, SPELL_INFERNAL_ERUPTION_A_1, SPELL_INFERNAL_ERUPTION_A_2, SPELL_INFERNAL_ERUPTION_A_3};
const uint32 m_idNetherPortalAura[4] = {SPELL_NETHER_PORTAL_AURA_0, SPELL_NETHER_PORTAL_AURA_1, SPELL_NETHER_PORTAL_AURA_2, SPELL_NETHER_PORTAL_AURA_3};

struct MANGOS_DLL_DECL boss_jaraxxusAI : public ScriptedAI
{
    boss_jaraxxusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;

    SpellTimer m_uiFelFireballTimer;
    SpellTimer m_uiFelLightningTimer;
    SpellTimer m_uiIncinerateFleshTimer;
    SpellTimer m_uiLegionFlameTimer;
    SpellTimer m_uiInfernalEruptionTimer;
    SpellTimer m_uiNetherPortalTimer;
    SpellTimer m_uiNetherPowerTimer;

    void Reset()
    {
        m_uiFelFireballTimer = SpellTimer(m_idFelFireball[m_dDifficulty], urand(4000,8000), urand(10000,15000));
        m_uiFelLightningTimer = SpellTimer(m_idFelLightning[m_dDifficulty], urand(6000,10000), urand(13000,17000));
        m_uiIncinerateFleshTimer = SpellTimer(m_idIncinerateFlesh[m_dDifficulty], urand(13000,15000), urand(18000,22000));
        m_uiLegionFlameTimer = SpellTimer(m_idLegionFlame[m_dDifficulty], urand(10000,15000), urand(25000,30000));
        m_uiInfernalEruptionTimer = SpellTimer(m_idInfernalEruption[m_dDifficulty], urand(70000,90000), urand(110000,120000));
        m_uiNetherPortalTimer = SpellTimer(m_idNetherPortal[m_dDifficulty], urand(15000,25000), urand(110000,120000));
        m_uiNetherPowerTimer = SpellTimer(SPELL_NETHER_POWER, 2000, 42000);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        //m_pInstance->SetData(TYPE_NORTHREND_BEASTS, GORMOK_DONE);
    }

    void JustReachedHome()
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_NORTHREND_BEASTS, FAIL);
        m_creature->ForcedDespawn();
    }

    void Aggro(Unit* pWho)
    {
        //m_pInstance->SetData(TYPE_NORTHREND_BEASTS, GORMOK_IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        bool isCasting = m_creature->IsNonMeleeSpellCasted(false);

        // Fel Fireball
        if (m_uiFelFireballTimer.CheckAndUpdate(uiDiff, isCasting))
        {
            m_creature->CastSpell(m_creature->getVictim(), m_uiFelFireballTimer.GetSpellId(), false);
            m_uiFelFireballTimer.AddCooldown();
        }

        // Fel Lightning
        if (m_uiFelLightningTimer.CheckAndUpdate(uiDiff, isCasting))
        {
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                m_creature->CastSpell(plr, m_uiFelLightningTimer.GetSpellId(), false);
            m_uiFelLightningTimer.AddCooldown();
        }

        // Incinerate Flesh
        if (m_uiIncinerateFleshTimer.CheckAndUpdate(uiDiff, isCasting))
        {
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                m_creature->CastSpell(plr, m_uiIncinerateFleshTimer.GetSpellId(), false);
            m_uiIncinerateFleshTimer.AddCooldown();
        }

        // Legion Flame
        if (m_uiLegionFlameTimer.CheckAndUpdate(uiDiff, isCasting))
        {
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                m_creature->CastSpell(plr, m_uiLegionFlameTimer.GetSpellId(), false);
            m_uiLegionFlameTimer.AddCooldown();
        }

        // Infernal Eruption
        if (m_uiInfernalEruptionTimer.CheckAndUpdate(uiDiff, isCasting))
        {
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                m_creature->CastSpell(m_creature, m_uiInfernalEruptionTimer.GetSpellId(), false);
            m_uiInfernalEruptionTimer.AddCooldown();
        }

        // Nether Portal
        if (m_uiNetherPortalTimer.CheckAndUpdate(uiDiff, isCasting))
        {
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                m_creature->CastSpell(m_creature, m_uiNetherPortalTimer.GetSpellId(), false);
            m_uiNetherPortalTimer.AddCooldown();
        }

        // Nether Power
        if (m_uiNetherPowerTimer.CheckAndUpdate(uiDiff, isCasting))
        {
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                m_creature->CastSpell(m_creature, m_uiNetherPowerTimer.GetSpellId(), false);
            m_uiNetherPowerTimer.AddCooldown();
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_felflame_infernalAI : public ScriptedAI
{
    npc_felflame_infernalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;

    SpellTimer m_uiFelStreakTimer;
    SpellTimer m_uiFelInfernoTimer;

    void Reset()
    {
        m_uiFelStreakTimer = SpellTimer(SPELL_FEL_STREAK, urand(8000,10000), urand(25000,30000));
        m_uiFelInfernoTimer = SpellTimer(SPELL_FEL_INFERNO, urand(10000,20000), urand(15000,25000));
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        bool isCasting = m_creature->IsNonMeleeSpellCasted(false);

        // Fel Streak
        if (m_uiFelStreakTimer.CheckAndUpdate(uiDiff, isCasting))
        {
            m_creature->CastSpell(m_creature, SPELL_FEL_STREAK_M, false);
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1))
                m_creature->CastSpell(plr, m_uiFelStreakTimer.GetSpellId(), true);
            m_uiFelStreakTimer.AddCooldown();
        }

        // Fel Inferno
        if (m_uiFelInfernoTimer.CheckAndUpdate(uiDiff, isCasting))
        {
            m_creature->CastSpell(m_creature, m_uiFelInfernoTimer.GetSpellId(), false);
            m_uiFelInfernoTimer.AddCooldown();
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_mistress_of_painAI : public ScriptedAI
{
    npc_mistress_of_painAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;

    SpellTimer m_uiShivanSlashTimer;
    SpellTimer m_uiSpinningPainSpikeTimer;
    SpellTimer m_uiMistressKiss;

    void Reset()
    {
        m_uiShivanSlashTimer = SpellTimer(m_idShivanSlash[m_dDifficulty], 15000, 15000);
        m_uiSpinningPainSpikeTimer = SpellTimer(SPELL_SPINNING_PAIN_SPIKE, 16000, 16000);
        m_uiMistressKiss = SpellTimer(m_idMistressKiss[m_dDifficulty], 17000, 17000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        bool isCasting = m_creature->IsNonMeleeSpellCasted(false);

        // Shivan Slash
        if (m_uiShivanSlashTimer.CheckAndUpdate(uiDiff, isCasting))
        {
            m_creature->CastSpell(m_creature->getVictim(), m_uiShivanSlashTimer.GetSpellId(), false);
            m_uiShivanSlashTimer.AddCooldown();
        }

        // Spinning Pain Spike
        if (m_uiSpinningPainSpikeTimer.CheckAndUpdate(uiDiff, isCasting))
        {
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                m_creature->CastSpell(plr, m_uiSpinningPainSpikeTimer.GetSpellId(), false);
            m_uiSpinningPainSpikeTimer.AddCooldown();
        }

        // Mistress's Kiss
        if (m_uiMistressKiss.CheckAndUpdate(uiDiff, isCasting))
        {
            if (uint32 spellId = m_uiMistressKiss.GetSpellId())
            {
                Unit* target;
                for(uint8 i = 0; i < 5; ++i)
                    if (target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                        if (target->getPowerType() == POWER_MANA)
                            break;

                m_creature->CastSpell(target, spellId, false);
            }
            m_uiMistressKiss.AddCooldown();
        }

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_jaraxxus(Creature* pCreature)
{
    return new boss_jaraxxusAI(pCreature);
}

CreatureAI* GetAI_felflame_infernal(Creature* pCreature)
{
    return new npc_felflame_infernalAI(pCreature);
}

CreatureAI* GetAI_mistress_of_pain(Creature* pCreature)
{
    return new npc_mistress_of_painAI(pCreature);
}

void AddSC_boss_jaraxxus()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_jaraxxus";
    newscript->GetAI = &GetAI_boss_jaraxxus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_felflame_infernal";
    newscript->GetAI = &GetAI_felflame_infernal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_mistress_of_pain";
    newscript->GetAI = &GetAI_mistress_of_pain;
    newscript->RegisterSelf();
}

/*
UPDATE creature_template SET ScriptName = 'boss_jaraxxus' WHERE entry=34780;
UPDATE creature_template SET ScriptName = 'npc_felflame_infernal' WHERE entry=34815;
UPDATE creature_template SET ScriptName = 'npc_mistress_of_pain' WHERE entry=34826;
*/
