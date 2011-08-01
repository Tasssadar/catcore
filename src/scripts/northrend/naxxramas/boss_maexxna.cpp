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
SD%Complete: 60
SDComment: this needs review, and rewrite of the webwrap ability
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

enum
{
    SPELL_WEBWRAP           = 28622,                        //Spell is normally used by the webtrap on the wall NOT by Maexxna

    SPELL_WEBSPRAY          = 29484,
    H_SPELL_WEBSPRAY        = 54125,
    SPELL_POISONSHOCK       = 28741,
    H_SPELL_POISONSHOCK     = 54122,
    SPELL_NECROTICPOISON    = 28776,
    H_SPELL_NECROTICPOISON  = 54121,
    SPELL_FRENZY            = 54123,
    H_SPELL_FRENZY          = 54124,

    //spellId invalid
    SPELL_SUMMON_SPIDERLING = 29434,
    NPC_SPIDERLING          = 17055
};

#define LOC_X1    3546.796f
#define LOC_Y1    -3869.082f
#define LOC_Z1    296.450f

#define LOC_X2    3531.271f
#define LOC_Y2    -3847.424f
#define LOC_Z2    299.450f

#define LOC_X3    3497.067f
#define LOC_Y3    -3843.384f
#define LOC_Z3    302.384f

struct MANGOS_DLL_DECL mob_webwrapAI : public ScriptedAI
{
    mob_webwrapAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}

    void AttackStart(Unit* pWho) {}
    void AuraDrop()
    {
        ThreatList const& list = m_creature->getThreatManager().getPlayerThreatList();
        for(ThreatList::const_iterator itr = list.begin(); itr != list.end(); ++itr)
            if (Unit* pVictim = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid()))
                if (pVictim->HasAura(SPELL_WEBWRAP))
                    pVictim->RemoveAurasDueToSpell(SPELL_WEBWRAP);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth())
            AuraDrop();
    }

    void JustDied(Unit* Killer)
    {
        AuraDrop();
    }
};

struct MANGOS_DLL_DECL boss_maexxnaAI : public ScriptedAI
{
    boss_maexxnaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiWebWrapTimer;
    uint32 m_uiWebSprayTimer;
    uint32 m_uiPoisonShockTimer;
    uint32 m_uiNecroticPoisonTimer;
    uint32 m_uiSummonSpiderlingTimer;
    bool   m_bEnraged;

    void Reset()
    {
        m_uiWebWrapTimer = 20000;                           //20 sec init, 40 sec normal
        m_uiWebSprayTimer = 40000;                          //40 seconds
        m_uiPoisonShockTimer = 20000;                       //20 seconds
        m_uiNecroticPoisonTimer = 30000;                    //30 seconds
        m_uiSummonSpiderlingTimer = 30000;                  //30 sec init, 40 sec normal
        m_bEnraged = false;
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

   void DoCastWebWrap()
    {
        uint8 count = m_bIsRegularMode ? 1 : 2;
        PlrList list = GetRandomPlayers(count, true);
        for(PlrList::iterator itr = list.begin(); itr != list.end(); ++itr)
        {
            float LocX, LocY, LocZ;
            switch(rand()%3)
            {
                case 0: LocX = LOC_X1 + rand()%5; LocY = LOC_Y1 + rand()%5; LocZ = LOC_Z1 + 1; break;
                case 1: LocX = LOC_X2 + rand()%5; LocY = LOC_Y2 + rand()%5; LocZ = LOC_Z2 + 1; break;
                case 2: LocX = LOC_X3 + rand()%5; LocY = LOC_Y3 + rand()%5; LocZ = LOC_Z3 + 1; break;
            }
            DoTeleportPlayer(*itr, LocX, LocY, LocZ, (*itr)->GetOrientation());
            if (Creature* pWrap = m_creature->SummonCreature(16486, LocX, LocY, LocZ, 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 120000))
            {
                (*itr)->CastSpell(*itr, SPELL_WEBWRAP, true);
                (*itr)->AddThreat(m_creature, 10);
                m_creature->AddThreat(*itr, 10);
            }
        }
        m_uiWebWrapTimer = 40000;
    }
    void SummonSpiderling()
    {
        uint8 number = 9;
        float x,y,z;
        for(uint8 i = 0; number >= i; i++)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                m_creature->GetRandomPoint(m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),7.0f,x,y,z);
                if(Creature* spiderling = m_creature->SummonCreature(NPC_SPIDERLING, x, y, z,0, TEMPSUMMON_DEAD_DESPAWN, 0))
                {
                    spiderling->AddThreat(pTarget, 0.0f);
                    spiderling->AI()->AttackStart(pTarget);
                }
            }
        }
        m_uiSummonSpiderlingTimer = 40000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Web Wrap
        if (HandleTimer(m_uiWebWrapTimer, uiDiff, true))
            DoCastWebWrap();

        // Summon Spiderling
        if (HandleTimer(m_uiSummonSpiderlingTimer, uiDiff, true))
            SummonSpiderling();

        // Web Spray
        if (HandleTimer(m_uiWebSprayTimer, uiDiff, true))
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_WEBSPRAY : H_SPELL_WEBSPRAY);
            m_uiWebSprayTimer = 40000;
        }

        // Poison Shock
        if (HandleTimer(m_uiPoisonShockTimer, uiDiff, true))
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_POISONSHOCK : H_SPELL_POISONSHOCK);
            m_uiPoisonShockTimer = 20000;
        }

        // Necrotic Poison
        if (HandleTimer(m_uiNecroticPoisonTimer, uiDiff, true))
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_NECROTICPOISON : H_SPELL_NECROTICPOISON);
            m_uiNecroticPoisonTimer = 30000;
        }
        
        //Enrage if not already enraged and below 30%
        if (!m_bEnraged && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 30)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_FRENZY : H_SPELL_FRENZY);
            m_bEnraged = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_webwrap(Creature* pCreature)
{
    return new mob_webwrapAI(pCreature);
}

CreatureAI* GetAI_boss_maexxna(Creature* pCreature)
{
    return new boss_maexxnaAI(pCreature);
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
