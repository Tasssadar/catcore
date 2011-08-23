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
SDName: boss_anubarak_trial
SD%Complete: 70%
SDComment: by /dev/rsa
SDCategory:
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Timers
{
    // boss
    TIMER_SLASH = 0,
    TIMER_COLD,
    TIMER_PHASE,
    TIMER_BURROWER_SPAWN,
    TIMER_BURROWER_STRIKE,
    TIMER_FROST_SPHERE,

    // spike
    TIMER_SPIKES,

    // burrower
    TIMER_SUBMERGE,

    // scarab
    TIMER_DETERMINATION
};

enum Spells
{
    // anubarak
    BOSS_ANUBARAK           = 34564,
    SPELL_FREEZING_SLASH    = 66012, // phase 1,3 ---- just timer, target victim
    SPELL_PENETRATING_COLD  = 66013, // phase 1,3 ---- handled in core, just timer, target self
    SPELL_LEECHING_SWARM    = 66118, // phase 3 -- just timer, casted on self, queued on start of phase 3
    //66170 -- effect teleport, implicit script, implicit to spike npc, used to keep up boss with spikes (atleast i hope so)
    //SPELL_LEECHING_HEAL     = 66125,
    //SPELL_LEECHING_DAMAGE   = 66240,

    SPELL_PURSUING_SPIKES_TEL=66170,

    SPELL_ROLLING_THROW     = 67730,
    SPELL_ROLLING_THROW_VEH = 67731,
    SPELL_SUBMERGE_BOSS     = 65981,
    //SPELL_EMERGE_BOSS       = 65982,

    SPELL_BERSERK           = 26662,

    // npc
    // frost sphere
    NPC_FROST_SPHERE        = 34606,

    SPELL_FROST_SPHERE      = 67539,
    SPELL_PERMAFROST        = 66193,
    SPELL_PERMAFROST_VISUAL = 65882,

    // burrower
    NPC_BURROWER            = 34607,

    SPELL_NERUBIAN_BURROWER = 66332, // phase 1,2, hc + phase 3
    SPELL_NERUB_BURR_PROC   = 66333,

    //SPELL_EXPOSE_WEAKNESS   = 67721,
    SPELL_EXPOSER_WEAKNESS_A= 67720,
    //SPELL_SPIDER_FRENZY     = 66129,
    SPELL_SPIDER_FRENZY_AURA= 66128,

    SPELL_SUBMERGE_BURR_D   = 67322,
    //SPELL_SUBMERGE_BURR_S   = 66845,
    //SPELL_EMERGE_BURR       = 65982,

    SPELL_SHADOW_STRIKE     = 66134, // hero 10, hero 25
    SPELL_SUMMON_PLAYER     = 21150, // hero 25

    // scarab
    NPC_SCARAB              = 34605,

    SPELL_SUMMON_SCARAB     = 66339, // phase 2
    SPELL_SUMMON_SCARAB_PROC= 66340,

    //SPELL_ACID_MANDIBLE     = 65775,
    SPELL_ACID_MANIBLE_AURA = 65774,
    SPELL_DETERMINATION     = 66092, // enrage
    SPELL_SCARAB_ACHI_10    = 68186, // 10 man
    SPELL_SCARAB_ACHI_25    = 68515, // 25 man

    // spike
    NPC_SPIKE               = 34660,
    SPELL_SPIKE_CALL        = 66169,

    SPELL_PURSUED           = 67574, // phase 2 --- casted by "Anubarak 2", handled in burrow
    SPELL_PURSUING_SPIKES_1 = 65920, // phase 2 --- casted by "Anubarak 2", handled in burrow
    SPELL_PURSUING_SPIKES_2 = 65922,
    SPELL_PURSUING_SPIKES_3 = 65923,
    SPELL_PURSUING_SPIKES_D = 65921,
    SPELL_PURSUING_SPIKES_F = 66181

};

const WorldLocation BurrowerLoc[]=
{
    WorldLocation(0, 694.886353f, 102.484665f, 142.119614f),
    WorldLocation(0, 694.500671f, 185.363968f, 142.117905f),
    WorldLocation(0, 731.987244f, 83.3824690f, 142.119614f),
    WorldLocation(0, 740.184509f, 193.443390f, 142.117584f),
};

const WorldLocation SphereLoc[] =
{
    WorldLocation(0, 786.6439f, 108.2498f, 155.6701f),
    WorldLocation(0, 806.8429f, 150.5902f, 155.6701f),
    WorldLocation(0, 759.1386f, 163.9654f, 155.6701f),
    WorldLocation(0, 744.3701f, 119.5211f, 155.6701f),
    WorldLocation(0, 710.0211f, 120.8152f, 155.6701f),
    WorldLocation(0, 706.6383f, 161.5266f, 155.6701f),
};

