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
SD%Complete: nevim%
SDComment:
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Say
{
    SAY_AGGRO           = -1649040,
    SAY_DEATH           = -1649041,
    SAY_BERSERK         = -1649042,
    EMOTE_SHIELD        = -1649043,
    SAY_SHIELD          = -1649044,
    SAY_KILL1           = -1649045,
    SAY_KILL2           = -1649046,
    EMOTE_LIGHT_VORTEX  = -1649047,
    SAY_LIGHT_VORTEX    = -1649048,
    EMOTE_DARK_VORTEX   = -1649049,
    SAY_DARK_VORTEX     = -1649050
};

enum Equipment
{
    EQUIP_MAIN_1         = 49303,
    EQUIP_OFFHAND_1      = 47146,
    EQUIP_RANGED_1       = 47267,
    EQUIP_MAIN_2         = 45990,
    EQUIP_OFFHAND_2      = 47470,
    EQUIP_RANGED_2       = 47267,
    EQUIP_DONE           = EQUIP_NO_CHANGE
};

enum Timers
{
    TIMER_SPIKE = 0,
    TIMER_SPECIAL,
    TIMER_CONCENTRATED,
    TIMER_TOUCH,
    TIMER_BERSERK

    //TIMER_ESSENCE = 0,
    //TIMER_SURGE,
    //TIMER_SHIELD,
    //TIMER_HEAL,
    //TIMER_POWER_OF_TWINS,
    //TIMER_VORTEX,
};

enum Spells
{
    SPELL_LIGHT_ESSENCE         = 65686,
    SPELL_DARK_ESSENCE          = 65684,
    SPELL_LIGHT_VORTEX          = 66046,
    SPELL_DARK_VORTEX           = 66058,

    SPELL_SHIELD_OF_LIGHTS      = 65858,
    SPELL_TWINS_PACT_L          = 65876,
    SPELL_SHIELD_OF_DARKNESS    = 65874,
    SPELL_TWINS_PACT_D          = 65875,
    SPELL_SURGE_OF_LIGHT        = 65766,
    SPELL_SURGE_OF_DARKNESS     = 65768,
    SPELL_TWIN_SPIKE_L          = 66075,
    SPELL_TWIN_SPIKE_D          = 66069,
    SPELL_BERSERK_VALKYR        = 64238,
    SPELL_EMPOWERED_LIGHT       = 65748,
    SPELL_EMPOWERED_DARK        = 65724,

    SPELL_REMOVE_TOUCH          = 68084,
    SPELL_TOUCH_OF_LIGHT        = 65950,
    SPELL_TOUCH_OF_DARKNESS     = 66001,

    SPELL_POWERING_UP           = 67590,
    SPELL_UNLEASHED_LIGHT       = 65795,
    SPELL_UNLEASHED_DARK        = 65808

    //SPELL_POWER_OF_THE_TWINS_L  = 65916,
    //SPELL_POWER_OF_THE_TWINS_D  = 65879,
    //SPELL_SUMM_CONC             = 66077,
};

const uint8 m_uiConcCount[MAX_DIFFICULTY] = {10, 15, 25, 35};

enum ValkyrSide
{
    SIDE_NONE   = 0,
    SIDE_LIGHT  = 1,
    SIDE_DARK   = 2
};

ValkyrSide GetUnitSide(Unit* unit)
{
    if (unit->HasAura(SPELL_LIGHT_ESSENCE))
        return SIDE_LIGHT;
    else if (unit->HasAura(SPELL_DARK_ESSENCE))
        return SIDE_DARK;
    else
        return SIDE_NONE;
}

ValkyrSide GetUnitEmpowered(Unit* unit)
{
    if (unit->HasAura(SPELL_EMPOWERED_LIGHT))
        return SIDE_LIGHT;
    else if (unit->HasAura(SPELL_EMPOWERED_DARK))
        return SIDE_DARK;
    else
        return SIDE_NONE;
}

struct MANGOS_DLL_DECL boss_twin_valkyrAI : public ScriptedAI
{
    boss_twin_valkyrAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        if (!(m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData()))
            pCreature->ForcedDespawn();

