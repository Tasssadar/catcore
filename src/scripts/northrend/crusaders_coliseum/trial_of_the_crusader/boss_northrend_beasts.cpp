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
SDName: northrend_beasts
SD%Complete: 90% 
SDComment: by /dev/rsa
SDCategory:
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"
#include "PathFinder.h"

enum Say
{
    //Gormok
    SAY_SNOBOLLED        = -1649000,
    //Acidmaw & Dreadscale
    SAY_SUBMERGE         = -1649010,
    SAY_EMERGE           = -1649011,
    SAY_BERSERK          = -1649012,
    //Icehowl
    SAY_TRAMPLE_STARE    = -1649020,
    SAY_TRAMPLE_FAIL     = -1649021,
    SAY_TRAMPLE_START    = -1649022,
};

///////////////////////////
/////     GORMOK      /////
///////////////////////////

enum GormokTimers
{
    TIMER_IMPALE=0,
    TIMER_STAGGERING_STOMP,
    TIMER_DO_SNOBOLDS
};

enum GormokSpells
{

    SPELL_IMPALE            = 66331, // Impale
    SPELL_STAGGERING_STOMP  = 66330, // Staggering Stomp

    // Snobolds
    SPELL_RISING_ANGER      = 66636,
    SPELL_SNOBOLLED         = 66406,
    NPC_SNOBOLD_VASSAL      = 34800,
    NPC_FIRE_BOMB           = 34854
};

enum SnoboldTimers
{
    TIMER_BATTER = 0,
    TIMER_FIRE_BOMB,
    TIMER_HEAD_CRACK
};

enum SnoboldSpells
{
    SPELL_BATTER            = 66408, // Batter
    SPELL_FIRE_BOMB         = 66313, // Fire Bomb
    SPELL_HEAD_CRACK        = 66407  // Head Crack
};

struct MANGOS_DLL_DECL northrend_beast_base : public ScriptedAI
{
    northrend_beast_base(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        isHeroic = pCreature->GetMap()->IsHeroicRaid();
        pCreature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        //m_bAttackEnabled = false;
        if (!m_pInstance)
            m_creature->ForcedDespawn();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;
    bool isHeroic;

    void JustReachedHome()
    {
        m_pInstance->SetData(TYPE_BEASTS, FAIL);

        m_creature->ForcedDespawn();
    }
};

struct MANGOS_DLL_DECL boss_gormokAI : public northrend_beast_base
{
    boss_gormokAI(Creature* pCreature) : northrend_beast_base(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        isHeroic = pCreature->GetMap()->IsHeroicRaid();
        Reset();
    }

    void Reset()
    {
        AddTimer(TIMER_IMPALE, SPELL_IMPALE, RV(9000,11000), RV(9000,11000), UNIT_SELECT_VICTIM, CAST_TYPE_QUEUE);
        AddTimer(TIMER_STAGGERING_STOMP, SPELL_STAGGERING_STOMP, RV(13000,17000), RV(17000,22000), UNIT_SELECT_SELF, CAST_TYPE_FORCE);
        AddTimer(TIMER_DO_SNOBOLDS, SPELL_RISING_ANGER, RV(20000,25000), RV(17000,22000), UNIT_SELECT_SELF, CAST_TYPE_FORCE);
    }

    void Aggro(Unit* )
    {
        m_pInstance->SetData(TYPE_BEASTS, GORMOK_IN_PROGRESS);
    }

    void JustDied(Unit* )
    {
        m_pInstance->SetData(TYPE_BEASTS, GORMOK_DONE);
    }

    Player* GetSnoboledTarget()
    {
        PlrList allApropriate = GetRandomPlayersInRange(100, 3, 15, DEFAULT_VISIBILITY_INSTANCE, true);
        PlrList noSnoboled;
        for(PlrList::iterator itr = allApropriate.begin(); itr != allApropriate.end(); ++itr)
            if (!(*itr)->HasAura(SPELL_SNOBOLLED) && (*itr)->getClass() != CLASS_HUNTER)
                noSnoboled.push_back(*itr);

        PlrList& selectList = noSnoboled.empty() ? allApropriate : noSnoboled;
        PlrList::iterator itr = selectList.begin();
        std::advance(itr, urand(0, selectList.size()-1));
        return *itr;
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Impale
        if (isHeroic || !m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISARMED))
            m_TimerMgr->TimerFinished(TIMER_IMPALE);

