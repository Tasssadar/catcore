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
    TIMER_TOUCH,
    TIMER_BERSERK
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
    SPELL_BERSERK               = 64238,
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

struct MANGOS_DLL_DECL boss_twin_valkyrAI : public ScriptedAI
{
    boss_twin_valkyrAI(Creature* pCreature) : ScriptedAI(pCreature)
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

    Creature* sis;

    void Reset()
    {
        //m_TimerMgr->AddTimer(TIMER_ESSENCE, FL_LIGHT_VORTEX, 0, 0,UNIT_SELECT_SELF);
        AddTimer(TIMER_SPIKE, isLight ? SPELL_TWIN_SPIKE_L : SPELL_TWIN_SPIKE_D, 10000, urand(15000,20000), UNIT_SELECT_VICTIM);
        AddTimer(TIMER_SURGE, isLight ? SPELL_SURGE_OF_LIGHT : SPELL_SURGE_OF_DARKNESS, 0, 0, UNIT_SELECT_SELF);
        AddTimer(TIMER_SHIELD, isLight ? SPELL_SHIELD_OF_LIGHTS : SPELL_SHIELD_OF_DARKNESS, urand(40000,50000), urand(40000,50000), UNIT_SELECT_SELF);
        AddTimer(TIMER_HEAL, isLight ? SPELL_TWINS_PACT_L : SPELL_TWINS_PACT_D, 0, 0, UNIT_SELECT_SELF);
        //m_TimerMgr->AddTimer(TIMER_POWER_OF_TWINS, isLight ? SPELL_POWER_OF_THE_TWINS_L : SPELL_POWER_OF_THE_TWINS_D, 0,0,UNIT_SELECT_SELF);
        AddTimer(TIMER_VORTEX, isLight ? SPELL_LIGHT_VORTEX : SPELL_DARK_VORTEX, urand(40000,50000),urand(40000,50000), UNIT_SELECT_SELF);
        AddTimer(TIMER_CONCENTRATED, 0, urand(15000,20000), urand(40000,50000), UNIT_SELECT_NONE, CAST_TYPE_IGNORE);
        if (isHeroic)
            AddTimer(TIMER_TOUCH, isLight ? SPELL_TOUCH_OF_LIGHT : SPELL_TOUCH_OF_DARKNESS, urand(10000,15000), urand(17500,22500), UNIT_SELECT_RANDOM_PLAYER, CAST_TYPE_NONCAST, 1);

        AddTimer(TIMER_BERSERK, SPELL_BERSERK, isHeroic ? 360000: 600000, 60000, UNIT_SELECT_SELF, CAST_TYPE_FORCE);

        m_TimerMgr->AddToCastQueue(TIMER_SURGE);

        sis = NULL;
    }

    Creature* GetSis()
    {
        if (!sis)
            sis = GetClosestCreatureWithEntry(m_creature, isLight ? 34496 : 34497, DEFAULT_VISIBILITY_INSTANCE);

        return sis;
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

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        AttackStart(pWho);
        if (Creature* sis = GetSis())
            sis->AI()->AttackStart(pWho);
    }

    void JustDied(Unit* /*pWho*/)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (sis)
        {
            if (!sis->isAlive())
            { /* complete*/ }
        }
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

        bool specialCasted = false;

        if (m_TimerMgr->TimerFinished(TIMER_SHIELD)) // shield + heal
        {
            m_TimerMgr->AddToCastQueue(TIMER_HEAL);
            DoScriptText(EMOTE_SHIELD, m_creature);
            DoScriptText(SAY_SHIELD, m_creature);
            specialCasted = true;
        }
        else if (m_TimerMgr->TimerFinished(TIMER_VORTEX))// vortex
        {
            specialCasted = true;
            DoScriptText(isLight ? EMOTE_LIGHT_VORTEX : EMOTE_DARK_VORTEX, m_creature);
            DoScriptText(isLight ? SAY_LIGHT_VORTEX : SAY_DARK_VORTEX, m_creature);
        }

        if (specialCasted)
        {
            // need to set cooldown manually, must me different value every time
            m_TimerMgr->Cooldown(TIMER_SHIELD, urand(40000,50000));
            m_TimerMgr->Cooldown(TIMER_VORTEX, urand(40000,50000));
            if (SpellTimerMgr* sisMgr = GetSisTimerMgr())
            {
                sisMgr->Cooldown(TIMER_SHIELD, urand(40000,50000));
                sisMgr->Cooldown(TIMER_VORTEX, urand(40000,50000));
            }
        }

        // Twin Spike
        m_TimerMgr->TimerFinished(TIMER_SPIKE);

        // Touch of Light/Darkness
        m_TimerMgr->TimerFinished(TIMER_TOUCH);

        // summon Concentrated
        if (m_TimerMgr->TimerFinished(TIMER_CONCENTRATED))
        {
            Coords coord = Center;
            float radius = 35.0f;
            const uint8 id[2] = {NPC_CONCENTRATED_DARKNESS, NPC_CONCENTRATED_LIGHT};
            for (float angle = 0; angle < M_PI_F*2; angle += M_PI_F*2/m_uiConcCount[m_dDifficulty])
            {
                float x = coord.x + radius*cos(angle);
                float y = coord.y + radius*sin(angle);
                for (uint8 i = 0; i < 2; ++i)
                    m_creature->SummonCreature(id[i], x, y, coord.z, 0, TEMPSUMMON_MANUAL_DESPAWN, 0);
            }
        }

        // Berserk
        if (m_TimerMgr->TimerFinished(TIMER_BERSERK))
            DoScriptText(SAY_BERSERK, m_creature);
    }
};

#define START_POINT 100
struct MANGOS_DLL_DECL npc_concentratedAI : public ScriptedAI
{
    npc_concentratedAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        isLight = m_creature->GetEntry() == 34630;
        isDark = m_creature->GetEntry() == 34628;
        if (!isLight && !isDark)
            m_creature->ForcedDespawn();
    }

    bool isLight;
    bool isDark;

    bool used;

    //Coords lastCoord;

    void Reset()
    {
        //lastCoord = m_creature->GetPosition();
        used = false;
        Ping(START_POINT);
    }

    void Ping(uint32 pointId)
    {
        float angleToMid = m_creature->GetAngle(Center.x, Center.y);
        float addAngle =  rand_norm_f()*M_PI_F*2/10*pow(-1.f, float(urand(1,2)));
        float angle = angleToMid + addAngle;

        float gama = M_PI_F-2*addAngle;
        float distance = addAngle ? (35*sin(gama))/sin(addAngle) : 70;
        Coords coord = m_creature->GetPosition();
        coord.x += distance*cos(angle);
        coord.y += distance*sin(angle);

        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MovePoint(pointId, coord.x, coord.y, coord.z);
        //m_creature->ChargeMonsterMove(path, SPLINETYPE_NORMAL, m_creature->GetSplineFlags(), time);
    }

    void MovementInform(uint32 moveType, uint32 pointId)
    {
        if (moveType == POINT_MOTION_TYPE && pointId >= START_POINT)
            Ping(pointId+1);
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

    /*void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetDistance(Center.x, Center.y, Center.z) > 33 &&
            m_creature->GetDistance(lastCoord.x, lastCoord.y, lastCoord.z) > 8)
            Ping();
    }*/
};
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
}