        if (pCreature->GetEntry() == NPC_LIGHTBANE)
            m_side = SIDE_LIGHT;
        else if (pCreature->GetEntry() == NPC_DARKBANE)
            m_side = SIDE_DARK;
        else
            pCreature->ForcedDespawn();

        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        isHeroic = pCreature->GetMap()->IsHeroicRaid();

        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_FLY_ANIM);
        pCreature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
        pCreature->SetSplineFlags(SPLINEFLAG_UNKNOWN7);
        pCreature->SetGroundOffset(4.f);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;
    bool isHeroic;
    ValkyrSide m_side;

    Creature* sis;

    void Reset()
    {
        AddTimer(TIMER_SPIKE, m_side == SIDE_LIGHT ? SPELL_TWIN_SPIKE_L : SPELL_TWIN_SPIKE_D, 10000, RV(15000,20000), UNIT_SELECT_VICTIM);
        AddTimer(TIMER_BERSERK, SPELL_BERSERK_VALKYR, isHeroic ? 360000: 600000, 60000, UNIT_SELECT_SELF, CAST_TYPE_FORCE);
        if (isHeroic)
            AddTimer(TIMER_TOUCH, m_side == SIDE_LIGHT ? SPELL_TOUCH_OF_LIGHT : SPELL_TOUCH_OF_DARKNESS, RV(10000,15000), RV(17500,22500), UNIT_SELECT_RANDOM_PLAYER, CAST_TYPE_NONCAST, 1);

        AddNonCastTimer(TIMER_CONCENTRATED, RV(15000,20000), RV(40000,50000));
        AddNonCastTimer(TIMER_SPECIAL, RV(30000,40000), RV(40000,45000));

        m_TimerMgr->AddSpellToQueue(m_side == SIDE_LIGHT ? SPELL_SURGE_OF_LIGHT : SPELL_SURGE_OF_DARKNESS);

        if (m_side == SIDE_LIGHT)
            SetEquipmentSlots(false, EQUIP_MAIN_1, EQUIP_OFFHAND_1, EQUIP_RANGED_1);
        else
            SetEquipmentSlots(false, EQUIP_MAIN_2, EQUIP_OFFHAND_2, EQUIP_RANGED_2);

        sis = NULL;;
    }

    Creature* GetSis()
    {
        if (!sis)
            sis = GetClosestCreatureWithEntry(m_creature, m_side == SIDE_LIGHT ? NPC_DARKBANE : NPC_LIGHTBANE, DEFAULT_VISIBILITY_INSTANCE);

        return sis;
    }

    void DamageTaken(Unit *pDoneBy, uint32 &uiDamage)
    {
        if (!pDoneBy || pDoneBy->GetTypeId() != TYPEID_PLAYER)
            return;

        // damage increase/decrease
        if (ValkyrSide essSide = GetUnitSide(pDoneBy))
        {
            if (m_side == essSide)
            {
                ValkyrSide empSide = GetUnitEmpowered(pDoneBy);
                if (empSide && m_side != empSide)
                    uiDamage *= 2.f;
                else
                    uiDamage *= 1.5f;
            }
            else
                uiDamage *= 0.5f;
        }

        // health pool sharing
        Creature* sis = GetSis();
        if (!sis || !sis->IsInWorld() || !sis->isAlive())
            return;

        uint32 sisHp = sis->GetHealth();
        if (sisHp > uiDamage)
            sisHp -= uiDamage;
        else
            sisHp = 1;

        sis->SetHealth(sisHp);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        m_pInstance->SetData(TYPE_VALKIRIES, IN_PROGRESS);

        if (!m_bAttackEnabled)
            return;

        AttackStart(pWho);
        if (Creature* sis = GetSis())
            sis->AI()->AttackStart(pWho);
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
            m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void JustDied(Unit* )
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (!sis || !sis->isAlive())
            m_pInstance->SetData(TYPE_VALKIRIES, DONE);
        else
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
    }

    void JustReachedHome()
    {
        m_pInstance->SetData(TYPE_VALKIRIES, FAIL);
        m_creature->ForcedDespawn();
    }

    void KilledUnit(Unit* pWho)
    {
        if (pWho->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // shield + heal or vortex and random creature
        if (m_TimerMgr->GetState(TIMER_SPECIAL))
        {
            m_creature->InterruptNonMeleeSpells(false);

            if (urand(0,1))
            {
                m_TimerMgr->AddSpellCast(m_side == SIDE_LIGHT ? SPELL_SHIELD_OF_LIGHTS : SPELL_SHIELD_OF_DARKNESS);
                m_TimerMgr->AddSpellToQueue(m_side == SIDE_LIGHT ? SPELL_TWINS_PACT_L : SPELL_TWINS_PACT_D);
                DoScriptText(EMOTE_SHIELD, m_creature);
                DoScriptText(SAY_SHIELD, m_creature);
            }
            else
            {
                m_TimerMgr->AddSpellCast(m_side == SIDE_LIGHT ? SPELL_LIGHT_VORTEX : SPELL_DARK_VORTEX);
                DoScriptText(m_side == SIDE_LIGHT ? EMOTE_LIGHT_VORTEX : EMOTE_DARK_VORTEX, m_creature);
                DoScriptText(m_side == SIDE_LIGHT ? SAY_LIGHT_VORTEX : SAY_DARK_VORTEX, m_creature);
            }

            // set cooldown on sis too
            if (Creature* s = GetSis())
                s->GetTimerMgr()->Cooldown(TIMER_SPECIAL);
        }

        // Twin Spike
        m_TimerMgr->GetState(TIMER_SPIKE);

        // Touch of Light/Darkness
        m_TimerMgr->GetState(TIMER_TOUCH);

        // summon Concentrated
        if (m_TimerMgr->GetState(TIMER_CONCENTRATED))
        {
            for (float angle = 0; angle < M_PI_F*2; angle += M_PI_F*2/m_uiConcCount[m_dDifficulty])
            {
                Coords coord = SpawnLoc[LOC_CENTER];
                coord.x += cos(angle)*35.f;
                coord.y += sin(angle)*35.f;
                if (Creature* pConc = m_creature->SummonCreature(m_side == SIDE_LIGHT ? NPC_CONCENTRATED_LIGHT : NPC_CONCENTRATED_DARKNESS, coord, 0, TEMPSUMMON_TIMED_DESPAWN, 60000))
                    pConc->SetRespawnDelay(7*DAY);
            }
        }

        // Berserk
        if (m_TimerMgr->GetState(TIMER_BERSERK))
            DoScriptText(SAY_BERSERK, m_creature);

        DoMeleeAttackIfReady();
    }
};

