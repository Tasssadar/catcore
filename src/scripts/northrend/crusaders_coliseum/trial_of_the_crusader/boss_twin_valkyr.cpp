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

enum Timers
{
    TIMER_ESSENCE = 0,
    TIMER_SPIKE,
    TIMER_SURGE,
    TIMER_SHIELD,
    TIMER_HEAL,
    TIMER_POWER_OF_TWINS,
    TIMER_VORTEX,
    TIMER_CONCENTRATED,
    TIMER_TOUCH
};

enum Spells
{
    SPELL_LIGHT_ESSENCE         = 65686,
    SPELL_DARK_ESSENCE          = 65684,
    SPELL_LIGHT_VORTEX          = 66046,
    SPELL_DARK_VORTEX           = 66058,
    //SPELL_POWER_OF_THE_TWINS_L  = 65916,
    //SPELL_POWER_OF_THE_TWINS_D  = 65879,
    SPELL_SHIELD_OF_LIGHTS      = 65858,
    SPELL_TWINS_PACT_L          = 65876,
    SPELL_SHIELD_OF_DARKNESS    = 65874,
    SPELL_TWINS_PACT_D          = 65875,
    SPELL_SURGE_OF_LIGHT        = 65766,
    SPELL_SURGE_OF_DARKNESS     = 65768,
    SPELL_TWIN_SPIKE_L          = 66075,
    SPELL_TWIN_SPIKE_D          = 66069,
    //SPELL_SUMM_CONC             = 66077,

    SPELL_REMOVE_TOUCH          = 68084,
    SPELL_TOUCH_OF_LIGHT        = 65950,
    SPELL_TOUCH_OF_DARKNESS     = 66001,

    NPC_DARK_ESSENCE            = 34567,
    NPC_LIGHT_ESSENCE           = 34568,

    NPC_CONCENTRATED_DARKNESS   = 34628,
    NPC_CONCENTRATED_LIGHT      = 34630,

    SPELL_POWERING_UP           = 67590,
    SPELL_UNLEASHED_LIGHT       = 65795,
    SPELL_UNLEASHED_DARK        = 65808
};

const uint8 m_uiConcCount[MAX_DIFFICULTY] = {10, 15, 25, 35};

struct MANGOS_DLL_DECL boss_twin_valkyr : public ScriptedAI
{
    boss_twin_valkyr(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_dDifficulty = pCreature->GetMap()->GetDifficulty();
        isLight = m_creature->GetEntry() == 34497;
        isDark = m_creature->GetEntry() == 34496;
        if (!isLight && !isDark)
            pCreature->ForcedDespawn();
        isHeroic = pCreature->GetMap()->IsHeroicRaid();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_dDifficulty;
    bool isHeroic;

    bool isLight;
    bool isDark;

    void Reset()
    {
        //m_TimerMgr->AddTimer(TIMER_ESSENCE, FL_LIGHT_VORTEX, 0, 0,UNIT_SELECT_SELF);
        m_TimerMgr->AddTimer(TIMER_SPIKE, isLight ? SPELL_TWIN_SPIKE_L : SPELL_TWIN_SPIKE_D, 10000, urand(15000,20000), UNIT_SELECT_VICTIM);
        m_TimerMgr->AddTimer(TIMER_SURGE, isLight ? SPELL_SURGE_OF_LIGHT : SPELL_SURGE_OF_DARKNESS, 0, 0, UNIT_SELECT_SELF);
        m_TimerMgr->AddTimer(TIMER_SHIELD, isLight ? SPELL_SHIELD_OF_LIGHTS : SPELL_SHIELD_OF_DARKNESS, urand(40000,50000), urand(40000,50000), UNIT_SELECT_SELF);
        m_TimerMgr->AddTimer(TIMER_HEAL, isLight ? SPELL_TWINS_PACT_L : SPELL_TWINS_PACT_D, 0, 0, UNIT_SELECT_SELF);
        //m_TimerMgr->AddTimer(TIMER_POWER_OF_TWINS, isLight ? SPELL_POWER_OF_THE_TWINS_L : SPELL_POWER_OF_THE_TWINS_D, 0,0,UNIT_SELECT_SELF);
        m_TimerMgr->AddTimer(TIMER_VORTEX, isLight ? SPELL_LIGHT_VORTEX : SPELL_DARK_VORTEX, urand(40000,50000),urand(40000,50000), UNIT_SELECT_SELF);
        m_TimerMgr->AddTimer(TIMER_CONCENTRATED, 0, urand(15000,20000), urand(40000,50000), UNIT_SELECT_NONE, CAST_TYPE_IGNORE);
        if (isHeroic)
            m_TimerMgr->AddTimer(TIMER_TOUCH, isLight ? SPELL_TOUCH_OF_LIGHT : SPELL_TOUCH_OF_DARKNESS, urand(10000,15000), urand(17500,22500), UNIT_SELECT_RANDOM_PLAYER, CAST_TYPE_NONCAST, 1);

        m_TimerMgr->AddToCastQueue(TIMER_SURGE);
    }

    Creature* GetSis()
    {
        return GetClosestCreatureWithEntry(m_creature, isLight ? 34496 : 34497, DEFAULT_VISIBILITY_INSTANCE);
    }

    SpellTimerMgr* GetSisTimerMgr()
    {
        return GetSis() ? GetSis()->GetTimerMgr() : NULL;
    }

    void DamageTaken(Unit */*pDoneBy*/, uint32 &uiDamage)
    {
        Creature* sis = GetSis();
        sis->SetHealth(sis->GetHealth()-uiDamage);
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        bool specialCasted = false;

        // shield + heal
        if (m_TimerMgr->TimerFinished(TIMER_SHIELD))
        {
            m_TimerMgr->AddToCastQueue(TIMER_HEAL);
            specialCasted = true;
        }

        // vortex
        if (m_TimerMgr->TimerFinished(TIMER_VORTEX))
            specialCasted = true;

        if (specialCasted)
        {
            m_TimerMgr->Cooldown(TIMER_SHIELD);
            m_TimerMgr->Cooldown(TIMER_VORTEX);
            if (SpellTimerMgr* sisMgr = GetSisTimerMgr())
            {
                sisMgr->Cooldown(TIMER_SHIELD);
                sisMgr->Cooldown(TIMER_VORTEX);
            }
        }

        // Twin Spike
        m_TimerMgr->TimerFinished(TIMER_SPIKE);

        // Touch of Light/Darkness
        m_TimerMgr->TimerFinished(TIMER_TOUCH);

        // summon Concentrated
        if (m_TimerMgr->TimerFinished(TIMER_CONCENTRATED))
        {
            WorldLocation loc(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 0);
            float radius = 35.0f;
            const uint8 id[2] = {NPC_CONCENTRATED_DARKNESS, NPC_CONCENTRATED_LIGHT};
            for (float angle = 0; angle < M_PI_F*2; angle += M_PI_F*2/m_uiConcCount[m_dDifficulty])
            {
                float x = loc.coord_x + radius*cos(angle);
                float y = loc.coord_y + radius*sin(angle);
                for (uint8 i = 0; i < 2; ++i)
                    m_creature->SummonCreature(id[i], x, y, loc.coord_z, loc.orientation, TEMPSUMMON_MANUAL_DESPAWN, 0);
            }
        }
    }
};

struct MANGOS_DLL_DECL npc_concentrated : public ScriptedAI
{
    npc_concentrated(Creature* pCreature) : ScriptedAI(pCreature)
    {
        isLight = m_creature->GetEntry() == 34630;
        isDark = m_creature->GetEntry() == 34628;
        if (!isLight && !isDark)
            m_creature->ForcedDespawn();
    }