        // Staggering Stomp
        m_TimerMgr->TimerFinished(TIMER_STAGGERING_STOMP);

        // Snobolds
        if (m_TimerMgr->TimerFinished(TIMER_DO_SNOBOLDS))
        {
            Player* plr = GetSnoboledTarget();
            if (!plr)
                return;

            if (Creature* crt = m_creature->SummonCreature(NPC_SNOBOLD_VASSAL, plr->GetPosition(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
            {
                crt->AI()->AttackStart(plr);
                plr->CastSpell(plr, SPELL_SNOBOLLED, true, NULL, NULL, crt->GetGUID());
                DoScriptText(SAY_SNOBOLLED, m_creature);
            }
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
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;

    uint32 m_uiBatterTimer;
    uint32 m_uiFireBombTimer;
    uint32 m_uiHeadCrackTimer;
    Unit* fixTarget;

    void Reset()
    {
        fixTarget = SelectNearestPlayer();

        AddTimer(TIMER_BATTER, SPELL_BATTER, 0, 10000, UNIT_SELECT_NONE, CAST_TYPE_FORCE);
        AddTimer(TIMER_FIRE_BOMB, SPELL_FIRE_BOMB, 15000, RV(25000,30000), UNIT_SELECT_NONE, CAST_TYPE_FORCE);
        AddTimer(TIMER_HEAD_CRACK, SPELL_HEAD_CRACK, RV(20000,25000), RV(30000,40000), UNIT_SELECT_NONE, CAST_TYPE_FORCE);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (pWho != fixTarget)
            pWho = fixTarget;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho, 999999.f);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);

            if (IsCombatMovement())
                m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    Player* SelectNearestPlayer()
    {
        Player* nearest = NULL;
        float neardist = 999.0f;
        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* plr = itr->getSource();
            if (!plr)
                continue;

            float currdist = plr->GetDistance(m_creature);
            if (currdist < neardist)
            {
                nearest = plr;
                neardist = currdist;
            }
        }
        return nearest;
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Batter
        if (fixTarget->IsNonMeleeSpellCasted(false))
            m_TimerMgr->TimerFinished(TIMER_BATTER, fixTarget);

        // Fire Bomb
        m_TimerMgr->TimerFinished(TIMER_FIRE_BOMB, fixTarget);

        // Head Crack
        m_TimerMgr->TimerFinished(TIMER_HEAD_CRACK, fixTarget);

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

/////// END OF  GORMOK ////////

///////////////////////////
/////   JORMUNGARS    /////
///////////////////////////
enum JormungarTimer
{
    TIMER_SLIME_POOL,
    TIMER_SPEW,
    TIMER_BITE,
    TIMER_SWEEP,
    TIMER_SPIT,
    TIMER_SPRAY,
    TIMER_MERGING
};

enum JormungarSpell
{
    SPELL_ENRAGE        = 68335, // Enrage

    //**************
    //* MOBILE ONLY
    //**************
    SPELL_SLIME_POOL    = 66883, // Slime Pool summon
    NPC_SLIME_POOL      = 35176, // Slime Pool npc

    //* Acidmaw
    //**********
    SPELL_ACID_SPEW     = 66818, // Acidic Spew
    SPELL_PARALYTIC_BITE= 66824, // Paralytic Bite

    //* Dreadscale
    //*************
    SPELL_MOLTEN_SPEW   = 66821, // Molten Spew
    SPELL_BURNING_BITE  = 66879, // Burning Bite


    //******************
    //* STATIONARY ONLY
    //******************
    SPELL_SWEEP         = 66794, // Sweep

    //* Acidmaw
    //**********
    SPELL_ACID_SPIT      = 66880, // Acid Spit
    SPELL_PARALYTIC_SPRAY= 66901, // Paralytic Spray

    //* Dreadscale
    //*************
    SPELL_FIRE_SPIT      = 66796, // Fire Spit
    SPELL_BURNING_SPRAY  = 66902, // Burning Spray

    // merging
    SPELL_SUBMERGE_0     = 66845,
    SPELL_SUBMERGE_1     = 66948,
    SPELL_EMERGE_0       = 66947,
    SPELL_EMERGE_1       = 66949

    //SPELL_SLIME_POOL_PROC       = 66882, // Slime Pool proc
    //SPELL_ACID_SPEW_PROC        = 66819, // Acidic Spew proc
    //SPELL_PARALYTIC_TOXIN_PROC  = 66823, // Paralytic Bite - proc
    //SPELL_MOLTEN_SPEW_PROC      = 66879, // Molten Spew proc
    //SPELL_BURNING_BILE_PROC     = 66869, // Burning Bite proc
};

enum PhaseAllow
{
    PHASE_MERGED    = 0,
    PHASE_MOVE      = 1,
    PHASE_STAND     = 2
};

struct MANGOS_DLL_DECL boss_jormungarsAI : public northrend_beast_base
{
    boss_jormungarsAI(Creature* pCreature) : northrend_beast_base(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        Acidmaw = pCreature->GetEntry() == 35144;
        Dreadscale = pCreature->GetEntry() == 34799;
        if (!Acidmaw && !Dreadscale)
            pCreature->ForcedDespawn();

        pCreature->SetSpeedRate(MOVE_RUN, 1.5f);
        Reset();
    }

    bool Acidmaw;
    bool Dreadscale;

    bool m_bIsSubmerged;
    bool m_bIsMobile;

    void Reset()
    {
        AddTimer(TIMER_SLIME_POOL, SPELL_SLIME_POOL, RV(10000,12000), RV(30000,40000), UNIT_SELECT_NONE, CAST_TYPE_NONCAST);
        AddTimer(TIMER_SPEW, Acidmaw ? SPELL_ACID_SPEW : SPELL_MOLTEN_SPEW, RV(20000,22000), RV(20000,22000), UNIT_SELECT_VICTIM, CAST_TYPE_NONCAST);
        AddTimer(TIMER_BITE, Acidmaw ?  SPELL_PARALYTIC_BITE : SPELL_BURNING_BITE,  Acidmaw ? RV (20000,25000) : RV(10000,15000), Acidmaw ? RV(23000,27000) : RV(13000, 17000), UNIT_SELECT_VICTIM, CAST_TYPE_NONCAST);
        AddTimer(TIMER_SWEEP, SPELL_SWEEP, RV(15000,17000), RV(15000,19000), UNIT_SELECT_SELF);
        AddTimer(TIMER_SPIT, Acidmaw ? SPELL_ACID_SPIT : SPELL_FIRE_SPIT, 0, 1000, UNIT_SELECT_RANDOM_PLAYER, CAST_TYPE_NONCAST, 0);
        AddTimer(TIMER_SPRAY, Acidmaw ? SPELL_PARALYTIC_SPRAY : SPELL_BURNING_SPRAY, RV(20000,22000), RV(20000,24000), UNIT_SELECT_RANDOM_PLAYER, CAST_TYPE_FORCE, 0);
        AddNonCastTimer(TIMER_MERGING, RV(40000,50000), RV(40000,50000));

        cat_log("OWNER: %s is Dreadscale %u so phase is %u", m_creature->GetName(), uint32(Dreadscale), Dreadscale ? uint32(PHASE_MOVE) : uint32(PHASE_STAND));
        SetPhase(Dreadscale ? PHASE_MOVE : PHASE_STAND);
    }
    
    Creature* GetBro()
    {
        Creature* crt = GetClosestCreatureWithEntry(m_creature, Acidmaw ? 34799 : 35144, DEFAULT_VISIBILITY_INSTANCE);
        if (!crt)
            crt = m_pInstance->GetCreature(Acidmaw ? NPC_DREADSCALE : NPC_ACIDMAW);

        if (!crt->isAlive() || !crt->IsInWorld())
            return NULL;

        return crt;
    }

    void SetPhase(PhaseAllow phase)
    {
        m_bIsSubmerged = phase == PHASE_MERGED;

        if (phase == PHASE_MERGED)
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->AttackStop();
        }
        else
        {
            m_bIsMobile = phase == PHASE_MOVE;

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->AttackStop();
        }

        //EnableAttack(canMoveAndAttack);
        SetCombatMovement(phase == PHASE_MOVE);
        cat_log("OWNER: %s phase is %u, submerged %u, mobility %u, canmove %u", m_creature->GetName(), uint32(phase),
             uint32(m_bIsSubmerged), uint32(m_bIsMobile), uint32(phase == PHASE_MOVE));

        m_TimerMgr->SetValue(TIMER_SLIME_POOL, TIMER_VALUE_UPDATEABLE, phase == PHASE_MOVE);
        m_TimerMgr->SetValue(TIMER_SPEW, TIMER_VALUE_UPDATEABLE, phase == PHASE_MOVE);
        m_TimerMgr->SetValue(TIMER_BITE, TIMER_VALUE_UPDATEABLE, phase == PHASE_MOVE);
        m_TimerMgr->SetValue(TIMER_SWEEP, TIMER_VALUE_UPDATEABLE, phase == PHASE_STAND);
        m_TimerMgr->SetValue(TIMER_SPIT, TIMER_VALUE_UPDATEABLE, phase == PHASE_STAND);
        m_TimerMgr->SetValue(TIMER_SPRAY, TIMER_VALUE_UPDATEABLE, phase == PHASE_STAND);
    }

    void Aggro(Unit *)
    {
        m_pInstance->SetData(TYPE_BEASTS, SNAKES_IN_PROGRESS);
    }

    void JustDied(Unit* )
    {
        Creature* bro = GetBro();
        if (bro && bro->isAlive())
        {
            bro->CastSpell(bro, SPELL_ENRAGE, true);
            DoScriptText(SAY_BERSERK, bro);
            m_pInstance->SetData(TYPE_BEASTS, SNAKES_ONE_DOWN);
        }
        else
            m_pInstance->SetData(TYPE_BEASTS, SNAKES_DONE);
    }

    boss_jormungarsAI* GetBroAI()
    {
        Creature* bro = GetBro();
        if (!bro)
            return NULL;

        return (boss_jormungarsAI*)bro->AI();
    }

    void DamageTaken(Unit *, uint32 &uiDamage)
    {
        if (m_bIsSubmerged)
            uiDamage = 0;
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Merging
        if (SpellTimer* mergeTimer = m_TimerMgr->TimerFinished(TIMER_MERGING))
        {
            bool submerge = !m_bIsSubmerged; // switch submerge/emerge state
            Creature* bro = GetBro();
            if (submerge)
            {
                SetPhase(PHASE_MERGED);
                if (boss_jormungarsAI* ai = GetBroAI())
                    ai->SetPhase(PHASE_MERGED);
                DoScriptText(SAY_SUBMERGE, m_creature);
                DoScriptText(SAY_SUBMERGE, bro);
            }
            else
            {
                bool thisShouldMove = !m_bIsMobile;
                SetPhase(thisShouldMove ? PHASE_MOVE : PHASE_STAND);
                if (boss_jormungarsAI* ai = GetBroAI())
                    ai->SetPhase(thisShouldMove ? PHASE_STAND : PHASE_MOVE);

                Coords coord = SpawnLoc[LOC_CENTER];
                Coords coord2 = SpawnLoc[LOC_CENTER];
                const float rand_o = rand_norm_f()*2*M_PI_F;
                const float range = urand(0,30);
                coord.x += range*cos(rand_o);
                coord.y += range*sin(rand_o);
                coord2.x += range*cos(rand_o+M_PI_F);
                coord2.y += range*sin(rand_o+M_PI_F);

                if (Map* m = m_creature->GetMap())
                {
                    m->CreatureRelocation(m_creature, coord, 0);
                    if (bro)
                        m->CreatureRelocation(bro, coord2, 0);
                }

                DoScriptText(SAY_EMERGE, m_creature);
                DoScriptText(SAY_EMERGE, bro);

                AttackStart(m_pInstance->GetRandomPlayerInMap());
                if (boss_jormungarsAI* ai = GetBroAI())
                    ai->AttackStart(m_pInstance->GetRandomPlayerInMap());
            }

            m_creature->AddAndLinkAura(SPELL_SUBMERGE_0, submerge);
            if (bro)
                bro->AddAndLinkAura(SPELL_SUBMERGE_0, submerge);

            mergeTimer->Cooldown(submerge ? RV(5000, 10000): RV(40000,50000));
            if (bro)
                bro->GetTimerMgr()->Cooldown(TIMER_MERGING, submerge ? RV(5000, 10000): RV(40000,50000));
        }

        // Spew
        m_TimerMgr->TimerFinished(TIMER_SPEW);

        // Bite
        m_TimerMgr->TimerFinished(TIMER_BITE);

        // Slime Pool
        m_TimerMgr->TimerFinished(TIMER_SLIME_POOL);

        // Spray
        m_TimerMgr->TimerFinished(TIMER_SPRAY);

        // Sweep
        m_TimerMgr->TimerFinished(TIMER_SWEEP);

        // Spit
        m_TimerMgr->TimerFinished(TIMER_SPIT);

        if (m_bIsMobile)
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

enum IcehowlTimer
{
    TIMER_BUTT = 0,
    TIMER_BREATH,
    TIMER_WHIRL,
    TIMER_MASSIVE_CRASH
};

enum IcehowlSpell
{
    SPELL_FEROCIOUS_BUTT        = 66770,
    SPELL_ARCTIC_BREATH         = 66689,
    SPELL_WHIRL                 = 67345,
    SPELL_MASSIVE_CRASH         = 66683,
    SPELL_TRAMPLE               = 66734,
    SPELL_STAGGERED_DAZE        = 66758,
    SPELL_ENRAGE_ICE            = 66759
};

struct MANGOS_DLL_DECL boss_icehowlAI : public northrend_beast_base
{
    boss_icehowlAI(Creature* pCreature) : northrend_beast_base(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        Reset();
    }


    uint32 m_uiChargeStepTimer;
    int8 m_uiChargeStepCount;

    bool isCastingBreath;

    Coords *m_chargeTargetPos;
    bool m_trample;
    bool m_sombodyDied;
    uint32 m_trampleTimer;
    std::set<uint32> m_hitPlayers;

    void Reset()
    {
        AddTimer(TIMER_BUTT, SPELL_FEROCIOUS_BUTT, RV(10000, 13000), RV(10000,15000), UNIT_SELECT_VICTIM);
        AddTimer(TIMER_WHIRL, SPELL_WHIRL, RV(15000, 17000), RV(10000,15000), UNIT_SELECT_SELF);
        AddTimer(TIMER_BREATH, SPELL_ARCTIC_BREATH, 20000, 20000, UNIT_SELECT_RANDOM_PLAYER, CAST_TYPE_NONCAST, 0);
        AddNonCastTimer(TIMER_MASSIVE_CRASH, 35000, RV(30000,35000));

        isCastingBreath = false;
        m_uiChargeStepCount = -1;
        m_trample = false;
        m_creature->GetMotionMaster()->Clear(false, true);
        SetCombatMovement(true);
    }

    void setUpdatableForTimers(bool update)
    {
        m_TimerMgr->SetValue(TIMER_BUTT, TIMER_VALUE_UPDATEABLE, update);
        m_TimerMgr->SetValue(TIMER_BREATH, TIMER_VALUE_UPDATEABLE, update);
        m_TimerMgr->SetValue(TIMER_WHIRL, TIMER_VALUE_UPDATEABLE, update);
        m_TimerMgr->SetValue(TIMER_MASSIVE_CRASH, TIMER_VALUE_UPDATEABLE, update);
    }

    void Aggro(Unit *)
    {
        m_pInstance->SetData(TYPE_BEASTS, ICEHOWL_IN_PROGRESS);
    }

    void JustDied(Unit *)
    {
        m_pInstance->SetData(TYPE_BEASTS, ICEHOWL_DONE);
        m_pInstance->SetData(TYPE_BEASTS, DONE);
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
        
        // handling steps of charge
        if (m_uiChargeStepCount != -1)
        {
            if (m_uiChargeStepTimer < uiDiff)
            {
                switch (m_uiChargeStepCount)
                {
                    case 0:
                    {
                        m_creature->CastSpell(m_creature, SPELL_MASSIVE_CRASH, false);
                        m_uiChargeStepTimer = 2500;
                        break;
                    }
                    case 1:
                    {
                        Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);
                        if (plr)
                            m_creature->FixOrientation(m_creature->GetAngle(plr));
                        m_creature->SetUInt64Value(UNIT_FIELD_TARGET, plr->GetGUID());
                        DoScriptText(SAY_TRAMPLE_STARE, m_creature, plr);
                        //emote
                        m_chargeTargetPos = new Coords(plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ());
                        m_uiChargeStepTimer = 2500;
                        break;
                    }
                    case 2:
                    {
                        PointPath pointPath;
                        pointPath.resize(2);
                        pointPath[0] = m_creature->GetPosition();
                        pointPath[1] = m_creature->GetPosition();
                        
                        float ang = m_creature->GetFixedOrientation() + M_PI_F;
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
                        DoScriptText(SAY_TRAMPLE_START, m_creature);
                        PointPath pointPath;
                        pointPath.resize(2);
                        pointPath.set(0, m_creature->GetPosition());
                        pointPath.set(1, *m_chargeTargetPos);
                        m_creature->ChargeMonsterMove(pointPath, SPLINETYPE_NORMAL, SPLINEFLAG_WALKMODE, 1000);
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
                        {
                            m_creature->CastSpell(m_creature, SPELL_STAGGERED_DAZE, false);
                            DoScriptText(SAY_TRAMPLE_FAIL, m_creature);
                        }
                        m_creature->FixOrientation();
                        setUpdatableForTimers(true);
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
        m_TimerMgr->TimerFinished(TIMER_BUTT);

        // Whirl
        m_TimerMgr->TimerFinished(TIMER_WHIRL);

        // Arctic Breath
        if (SpellTimer* timer = m_TimerMgr->TimerFinished(TIMER_BREATH))
        {
            if (!isCastingBreath)
            {
                if (Unit* target = timer->getTarget())
                    m_creature->FixOrientation(m_creature->GetAngle(target));

                timer->SetValue(TIMER_VALUE_SPELLID, 0);
                timer->Cooldown(TIMER_BREATH, 5000);
                isCastingBreath = true;
            }
            else
            {
                m_creature->FixOrientation();
                isCastingBreath = false;
                timer->Reset(TIMER_VALUE_SPELLID);
                timer->Cooldown(15000);
            }
        }

        // Massive Crash
        if (m_TimerMgr->TimerFinished(TIMER_MASSIVE_CRASH))
        {
            setUpdatableForTimers(false);

            PointPath pointPath;
            pointPath.resize(2);
            SetCombatMovement(false);
            m_creature->GetMotionMaster()->Clear(false, true);
            pointPath[0] = m_creature->GetPosition();
            pointPath[1] = SpawnLoc[LOC_CENTER];
            m_creature->SendMonsterMove(SpawnLoc[LOC_CENTER].x, SpawnLoc[LOC_CENTER].y, SpawnLoc[LOC_CENTER].z, SPLINETYPE_NORMAL, SPLINEFLAG_WALKMODE, 1000);
            m_creature->GetMotionMaster()->MoveCharge(pointPath, 1000.0f, 1, 1);

            m_uiChargeStepTimer = 1000;
            m_uiChargeStepCount = 0;            
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
