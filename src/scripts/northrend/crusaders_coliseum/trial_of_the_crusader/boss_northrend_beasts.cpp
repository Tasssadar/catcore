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
// Gormok - Firebomb not implemented, timers need correct
// Snakes - Underground phase not worked, timers need correct
// Icehowl - Trample&Crash event not implemented, timers need correct

/* ScriptData
SDName: northrend_beasts
SD%Complete: 90% 
SDComment: by /dev/rsa
SDCategory:
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"
#include "PathFinder.h"

const float center[3] = {564.45f, 138.25f, 395.67f};

///////////////////////////
/////     GORMOK      /////
///////////////////////////

enum Gormok
{
    // Impale
    SPELL_IMPALE_0          = 66331,
    SPELL_IMPALE_1          = 67477,
    SPELL_IMPALE_2          = 67478,
    SPELL_IMPALE_3          = 67479,

    // Staggering Stomp
    SPELL_STAGGERING_STOMP_0= 66330,
    SPELL_STAGGERING_STOMP_1= 67647,
    SPELL_STAGGERING_STOMP_2= 67648,
    SPELL_STAGGERING_STOMP_3= 67649,

    // Snobolds
    SPELL_RISING_ANGER      = 66636,
    SPELL_SNOBOLLED         = 66406,
    NPC_SNOBOLD_VASSAL      = 34800,
    NPC_FIRE_BOMB           = 34854,
};

enum Snobold
{
    // Batter
    SPELL_BATTER            = 66408,
    // Fire Bomb
    SPELL_FIRE_BOMB         = 66313,
    // Head Crack
    SPELL_HEAD_CRACK        = 66407,
};

// spells with difficulty
const uint32 m_idImpale[4] = { SPELL_IMPALE_0, SPELL_IMPALE_1, SPELL_IMPALE_2, SPELL_IMPALE_3 };
const uint32 m_idStaggeringStomp[4] = {SPELL_STAGGERING_STOMP_0, SPELL_STAGGERING_STOMP_1, SPELL_STAGGERING_STOMP_2, SPELL_STAGGERING_STOMP_3};

struct MANGOS_DLL_DECL boss_gormokAI : public ScriptedAI
{
    boss_gormokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;

    uint32 m_uiImpaleTimer;
    uint32 m_uiStaggeringStompTimer;
    uint32 m_uiSnoboldsTimer;

    void Reset()
    {
        m_uiImpaleTimer = 10000;
        m_uiStaggeringStompTimer = 15000;
        m_uiSnoboldsTimer = 22000;
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_NORTHREND_BEASTS, GORMOK_DONE);
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
        m_pInstance->SetData(TYPE_NORTHREND_BEASTS, GORMOK_IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Impale
        if (HandleTimer(m_uiImpaleTimer, uiDiff, true))
        {
            if (m_creature->GetMap()->IsHeroicRaid() || !m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
            {
                DoCast(m_creature->getVictim(), m_idImpale[m_dDifficulty]);
                m_uiImpaleTimer = 10000;
            }
        }

        // Staggering Stomp
        if (HandleTimer(m_uiStaggeringStompTimer, uiDiff, true))
        {
            DoCast(m_creature, m_idStaggeringStomp[m_dDifficulty]);
            m_uiStaggeringStompTimer = 20000;
        }

        // Snobolds
        if (HandleTimer(m_uiSnoboldsTimer, uiDiff, true))
        {
            DoCast(m_creature, SPELL_RISING_ANGER);
            Player* plr = SelectRandomPlayerInRange(3, 15, true);
            if (Creature* crt = m_creature->SummonCreature(NPC_SNOBOLD_VASSAL, plr->GetLocation(), TEMPSUMMON_CORPSE_DESPAWN, 0))
            {
                crt->AI()->AttackStart(plr);
                crt->CastSpell(plr, SPELL_SNOBOLLED, true);
            }
            m_uiSnoboldsTimer = 20000;

        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_snoboldAI : public ScriptedAI
{
    npc_snoboldAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        // due to not working vehicles just make mob really fast
        m_creature->SetSpeedRate(MOVE_RUN, 10, true);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;

    uint32 m_uiBatterTimer;
    uint32 m_uiFireBombTimer;
    uint32 m_uiHeadCrackTimer;
    Unit* target;

    void Reset()
    {
        m_uiBatterTimer = 0;
        m_uiFireBombTimer = 15000;
        m_uiHeadCrackTimer = 22000;
        target = NULL;
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (!target)
            target = pWho;

        if (target != pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho, 999999);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if (IsCombatMovement())
                m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Batter
        if (HandleTimer(m_uiBatterTimer, uiDiff))
        {
            if (target && target->IsNonMeleeSpellCasted(false))
            {
                DoCast(target, SPELL_BATTER);
                m_uiBatterTimer = 10000;
            }
        }

        // Fire Bomb
        if (HandleTimer(m_uiFireBombTimer, uiDiff))
        {
            DoCast(target, SPELL_FIRE_BOMB);
            m_uiFireBombTimer = 20000;
        }

        // Head Crack
        if (HandleTimer(m_uiHeadCrackTimer, uiDiff))
        {
            DoCast(target, SPELL_HEAD_CRACK);
            m_uiHeadCrackTimer = 35000;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gormok(Creature* pCreature)
{
    return new boss_gormokAI(pCreature);
}

CreatureAI* GetAI_npc_snobold(Creature* pCreature)
{
    return new npc_snoboldAI(pCreature);
}

/*
INSERT INTO spell_script_target VALUES (66636, 1, 34796);
UPDATE creature_template SET AIName = 'NullAI' WHERE entry = 34854;
immune masky pro snobolda
*/