struct MANGOS_DLL_DECL boss_anubarak_toc : public ScriptedAI
{
    boss_anubarak_toc(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        isHC = pCreature->GetMap()->IsHeroicRaid();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;
    bool isHC;

    uint8 currentPhase;

    void Reset()
    {
        currentPhase = 0;

        AddTimer(TIMER_SLASH, SPELL_FREEZING_SLASH, urand(0,15000), 15000, UNIT_SELECT_VICTIM);
        AddTimer(TIMER_COLD, SPELL_PENETRATING_COLD, urand(0,20000), 20000, UNIT_SELECT_SELF);
        AddNonCastTimer(TIMER_PHASE, 80000, 60000);
        AddNonCastTimer(TIMER_BURROWER_SPAWN, urand(5000,15000), urand(80000,90000));

        if (isHC)
            AddNonCastTimer(TIMER_BURROWER_STRIKE, urand(25000,30000), 30000);
        else
            AddNonCastTimer(TIMER_FROST_SPHERE, 30000, 90000);

        DespawnAllWithEntry(NPC_BURROWER, TYPEID_UNIT);
        DespawnAllWithEntry(NPC_SCARAB, TYPEID_UNIT);
        DespawnAllWithEntry(NPC_FROST_SPHERE, TYPEID_UNIT);
        DespawnAllWithEntry(NPC_SPIKE, TYPEID_UNIT);
    }

    void Aggro(Unit* pWho)
    {
        if (!pWho)
            return;

        currentPhase = 1;

        for(uint8 i = 0; i < 6; ++i)
            m_creature->SummonCreature(NPC_FROST_SPHERE, SphereLoc[i], TEMPSUMMON_MANUAL_DESPAWN, 0);
    }

    void AttackStart(Unit* pWho)
    {
        if (currentPhase == 2)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (currentPhase == 2)
        {
            // am i sure ?
            // yes i am
            uiDamage = 0;
        }
        else if (currentPhase == 1 &&
                 m_creature->GetHealth()-uiDamage < m_creature->GetMaxHealth()*0.3f)
        {
            SwitchPhase(true);
        }
    }

    void SwitchPhase(bool three = false)
    {
        if (three)
            currentPhase = 3;

        // this shout switch bethween phases 1 and 2 preety easily
        currentPhase = currentPhase%2+1;

        // custom handlers for switch to concrete phase
        switch(currentPhase)
        {
            case 1:
            {
                // timers for phase 1
                m_creature->AddAndLinkAura(SPELL_SUBMERGE_BOSS, false);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                m_TimerMgr->Cooldown(TIMER_PHASE, 80000);
                m_TimerMgr->SetValue(TIMER_SLASH, TIMER_VALUE_UPDATEABLE, true);
                m_TimerMgr->SetValue(TIMER_COLD, TIMER_VALUE_UPDATEABLE, true);
                m_TimerMgr->SetValue(TIMER_BURROWER_STRIKE, TIMER_VALUE_UPDATEABLE, true);
                break;
            }
            case 2:
            {
                // timers for phase 2
                m_creature->AddAndLinkAura(SPELL_SUBMERGE_BOSS, true);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                m_TimerMgr->Cooldown(TIMER_PHASE, 60000);
                m_TimerMgr->AddSpellToQueue(SPELL_SUMMON_SCARAB, UNIT_SELECT_SELF);
                m_TimerMgr->SetValue(TIMER_SLASH, TIMER_VALUE_UPDATEABLE, false);
                m_TimerMgr->SetValue(TIMER_COLD, TIMER_VALUE_UPDATEABLE, false);
                m_TimerMgr->SetValue(TIMER_BURROWER_STRIKE, TIMER_VALUE_UPDATEABLE, false);
                break;
            }
            case 3:
            {
                // timers for phase 3
                // add to queue leeching swarm
                if (!isHC)
                    m_TimerMgr->SetValue(TIMER_BURROWER_SPAWN, TIMER_VALUE_UPDATEABLE, false);

                m_TimerMgr->AddSpellToQueue(SPELL_LEECHING_SWARM, UNIT_SELECT_SELF);
                break;
            }
            default:
                return;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // phase switcher
        if (m_TimerMgr->TimerFinished(TIMER_PHASE))
        {
            SwitchPhase();
            return;
        }

        // Freezing Slash
        m_TimerMgr->TimerFinished(TIMER_SLASH);

        // Penetrating Cold
        m_TimerMgr->TimerFinished(TIMER_COLD);

        // Burrower spawn
        if (m_TimerMgr->TimerFinished(TIMER_BURROWER_SPAWN))
        {
            uint8 index[] = {0, 1, 2, 3};
            uint8 count = 0;
            switch(m_dDifficulty)
            {
                case RAID_DIFFICULTY_10MAN_NORMAL:
                    index[0] = urand(0,3);
                    count = 1;
                    break;
                case RAID_DIFFICULTY_25MAN_NORMAL:
                case RAID_DIFFICULTY_10MAN_HEROIC:
                    index[0] = urand(0,3);
                    index[1] = index[0]+urand(1,3);
                    count = 2;
                    break;
                case RAID_DIFFICULTY_25MAN_HEROIC:
                    count = 4;
                    break;
                default:
                    break;
            }

            for(uint8 i = 0; i < count; ++i)
            {
                if (Creature* crt = m_creature->SummonCreature(NPC_BURROWER, BurrowerLoc[index[i]], TEMPSUMMON_CORPSE_DESPAWN, 0))
                {
                    if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1))
                        crt->AI()->AttackStart(target);
                }
            }
        }

        // Burrower Strike
        if (m_TimerMgr->TimerFinished(TIMER_BURROWER_STRIKE))
        {
            CreatureList list;
            GetCreatureListWithEntryInGrid(list, m_creature, NPC_BURROWER, DEFAULT_VISIBILITY_INSTANCE);
            for (CreatureList::iterator itr = list.begin(); itr != list.end(); ++itr)
                if (Unit* target = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1))
                    (*itr)->CastSpell(target, SPELL_SHADOW_STRIKE, false);
        }