    bool isLight;
    bool isDark;

    bool used;

    WorldLocation lastLoc;
    WorldLocation center;

    void Reset()
    {
        lastLoc = m_creature->GetLocation();
        center = WorldLocation(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 0);
        used = false;
        Ping();
    }

    void Ping()
    {
        float angleToMid = m_creature->GetAngle(center.coord_x, center.coord_y);
        float addAngle =  rand_norm_f()*M_PI_F*2/10*pow(-1.f, float(urand(1,2)));
        float angle = angleToMid + addAngle;

        float gama = M_PI_F-2*addAngle;
        float distance = addAngle ? (35*sin(gama))/sin(addAngle) : 70;
        float x,y,z;
        m_creature->GetPosition(x,y,z);
        x += distance*cos(angle);
        y += distance*sin(angle);
        uint32 time = (distance/70)*10000;

        PointPath path;
        path.resize(2);
        path.set(0, PathNode(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()));
        path.set(1, PathNode(x, y, z));

        m_creature->GetMotionMaster()->Clear();
        m_creature->ChargeMonsterMove(path, SPLINETYPE_NORMAL, m_creature->GetSplineFlags(), time);
    }

    void MoveInLineOfSight(Unit * pWho)
    {
        if (!pWho || used)
            return;

        if (pWho->GetTypeId() == TYPEID_PLAYER && pWho->isTargetableForAttack() && pWho->IsWithinDist(m_creature, 2))
        {
            if (isLight)
            {
                if (pWho->HasAuraOnDifficulty(SPELL_LIGHT_ESSENCE))
                    pWho->CastSpell(pWho, SPELL_POWERING_UP, true);
                else
                    m_creature->CastSpell(m_creature, SPELL_UNLEASHED_LIGHT, true);
            }
            else
            {
                if (pWho->HasAuraOnDifficulty(SPELL_DARK_ESSENCE))
                    pWho->CastSpell(pWho, SPELL_POWERING_UP, true);
                else
                    m_creature->CastSpell(m_creature, SPELL_UNLEASHED_DARK, true);
            }
            used = true;
            m_creature->ForcedDespawn(500);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetDistance(center.coord_x, center.coord_y, center.coord_z) > 33 &&
            m_creature->GetDistance(lastLoc.coord_x, lastLoc.coord_y, lastLoc.coord_z) > 8)
            Ping();
    }
};

void AddSC_twin_valkyr()
{
    Script* newscript;

    /*newscript = new Script;
    newscript->Name = "boss_fjola";
    newscript->GetAI = &GetAI_boss_fjola;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_eydis";
    newscript->GetAI = &GetAI_boss_eydis;
    newscript->RegisterSelf();*/

}
/*
script_target
-- light
INSERT INTO spell_script_target VALUES (65876, 1, 34496);
INSERT INTO spell_script_target VALUES (67306, 1, 34496);
INSERT INTO spell_script_target VALUES (67307, 1, 34496);
INSERT INTO spell_script_target VALUES (67308, 1, 34496);
-- dark
INSERT INTO spell_script_target VALUES (65875, 1, 34497);
INSERT INTO spell_script_target VALUES (67303, 1, 34497);
INSERT INTO spell_script_target VALUES (67304, 1, 34497);
INSERT INTO spell_script_target VALUES (67305, 1, 34497);
*/