/////// END OF  GORMOK ////////

///////////////////////////
/////   JORMUNGARS    /////
///////////////////////////
enum Jormungars
{
    // Enrage
    SPELL_ENRAGE                = 68335,

    ///// MOBILE /////
    // Slime Pool
    SPELL_SLIME_POOL_0          = 66883,
    SPELL_SLIME_POOL_1          = 67641,
    SPELL_SLIME_POOL_2          = 67642,
    SPELL_SLIME_POOL_3          = 67643,
    NPC_SLIME_POOL              = 35176,

    //// STATIONARY ////
    // Sweep
    SPELL_SWEEP_0               = 66794,
    SPELL_SWEEP_1               = 67644,
    SPELL_SWEEP_2               = 67645,
    SPELL_SWEEP_3               = 67646,

    // there is more spell to be used, but who gives a shit
    // ok i do
    SPELL_SUBMERGE_0            = 66845,
    SPELL_SUBMERGE_1            = 66948,
    SPELL_EMERGE_0              = 66947,
    SPELL_EMERGE_1              = 66949,
};

enum Acidmaw
{
    // Paralytic Toxin (probably unused)
    SPELL_PARALYTIC_TOXIN_0     = 66823,
    SPELL_PARALYTIC_TOXIN_1     = 67618,
    SPELL_PARALYTIC_TOXIN_2     = 67619,
    SPELL_PARALYTIC_TOXIN_3     = 67620,

    ///// MOBILE /////
    // Acidic Spew
    SPELL_ACID_SPEW             = 66818,
    SPELL_ACID_SPEW_0           = 66819,
    SPELL_ACID_SPEW_1           = 67609,
    SPELL_ACID_SPEW_2           = 67610,
    SPELL_ACID_SPEW_3           = 67611,

    // Paralytic Bite
    SPELL_PARALYTIC_BITE_0      = 66824,
    SPELL_PARALYTIC_BITE_1      = 67612,
    SPELL_PARALYTIC_BITE_2      = 67613,
    SPELL_PARALYTIC_BITE_3      = 67614,

    //// STATIONARY ////
    // Acid Spit
    SPELL_ACID_SPIT_0           = 66880,
    SPELL_ACID_SPIT_1           = 67606,
    SPELL_ACID_SPIT_2           = 67607,
    SPELL_ACID_SPIT_3           = 67608,

    // Paralytic Spray
    SPELL_PARALYTIC_SPRAY_0     = 66901,
    SPELL_PARALYTIC_SPRAY_1     = 67615,
    SPELL_PARALYTIC_SPRAY_2     = 67616,
    SPELL_PARALYTIC_SPRAY_3     = 67617,
};

enum Dreadscale
{
    // Burning Bile (probably unused)
    SPELL_BURNING_BILE_0        = 66879,
    SPELL_BURNING_BILE_1        = 67624,
    SPELL_BURNING_BILE_2        = 67625,
    SPELL_BURNING_BILE_3        = 67626,