        // Frost Sphere spawn
        if (m_TimerMgr->TimerFinished(TIMER_FROST_SPHERE))
        {
            m_creature->SummonCreature(NPC_FROST_SPHERE, SphereLoc[urand(0,5)], TEMPSUMMON_MANUAL_DESPAWN, 0);
        }
    }
};

struct MANGOS_DLL_DECL mob_burrowerAI : public ScriptedAI
{
    mob_burrowerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool isSubmerged;

    void Reset()
    {
        // submerge handling
        AddNonCastTimer(TIMER_SUBMERGE, 2000, 2000);

        m_TimerMgr->AddSpellToQueue(SPELL_SPIDER_FRENZY_AURA, UNIT_SELECT_SELF);
        m_TimerMgr->AddSpellToQueue(SPELL_EXPOSER_WEAKNESS_A, UNIT_SELECT_SELF);
        isSubmerged = false;
    }

    void AttackStart(Unit * pWho)
    {
        if (isSubmerged)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // submerge handling
        if (m_TimerMgr->TimerFinished(TIMER_SUBMERGE))
        {
            if (isSubmerged)
            {
                uint32 healthToAdd = 5 * m_creature->GetMaxHealth()/100;
                if (m_creature->GetHealth()+healthToAdd > m_creature->GetMaxHealth())
                {
                    m_creature->SetHealth(m_creature->GetMaxHealth());
                    isSubmerged = false;
                    m_creature->AddAndLinkAura(SPELL_SUBMERGE_BURR_D, isSubmerged);
                }
                m_creature->SetHealth(m_creature->GetHealth()+healthToAdd);
            }
            else
            {
                if ( m_creature->GetHealthPercent() < 75 &&
                    !m_creature->HasAura(SPELL_PERMAFROST))
                {
                    isSubmerged = true;
                    m_creature->AddAndLinkAura(SPELL_SUBMERGE_BURR_D, isSubmerged);
                }
            }
        }
    }
};

struct MANGOS_DLL_DECL mob_scarab_tocAI : public ScriptedAI
{
    mob_scarab_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        m_TimerMgr->AddSpellToQueue(SPELL_ACID_MANIBLE_AURA, UNIT_SELECT_SELF);
        AddTimer(TIMER_DETERMINATION, SPELL_DETERMINATION, urand(10000,45000), urand(10000,45000), UNIT_SELECT_SELF, CAST_TYPE_FORCE);

        if (Creature* boss = GetClosestCreatureWithEntry(m_creature, BOSS_ANUBARAK, DEFAULT_VISIBILITY_INSTANCE))
        {
            if (Unit* target = boss->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1))
            {
                AttackStart(target);
                m_creature->AddThreat(target, 20000.f);
            }
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Determination
        m_TimerMgr->TimerFinished(TIMER_DETERMINATION);
    }
};

#define POINT_OF_MOVE 123

struct MANGOS_DLL_DECL mob_frost_sphereAI : public ScriptedAI
{
    mob_frost_sphereAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool isDead;