#define START_POINT 100
struct MANGOS_DLL_DECL npc_concentratedAI : public ScriptedAI
{
    npc_concentratedAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        if (pCreature->GetEntry() == 34630)
            m_side = SIDE_LIGHT;
        else if (pCreature->GetEntry() == 34628)
            m_side = SIDE_DARK;
        else
            pCreature->ForcedDespawn();

        pCreature->RemoveSplineFlag(SPLINEFLAG_WALKMODE);

        finished = false;
        Ping(START_POINT);
    }

    ValkyrSide m_side;
    bool finished;

    void Reset(){}
    void Ping(uint32 pointId)
    {
        float angleToMid = m_creature->GetAngle(SpawnLoc[LOC_CENTER].x, SpawnLoc[LOC_CENTER].y);
        float addAngle =  rand_norm_f()*M_PI_F*2/10*pow(-1.f, float(urand(1,2)));
        float angle = angleToMid + addAngle;

        float gama = M_PI_F-2*addAngle;
        float distance = addAngle ? (35*sin(gama))/sin(addAngle) : 70;
        Coords coord = m_creature->GetPosition();
        coord.x += distance*cos(angle);
        coord.y += distance*sin(angle);

        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MovePoint(pointId, coord.x, coord.y, coord.z);
    }

    void MovementInform(uint32 moveType, uint32 pointId)
    {
        if (moveType == POINT_MOTION_TYPE && pointId >= START_POINT)
            Ping(pointId+1);
    }

    Player* GetNearestPlayerAndDist(float& dist)
    {
        Player* nearest = NULL;
        dist = 999.f;
        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* plr = itr->getSource();
            if (!plr || !plr->isTargetableForAttack())
                continue;

            float currdist = m_creature->GetDistance2d(plr);
            if (dist > currdist)
            {
                nearest = plr;
                dist = currdist;
            }
        }

        return nearest;
    }

    void UpdateAI(const uint32)
    {
        if (finished)
            return;

        float distance;
        Player* plr = GetNearestPlayerAndDist(distance);
        if (!plr || distance > 4.f)
            return;

        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->GetMotionMaster()->MoveIdle();

        if (m_side == GetUnitSide(plr))
            plr->CastSpell(plr, SPELL_POWERING_UP, true);
        else
            m_creature->CastSpell(m_creature, m_side == SIDE_LIGHT ? SPELL_UNLEASHED_LIGHT : SPELL_UNLEASHED_DARK, true);

        finished = true;

        m_creature->ForcedDespawn(500);
    }
};

bool GossipHello_npc_toc_essence(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    if (!m_pInstance || m_pInstance->GetData(TYPE_VALKIRIES) != IN_PROGRESS)
        return true;

    ValkyrSide m_side = SIDE_NONE;
    if (pCreature->GetEntry() == NPC_LIGHT_ESSENCE)
        m_side = SIDE_LIGHT;
    else if (pCreature->GetEntry() == NPC_DARK_ESSENCE)
        m_side = SIDE_DARK;
    else
        return true;

    pPlayer->AuraLink(SPELL_DARK_ESSENCE, m_side == SIDE_DARK);
    pPlayer->AuraLink(SPELL_LIGHT_ESSENCE, m_side == SIDE_LIGHT);
    return true;
}

CreatureAI* GetAI_boss_twin_valkyr(Creature* pCreature)
{
    return new boss_twin_valkyrAI(pCreature);
}

CreatureAI* GetAI_npc_concentrated(Creature* pCreature)
{
    return new npc_concentratedAI(pCreature);
}

void AddSC_twin_valkyr()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_twin_valkyr";
    newscript->GetAI = &GetAI_boss_twin_valkyr;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_concentrated";
    newscript->GetAI = &GetAI_npc_concentrated;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_toc_essence";
    newscript->pGossipHello = &GossipHello_npc_toc_essence;
    newscript->RegisterSelf();
}