    ///// MOBILE /////
    // Molten Spew
    SPELL_MOLTEN_SPEW           = 66821,
    SPELL_MOLTEN_SPEW_0         = 66879,
    SPELL_MOLTEN_SPEW_1         = 67624,
    SPELL_MOLTEN_SPEW_2         = 67625,
    SPELL_MOLTEN_SPEW_3         = 67626,

    // Burning Bite
    SPELL_BURNING_BITE_0        = 66879,
    SPELL_BURNING_BITE_1        = 67624,
    SPELL_BURNING_BITE_2        = 67625,
    SPELL_BURNING_BITE_3        = 67626,

    //// STATIONARY ////
    // Fire Spit
    SPELL_FIRE_SPIT_0           = 66880,
    SPELL_FIRE_SPIT_1           = 67606,
    SPELL_FIRE_SPIT_2           = 67607,
    SPELL_FIRE_SPIT_3           = 67608,

    // Burning Spray
    SPELL_BURNING_SPRAY_0       = 66901,
    SPELL_BURNING_SPRAY_1       = 67615,
    SPELL_BURNING_SPRAY_2       = 67616,
    SPELL_BURNING_SPRAY_3       = 67617,
};

//const float idASpew[MAX_DIFFICULTY] = {SPELL_ACID_SPEW_0, SPELL_ACID_SPEW_1, SPELL_ACID_SPEW_2, SPELL_ACID_SPEW_3};
const uint32 idASpew[MAX_DIFFICULTY] = {SPELL_ACID_SPEW, SPELL_ACID_SPEW, SPELL_ACID_SPEW, SPELL_ACID_SPEW};
const uint32 idABite[MAX_DIFFICULTY] = {SPELL_PARALYTIC_BITE_0, SPELL_PARALYTIC_BITE_1, SPELL_PARALYTIC_BITE_2, SPELL_PARALYTIC_BITE_3};
const uint32 idASpit[MAX_DIFFICULTY] = {SPELL_ACID_SPIT_0, SPELL_ACID_SPIT_1, SPELL_ACID_SPIT_2, SPELL_ACID_SPIT_3};
const uint32 idASpray[MAX_DIFFICULTY]= {SPELL_PARALYTIC_SPRAY_0, SPELL_ACID_SPEW_1, SPELL_ACID_SPEW_2, SPELL_ACID_SPEW_3};

//const float idDSpew[MAX_DIFFICULTY] = {SPELL_MOLTEN_SPEW_0, SPELL_MOLTEN_SPEW_1, SPELL_MOLTEN_SPEW_2, SPELL_MOLTEN_SPEW_3 };
const uint32 idDSpew[MAX_DIFFICULTY] = {SPELL_MOLTEN_SPEW, SPELL_MOLTEN_SPEW, SPELL_MOLTEN_SPEW, SPELL_MOLTEN_SPEW};
const uint32 idDBite[MAX_DIFFICULTY] = {SPELL_BURNING_BITE_0, SPELL_BURNING_BITE_1, SPELL_BURNING_BITE_2, SPELL_BURNING_BITE_3 };
const uint32 idDSpit[MAX_DIFFICULTY] = {SPELL_FIRE_SPIT_0, SPELL_FIRE_SPIT_1, SPELL_FIRE_SPIT_2, SPELL_FIRE_SPIT_3};
const uint32 idDSpray[MAX_DIFFICULTY]= {SPELL_BURNING_SPRAY_0, SPELL_BURNING_SPRAY_1, SPELL_BURNING_SPRAY_2, SPELL_BURNING_SPRAY_3 };

const uint32 idSMerging[2] = {SPELL_SUBMERGE_0, SPELL_EMERGE_0};
const uint32 idMMerging[2] = {SPELL_SUBMERGE_1, SPELL_EMERGE_1};

const uint32 idSlimePool[MAX_DIFFICULTY] = {SPELL_SLIME_POOL_0, SPELL_SLIME_POOL_1, SPELL_SLIME_POOL_2, SPELL_SLIME_POOL_3};
const uint32 idSweep[MAX_DIFFICULTY] = {SPELL_SWEEP_0, SPELL_SWEEP_1, SPELL_SWEEP_2, SPELL_SWEEP_3};