    void Reset()
    {
        isDead = false;
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
        m_creature->SetSplineFlags(SPLINEFLAG_FLYING);
        m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_CAN_FLY);
        m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
        m_creature->SetSpeedRate(MOVE_FLIGHT, 1.f, true);
        m_creature->SetDisplayId(25144);
        m_creature->SetSpeedRate(MOVE_RUN, 0.5, false);
        m_creature->GetMotionMaster()->MoveRandom();
        DoCast(m_creature, SPELL_FROST_SPHERE);
    }

    void AttackStart(Unit *){}
    void DamageTaken(Unit* /*pWho*/, uint32& uiDamage)
    {
        if (isDead)
        {
            uiDamage = 0;
            return;
        }

        if (m_creature->GetHealth() < uiDamage)
        {
            uiDamage = 0;
            isDead = true;

            m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
            m_creature->RemoveSplineFlag(SPLINEFLAG_FLYING);
            m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_CAN_FLY);
            m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
            m_creature->GetMotionMaster()->Clear();
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            //At hit the ground
            WorldLocation loc = m_creature->GetLocation();
            loc.coord_z = m_creature->GetTerrain()->GetHeight(loc.coord_x, loc.coord_y, 130.f, true, 30.f);
            m_creature->GetMotionMaster()->MovePoint(POINT_OF_MOVE, loc.coord_x, loc.coord_y, loc.coord_z);
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_OF_MOVE)
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveIdle();
            m_creature->RemoveAurasDueToSpell(SPELL_FROST_SPHERE);
            m_creature->SetDisplayId(11686);
            m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.0f);
            m_TimerMgr->AddSpellToQueue(SPELL_PERMAFROST_VISUAL, UNIT_SELECT_SELF);
            m_TimerMgr->AddSpellToQueue(SPELL_PERMAFROST, UNIT_SELECT_SELF);
        }
    }
};

struct MANGOS_DLL_DECL mob_anubarak_spikeAI : public ScriptedAI
{
    mob_anubarak_spikeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint8 speedLevel;
    uint32 timeOnTarget;

    void Reset()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
        speedLevel = 0;
        timeOnTarget = 0;
        m_creature->DeleteThreatList();
        m_creature->GetMotionMaster()->Clear();

        AddTimer(TIMER_SPIKES, pursuingId(), 1000, 3000, UNIT_SELECT_SELF, CAST_TYPE_FORCE);

        if (Creature* anub = GetClosestCreatureWithEntry(m_creature, BOSS_ANUBARAK, DEFAULT_VISIBILITY_INSTANCE))
            anub->CastSpell(m_creature, SPELL_PURSUING_SPIKES_TEL, false);
    }

    uint32 pursuingId()
    {
        switch(speedLevel)
        {
            case 0: return SPELL_PURSUING_SPIKES_D;
            case 1: return SPELL_PURSUING_SPIKES_1;
            case 2: return SPELL_PURSUING_SPIKES_2;
            case 3: return SPELL_PURSUING_SPIKES_3;
            default:return SPELL_PURSUING_SPIKES_3;
        }
    }

    void SetTarget(Unit* pWho)
    {
        DoCast(pWho, SPELL_PURSUED);
        m_creature->SetSpeedRate(MOVE_RUN, 0.5f);
        AttackStart(pWho);
    }

    void DamageTaken(Unit* /*pWho*/, uint32& uiDamage)
    {
        uiDamage = 0;
    }

    /*void SpellHitTarget(Unit* pWho, const SpellEntry *spellInfo)
    {
        if (spellInfo->Id == SPELL_PURSUED)
            SetTarget(pWho);
    }*/

    void CastFinished(const SpellEntry *spellInfo)
    {
        if (spellInfo->Id == SPELL_PURSUING_SPIKES_F &&
            timeOnTarget > 4000)
        {
            if (Creature* crt = GetClosestCreatureWithEntry(m_creature, NPC_FROST_SPHERE, 20.f))
                crt->ForcedDespawn();

            Reset();
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        m_creature->AddThreat(pWho, 99999.f);
        m_creature->SetInCombatWith(pWho);
        pWho->SetInCombatWith(m_creature);
        m_creature->GetMotionMaster()->MoveChase(pWho);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_TimerMgr->TimerFinished(TIMER_SPIKES))
        {
            ++speedLevel;

            if (SpellTimer* tSpikes = m_TimerMgr->GetTimer(TIMER_SPIKES))
            {
                if (tSpikes->GetValue(TIMER_VALUE_SPELLID) == SPELL_PURSUING_SPIKES_D)
                {
                    if (Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0))
                        SetTarget(plr);
                }
                else
                    tSpikes->Cooldown(7000, true);

                tSpikes->SetValue(TIMER_VALUE_SPELLID, pursuingId());
            }
        }

        timeOnTarget += uiDiff;
    }
};

void AddSC_boss_anubarak_trial()
{
    Script* newscript;

}
