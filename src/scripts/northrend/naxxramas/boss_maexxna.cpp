/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Boss_Maexxna
SD%Complete: 100
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    SPELL_WEBWRAP           = 28622,
    NPC_SPIDERLING          = 17055
};

enum Timer
{
    TIMER_WEB_WRAP = 0,
    TIMER_WEB_SPRAY,
    TIMER_POISON_SHOCK,
    TIMER_NECROTIC_POISON,
    TIMER_SPIDER_SPAWN
};

const int m_idWebSpray[] = {29484, 54125};
const int m_idPoisonShock[] = {28741, 54122};
const int m_idNecroticPoison[] = {28776, 54121};
const int m_idFrenzy[] = {54123, 54124};

const Coords WebWrapLoc[] =
{
    Coords(3546.79f, -3869.08f, 297.45f),
    Coords(3531.27f, -3847.42f, 300.45f),
    Coords(3497.07f, -3843.38f, 303.38f)
};

struct MANGOS_DLL_DECL boss_maexxnaAI : public ScriptedAI
{
    boss_maexxnaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Is25Man = pCreature->GetMap()->IsRaid25Man();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool Is25Man;
    bool m_bIsEnraged;

    void Reset()
    {
        AddNonCastTimer(TIMER_WEB_WRAP, RV(15000,2000), RV(30000, 40000));
        AddTimer(TIMER_WEB_SPRAY, m_idWebSpray[Is25Man], 40000, 40000, UNIT_SELECT_SELF, CAST_TYPE_FORCE);
        AddTimer(TIMER_POISON_SHOCK, m_idPoisonShock[Is25Man], 20000, 20000, UNIT_SELECT_VICTIM);
        AddTimer(TIMER_NECROTIC_POISON, m_idNecroticPoison[Is25Man], 30000, 30000, UNIT_SELECT_VICTIM);
        AddNonCastTimer(TIMER_SPIDER_SPAWN, RV(25000, 30000), RV(30000,40000));
        m_bIsEnraged = false;
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAEXXNA, IN_PROGRESS);

        m_creature->SetInCombatWithZone();
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAEXXNA, DONE);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MAEXXNA, FAIL);
    }

    void DamageTaken(Unit */*pDoneBy*/, uint32 &uiDamage)
    {
        if (!m_bIsEnraged && m_creature->GetHealth()-uiDamage < m_creature->GetMaxHealth()*0.3f)
        {
            DoCast(m_creature, m_idFrenzy[Is25Man]);
            m_bIsEnraged = true;
        }
    }

    void UpdateAI(const uint32 /*uiDiff*/)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Web Wrap
        if (m_TimerMgr->GetState(TIMER_WEB_WRAP))
        {
            PlrList list = GetRandomPlayers(Is25Man ? 2 : 1, false);
            for(PlrList::iterator itr = list.begin(); itr != list.end(); ++itr)
            {
                Player* plr = *itr;
                if (!plr) continue;         // this should never happen
                const Coords& warpLoc = WebWrapLoc[rand()%3];
                DoTeleportPlayer(plr, warpLoc.x, warpLoc.y, warpLoc.z, plr->GetOrientation());
                if (Creature* pWrap = m_creature->SummonCreature(16486, warpLoc.x, warpLoc.y, warpLoc.y, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000))
                {
                    plr->CastSpell(plr, SPELL_WEBWRAP, true, NULL, NULL, pWrap->GetObjectGuid());
                    plr->AddThreat(pWrap, 99999.f);
                    pWrap->AddThreat(plr, 99999.f);
                }
            }
        }

        // Web Spray
        m_TimerMgr->GetState(TIMER_WEB_SPRAY);

        // Poison Shock
        m_TimerMgr->GetState(TIMER_POISON_SHOCK);

        // Necrotic Poison
        m_TimerMgr->GetState(TIMER_NECROTIC_POISON);

        // Summon Spiders
        if (m_TimerMgr->GetState(TIMER_SPIDER_SPAWN))
        {
            uint8 SpiderCount = urand(8,10);
            for(uint8 i = 0; i < SpiderCount; ++i)
                if (Creature* pSpider = m_creature->SummonCreature(NPC_SPIDERLING, m_creature->GetPosition(), 0, TEMPSUMMON_DEAD_DESPAWN, 0))
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        pSpider->AI()->AttackStart(pTarget);
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_webwrapAI : public ScriptedAI
{
    mob_webwrapAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }
    void Reset() {}
    void AttackStart(Unit* pWho) {}
    void DamageTaken(Unit* /*pDoneBy*/, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth())
            if (Unit* pTemp = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                pTemp->RemoveAurasDueToSpell(SPELL_WEBWRAP);
    }
    void JustDied(Unit* /*pKiller*/)
    {
        if (Unit* pTemp = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
            pTemp->RemoveAurasDueToSpell(SPELL_WEBWRAP);
    }
};

CreatureAI* GetAI_boss_maexxna(Creature* pCreature)
{
    return new boss_maexxnaAI(pCreature);
}

CreatureAI* GetAI_mob_webwrap(Creature* pCreature)
{
    return new mob_webwrapAI(pCreature);
}

void AddSC_boss_maexxna()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_maexxna";
    NewScript->GetAI = &GetAI_boss_maexxna;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_webwrap";
    NewScript->GetAI = &GetAI_mob_webwrap;
    NewScript->RegisterSelf();
}