struct MANGOS_DLL_DECL boss_jormungarsAI : public ScriptedAI
{
    boss_jormungarsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        Acidmaw = m_creature->GetEntry() == 35144;
        Dreadscale = m_creature->GetEntry() == 34799;
        if (Acidmaw)
        {
            m_idSpew = idASpew;
            m_idBite = idABite;			
            m_idSpit = idASpit;
            m_idSpray = idASpray;
        }
        else if (Dreadscale)
        {
            m_idSpew = idDSpew;
            m_idBite = idDBite;			
            m_idSpit = idDSpit;
            m_idSpray = idDSpray;
        }
        
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;
    const uint32 *m_idSpew;
    const uint32 *m_idBite;
    const uint32 *m_idSpit;
    const uint32 *m_idSpray;
    bool Acidmaw;
    bool Dreadscale;

    bool m_bIsMobile;
    bool m_bIsSubmerged;

    uint32 m_uiSpitTimer;

    uint32 m_uiSpewTimer;
    uint32 m_uiBiteTimer;
    uint32 m_uiSprayTimer;

    uint32 m_uiSlimePoolTimer;
    uint32 m_uiSweepTimer;

    uint32 m_uiSubmergeTimer;

    void Reset()
    {
        // set mobility
        m_bIsMobile = Dreadscale;
        SetCombatMovement(m_bIsMobile);

        m_bIsSubmerged = false;

        m_uiSpitTimer = 0;
 
        m_uiSlimePoolTimer = urand(10000,12000);
        m_uiSweepTimer = urand(15000,17000);

        m_uiSpewTimer = urand(20000,22000);
        m_uiBiteTimer = Acidmaw ? urand (20000,25000) : urand(10000,15000);
        m_uiSprayTimer = urand(20000,22000);
    }
    
    Creature* GetOtherJormungar()
    {
        Creature* crt;
        crt = GetClosestCreatureWithEntry(m_creature, (Acidmaw ? 34799 : 35144), DEFAULT_VISIBILITY_INSTANCE);
        if (!crt)
            crt = (Creature*)Unit::GetUnit(*m_creature, m_pInstance->GetData64(Acidmaw ? NPC_DREADSCALE : NPC_ACIDMAW));

        return crt;
    }

    void JustDied(Unit* killer)
    {
        if (Creature* crt = GetOtherJormungar())
            crt->CastSpell(crt, SPELL_ENRAGE, false);
    }
    void Submerge(bool apply)
    {
        // spells
        const uint32* ids = m_bIsMobile ? idMMerging : idSMerging;
        uint32 spellId = ids[!apply];
        m_creature->CastSpell(m_creature, spellId, false);
        if (apply)
        {
            //m_creature->SetVisibility(VISIBILITY_OFF);
            EnableAttack(false);
            SetCombatMovement(false);
        }
        else
        {
            //m_creature->SetVisibility(VISIBILITY_ON);
            EnableAttack(true);
            m_bIsMobile = !m_bIsMobile;
            SetCombatMovement(m_bIsMobile);

            WorldLocation loc = WorldLocation(m_creature->GetMapId(), 564,137, 396, 0);
            float rand_o = rand_norm_f()*2*M_PI_F;
            const float range = urand(0,30);
            loc.coord_x += range*cos(rand_o);
            loc.coord_y += range*sin(rand_o);
            m_creature->NearTeleportTo(loc.coord_x, loc.coord_y, loc.coord_z, loc.orientation);
        }
    }
    void UpdateAI(const uint32 uiDiff)
    {
        // Submerge
        if (HandleTimer(m_uiSubmergeTimer, uiDiff, true))
        {
            m_bIsSubmerged = !m_bIsSubmerged;
            Submerge(m_bIsSubmerged);
            // if submerged 10 sec after unsubmerge, else 45 sec to submerge
            if (m_bIsSubmerged)
                m_uiSubmergeTimer = 10000;
            else
                m_uiSubmergeTimer = 45000;

        }
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_bIsSubmerged)
            return;

        if (m_bIsMobile)
        {
            // Acidic Spew / Molten Spew
            if (HandleTimer(m_uiSpewTimer, uiDiff))
            {
                DoCast(m_creature->getVictim(), m_idSpew[m_dDifficulty]);
                m_uiSpewTimer = urand(20000,22000);
            }

            // Paralytic Bite / Burning Bite
            if (HandleTimer(m_uiBiteTimer, uiDiff))
            {
                DoCast(m_creature->getVictim(), m_idBite[m_dDifficulty]);
                m_uiBiteTimer = (Acidmaw ? urand(23,27) : urand(13, 17))*IN_MILLISECONDS;
            }
            // Slime Pool
            if (HandleTimer(m_uiSlimePoolTimer, uiDiff, true))
            {
                //TODO:: find spell effect, cast on sumoned 66882, handle dummy, probably best way is in sd2
                DoCast(m_creature, idSlimePool[m_dDifficulty]);

            }
        }
        else
        {
            // Paralytic Spray / Burning Spray
            if (HandleTimer(m_uiSprayTimer, uiDiff))
            {
                if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                {
                    m_creature->SetFacingToObject(target);
                    DoCast(target, m_idSpray[m_dDifficulty]);
                }
                m_uiSprayTimer = urand(20000,24000);
            }
            // Sweep
            if (HandleTimer(m_uiSweepTimer, uiDiff))
            {
                DoCast(m_creature, idSweep[m_dDifficulty]);
                m_uiSweepTimer = urand(15000,19000);
            }
        }
        
        // using "spam" abilities
        if (!m_bIsMobile)
        {
            // Spit
            if (HandleTimer(m_uiSpitTimer, uiDiff))
            {
                if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM,0))
                    DoCast(target, m_idSpit[m_dDifficulty]);
                m_uiSpitTimer = 1000;
            }
        }
        else
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_jormungars(Creature* pCreature)
{
    return new boss_jormungarsAI(pCreature);
}

/////// END OF  JORMUNGARS ////////

//////////////////////////
//////    ICEHOWL   //////
//////////////////////////
enum Icehowl
{
    SPELL_FEROCIOUS_BUTT_0      = 66770,
    SPELL_FEROCIOUS_BUTT_1      = 67654,
    SPELL_FEROCIOUS_BUTT_2      = 67655,
    SPELL_FEROCIOUS_BUTT_3      = 67666,
    SPELL_ARCTIC_BREATH_0       = 66689,
    SPELL_ARCTIC_BREATH_1       = 67650,
    SPELL_ARCTIC_BREATH_2       = 67651,
    SPELL_ARCTIC_BREATH_3       = 67652,
    SPELL_WHIRL_0               = 67345,
    SPELL_WHIRL_1               = 67664,
    SPELL_WHIRL_2               = 67663,
    SPELL_WHIRL_3               = 67665,
    SPELL_MASSIVE_CRASH_0       = 66683,
    SPELL_MASSIVE_CRASH_1       = 67661,
    SPELL_MASSIVE_CRASH_2       = 67660,
    SPELL_MASSIVE_CRASH_3       = 67662,
    SPELL_TRAMPLE               = 66734,
    SPELL_STAGGERED_DAZE        = 66758,
    SPELL_ENRAGE_ICE            = 66759
};

const uint32 m_idFerociousButt[4] = {SPELL_FEROCIOUS_BUTT_0, SPELL_FEROCIOUS_BUTT_1, SPELL_FEROCIOUS_BUTT_2, SPELL_FEROCIOUS_BUTT_3};
const uint32 m_idArcticBreath[4] = {SPELL_ARCTIC_BREATH_0, SPELL_ARCTIC_BREATH_1, SPELL_ARCTIC_BREATH_2, SPELL_ARCTIC_BREATH_3};
const uint32 m_idWhirl[4] = {SPELL_WHIRL_0, SPELL_WHIRL_1, SPELL_WHIRL_2, SPELL_WHIRL_3};
const uint32 m_idMassiveCrash[4] = {SPELL_MASSIVE_CRASH_0, SPELL_MASSIVE_CRASH_1, SPELL_MASSIVE_CRASH_2, SPELL_MASSIVE_CRASH_3};

struct MANGOS_DLL_DECL boss_icehowlAI : public ScriptedAI
{
    boss_icehowlAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }
    // 0 odkopnuti // 4 emote  otoci se zarve // 6.5 spadne stun a odskočí // 8 charguje

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;

    uint32 m_uiChargeStepTimer;
    int8 m_uiChargeStepCount;
    uint32 m_uiFerociousButtTimer;
    uint32 m_uiArcticBreathTimer;
    uint32 m_uiWhirlTimer;
    uint32 m_uiMassiveCrashTimer;

    uint32 m_uiArcticBreathDefixTimer;

    PathNode *m_chargeTargetPos;
    bool m_trample;
    bool m_sombodyDied;
    uint32 m_trampleTimer;
    std::set<uint32> m_hitPlayers;

    void Reset()
    {
        setInitTimers();

        m_uiArcticBreathDefixTimer = 0;
        m_uiChargeStepCount = -1;
        m_trample = false;
        m_creature->GetMotionMaster()->Clear(false, true);
        SetCombatMovement(true);
    }

    void setInitTimers()
    {
        m_uiFerociousButtTimer = 10000;
        m_uiArcticBreathTimer = 20000;
        m_uiWhirlTimer = 15000;
        m_uiMassiveCrashTimer = 35000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_creature->hasUnitState(UNIT_STAT_STUNNED))
            return;

        if (m_trample)
        {
            if (m_trampleTimer <= uiDiff)
            {
                PlrList pList = GetAttackingPlayers();
                for (PlrList::iterator itr = pList.begin(); itr != pList.end(); ++itr)
                {
                    if (!(*itr) || !(*itr)->IsInWorld() || !(*itr)->isAlive())
                        continue;

                    if (m_hitPlayers.find((*itr)->GetGUIDLow()) == m_hitPlayers.end() &&
                        (*itr)->IsWithinDist2d(m_creature->GetPositionX(), m_creature->GetPositionY(), 12.0f))
                    {
                        m_hitPlayers.insert((*itr)->GetGUIDLow());
                        m_sombodyDied = true;
                        m_creature->DealDamage(*itr, 50000, NULL, SPELL_DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NATURE, NULL, false);
                    }
                }
                m_trampleTimer = 200;
            }else m_trampleTimer -= uiDiff;
        }
        
        // remove target fix from arctic breath
        if (m_uiArcticBreathDefixTimer)
        {
            if (m_uiArcticBreathDefixTimer < uiDiff)
            {
                m_creature->FixOrientation();
                m_uiArcticBreathDefixTimer = 0;
            }else m_uiArcticBreathDefixTimer -= uiDiff;
            return;
        }

        // handling steps of charge
        if (m_uiChargeStepCount != -1)
        {
            if (m_uiChargeStepTimer < uiDiff)
            {
                switch (m_uiChargeStepCount)
                {
                    case 0:
                    {
                        m_creature->CastSpell(m_creature, m_idMassiveCrash[m_dDifficulty], false);
                        m_uiChargeStepTimer = 2500;
                        break;
                    }
                    case 1:
                    {
                        Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);
                        if (plr)
                            m_creature->FixOrientation(m_creature->GetAngle(plr));
                        m_creature->SetUInt64Value(UNIT_FIELD_TARGET, plr->GetGUID());
                        //emote
                        m_chargeTargetPos = new PathNode(plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ());
                        m_uiChargeStepTimer = 2500;
                        break;
                    }
                    case 2:
                    {
                        float ang;
                        PointPath pointPath;
                        pointPath.resize(2);
                        m_creature->GetPosition(pointPath[0].x, pointPath[0].y, pointPath[0].z);
                        m_creature->GetPosition(pointPath[1].x, pointPath[1].y, pointPath[1].z);
                        
                        ang = m_creature->GetFixedOrientation() + M_PI_F;
                        ang = ang > M_PI_F*2 ? ang - M_PI_F*2 : ang;
                        pointPath[1].x += cos(ang)*35.0f;
                        pointPath[1].y += sin(ang)*35.0f;
                        m_chargeTargetPos->x += cos(ang)*8;
                        m_chargeTargetPos->y += sin(ang)*8;
                        m_creature->GetMotionMaster()->MoveCharge(pointPath, 1000.0f, 1, 1);
                        m_creature->SendTrajMonsterMove(pointPath[1].x, pointPath[1].y, pointPath[1].z, true, 100.0f, 1000, SPLINETYPE_FACINGANGLE, m_creature->GetFixedOrientation());
                        m_uiChargeStepTimer = 4500;
                        break;
                    }
                    case 3:
                    {
                        PointPath pointPath;
                        pointPath.resize(2);
                        m_creature->GetPosition(pointPath[0].x, pointPath[0].y, pointPath[0].z);
                        pointPath.set(1, *m_chargeTargetPos);
                        m_creature->SendMonsterMove(m_chargeTargetPos->x, m_chargeTargetPos->y, m_chargeTargetPos->z, SPLINETYPE_NORMAL, SPLINEFLAG_WALKMODE, 1000);
                        m_creature->GetMotionMaster()->MoveCharge(pointPath, 1000.0f, 1, 1);
                        m_uiChargeStepTimer = 1200;
                        m_hitPlayers.clear();
                        m_trample = true;
                        m_sombodyDied = false;
                        m_trampleTimer = 100;
                        break;
                    }
                    case 4:
                    {
                        delete m_chargeTargetPos;
                        m_chargeTargetPos = NULL;
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                        m_creature->SetUInt64Value(UNIT_FIELD_TARGET, m_creature->getVictim()->GetGUID());
                        SetCombatMovement(true);
                        m_creature->CastSpell(m_creature, SPELL_TRAMPLE, true);
                        if (m_sombodyDied)
                            m_creature->CastSpell(m_creature, SPELL_ENRAGE_ICE, false);
                        else
                            m_creature->CastSpell(m_creature, SPELL_STAGGERED_DAZE, false);
                        m_creature->FixOrientation();
                        setInitTimers();
                        break;
                    }
                }
                ++m_uiChargeStepCount;
                if (m_uiChargeStepCount == 5)
                    m_uiChargeStepCount = -1;

            }else m_uiChargeStepTimer -= uiDiff;

            return;
        }

        // Ferocious Butt
        if (HandleTimer(m_uiFerociousButtTimer, uiDiff, true))
        {
            m_creature->CastSpell(m_creature->getVictim(), m_idFerociousButt[m_dDifficulty], false);
            m_uiFerociousButtTimer = 10000;
        }

        // Whirl
        if (HandleTimer(m_uiWhirlTimer, uiDiff, true))
        {
            m_creature->CastSpell(m_creature, m_idWhirl[m_dDifficulty], false);
            m_uiWhirlTimer = 15000;
        }

        // Arctic Breath
        if (HandleTimer(m_uiArcticBreathTimer, uiDiff, true))
        {
            if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
            {
                m_creature->CastSpell(plr, m_idArcticBreath[m_dDifficulty], false);
                m_creature->FixOrientation(m_creature->GetAngle(plr));
            }
            m_uiArcticBreathDefixTimer = 5000;

            m_uiArcticBreathTimer = 20000;
        }

        // Massive Crash
        if (HandleTimer(m_uiMassiveCrashTimer, uiDiff, true))
        {
            PointPath pointPath;
            pointPath.resize(2);
            SetCombatMovement(false);
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetPosition(pointPath[0].x, pointPath[0].y, pointPath[0].z);
            pointPath[1].x = SpawnLoc[1].x;
            pointPath[1].y = SpawnLoc[1].y;
            pointPath[1].z = SpawnLoc[1].z;
            m_creature->SendMonsterMove(SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, SPLINETYPE_NORMAL, SPLINEFLAG_WALKMODE, 1000);
            m_creature->GetMotionMaster()->MoveCharge(pointPath, 1000.0f, 1, 1);

            m_uiChargeStepTimer = 1000;
            m_uiChargeStepCount = 0;

            m_uiMassiveCrashTimer = 35000;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_icehowl(Creature* pCreature)
{
    return new boss_icehowlAI(pCreature);
}

void AddSC_northrend_beasts()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_gormok";
    newscript->GetAI = &GetAI_boss_gormok;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_snobold";
    newscript->GetAI = &GetAI_npc_snobold;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_jormungars";
    newscript->GetAI = &GetAI_boss_jormungars;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_icehowl";
    newscript->GetAI = &GetAI_boss_icehowl;
    newscript->RegisterSelf();
}

/*
UPDATE creature SET spawnMask = 15 WHERE map = 649;
UPDATE gameobject SET spawnMask = 15 WHERE map = 649;
UPDATE creature_template SET ScriptName = 'boss_gormok' WHERE entry = 34796;
UPDATE creature_template SET ScriptName = 'npc_snobold' WHERE entry = 34800;
UPDATE creature_template SET ScriptName = 'boss_jormungars' WHERE entry = 35144;
UPDATE creature_template SET ScriptName = 'boss_jormungars' WHERE entry = 34799;
UPDATE creature_template SET ScriptName = 'boss_icehowl' WHERE entry = 34797;
*/
