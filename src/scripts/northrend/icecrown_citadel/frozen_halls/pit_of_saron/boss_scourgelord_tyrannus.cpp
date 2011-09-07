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
SDName: boss_scourgelord_tyrannus
SD%Complete: 0%
SDComment:
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "pit_of_saron.h"
#include "Vehicle.h"

enum
{
    SAY_INTRO1              = -1658302,
    SAY_INTRO2              = -1658304,
    SAY_AGGRO               = -1658305,
    SAY_MARK                = -1658306,
    SAY_POWER               = -1658307,
    SAY_KILL1               = -1658308,
    SAY_KILL2               = -1658309,
    SAY_DEATH               = -1658311,

    EMOTE_POWER             = -1658504,
    EMOTE_MARK              = -1658505,

    SPELL_FORCEFUL_SMASH    = 69155,
    SPELL_FORCEFUL_SMASH_H  = 69627,
    SPELL_OVERLORDS_BRAND   = 69172,
    SPELL_UNHOLY_POWER      = 69167,
    SPELL_UNHOLY_POWER_H    = 69629,

    SPELL_MARK_OF_RIMEFANG  = 69275,

    SPELL_HOARFROST         = 69245,
    SPELL_HOARFROST_H       = 69645,

    SPELL_ICY_BLAST         = 69232,
    
    ACTION_START_INTRO      = 1,
    ACTION_START_FIGHT      = 2,
    ACTION_END              = 3,

    NPC_ICY_BLAST           = 36731,
};

const float tyrannusPos[4] = { 1017.29, 168.97, 642.92, 5.27};
const float guidePos[4] = { 1064.50, 96.96, 631.1, 2.035 };

struct MANGOS_DLL_DECL boss_tyrannusAI : public ScriptedAI
{
    boss_tyrannusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        if(m_pInstance)
            m_pInstance->SetData64(NPC_TYRANNUS, m_creature->GetGUID());
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSmashTimer;
    uint32 m_uiUnholyPowerTimer;
    uint32 m_uiOverlordsBrandTimer;

    uint32 m_uiHoarfrostTimer;
    uint32 m_uiMarkOfRimefangTimer;
    uint8 m_uiMarkPhase;
    uint32 m_uiIcyBlastTimer;

    uint32 m_uiEvadeCheckTimer;

    uint8 introPhase;
    uint32 m_uiIntroTimer;
    bool m_intro;

    Vehicle *pRimefang;
    Unit *pMarkTarget;
    Creature *pGuide;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        SetCombatMovement(false);
        m_creature->SetRespawnDelay(86400);
        introPhase = 0;
        m_intro = false;
        pRimefang = NULL;

        m_uiSmashTimer = 10000;
        m_uiUnholyPowerTimer = 20000;
        m_uiIcyBlastTimer = 12000;
        m_uiMarkOfRimefangTimer = 6000;
        m_uiHoarfrostTimer = 5000;
        m_uiOverlordsBrandTimer = 12000;
        pMarkTarget = NULL;
        m_uiMarkPhase = 0;
        pGuide = NULL;
        m_uiEvadeCheckTimer = 3000;
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || !pRimefang || m_intro)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            if (IsCombatMovement())
                m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        pGuide->AI()->DoAction(1);
        pRimefang->AI()->DoAction(ACTION_END);
        DoScriptText(SAY_DEATH, m_creature);
        m_pInstance->SetData(TYPE_EVENT_STATE, 3);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0,1) ? SAY_KILL1 : SAY_KILL2, m_creature);
    }

    void EnterEvadeMode()
    {
        Vehicle *pRimefangNew = m_creature->SummonVehicle(NPC_RIMEFANG, tyrannusPos[0], tyrannusPos[1], tyrannusPos[2], tyrannusPos[3], 535, NULL, 0);
        pRimefangNew->SetRespawnDelay(86400);
        
        pRimefang->SetVisibility(VISIBILITY_OFF);
        pRimefang->ForcedDespawn();
        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->ForcedDespawn();
    }

    void DoAction(uint32 action)
    {
        switch(action)
        {
            case ACTION_START_INTRO:
            {
                m_intro = true;
                pRimefang = m_creature->GetMap()->GetVehicle(m_pInstance->GetData64(NPC_RIMEFANG));

                uint8 faction = m_pInstance->GetData(TYPE_FACTION);
                pGuide = GetClosestCreatureWithEntry(m_creature, faction ? NPC_SYLVANAS_END : NPC_JAINA_END, 200.0f);
                if(pGuide)
                {
                    introPhase = 2;
                    m_uiIntroTimer = 1000;
                    break;
                }

                DoScriptText(SAY_INTRO1, m_creature);
                m_uiIntroTimer = 15000;
                pGuide = m_creature->SummonCreature(faction ? NPC_SYLVANAS_END : NPC_JAINA_END,
                                           guidePos[0], guidePos[1], guidePos[2], guidePos[3], TEMPSUMMON_DEAD_DESPAWN, 0);
                break;
            }
        }
    }

    Unit *getRangedTarget()
    {
        Map::PlayerList const &lPlayers = m_creature->GetMap()->GetPlayers();
        std::vector<Unit*> targets;
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            if (Player* pPlayer = itr->getSource())
            {
                if(m_creature->GetDistance(pPlayer) >= 10.0f)
                    targets.push_back(pPlayer);
            }
        }
        if(!targets.empty())
            return targets[time(0)%targets.size()];
        else
            return m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 0);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_intro)
        {
            if(m_uiIntroTimer <= uiDiff)
            {
                switch(introPhase)
                {
                    case 0:
                        pGuide->AI()->DoAction(0);
                        m_uiIntroTimer = 10000;
                        break;
                    case 1:
                        DoScriptText(SAY_INTRO2, m_creature);
                        m_uiIntroTimer = 16000;
                        break;
                    case 2:
                        pRimefang->AI()->DoAction(ACTION_START_FIGHT);
                        m_uiIntroTimer = 2000;
                        break;
                    case 3:
                        SetCombatMovement(true);
                        m_intro = false;
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        if(pRimefang->getVictim())
                            AttackStart(pRimefang->getVictim());
                        break;
                }
                ++introPhase;
            }else m_uiIntroTimer -= uiDiff;
            return;
        }
        
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiEvadeCheckTimer <= uiDiff)
        {
            float x,y,z;
            m_creature->GetPosition(x, y, z);
            if(z < 610 || z > 640 || !m_creature->GetMap()->GetTerrain()->IsOutdoors(x,y,z))
            {
                EnterEvadeMode();
                return;
            }
            m_uiEvadeCheckTimer = 3000;
        }else m_uiEvadeCheckTimer -= uiDiff;

        if(m_uiSmashTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FORCEFUL_SMASH : SPELL_FORCEFUL_SMASH_H);
            m_uiSmashTimer = urand(10000,15000);
        }else m_uiSmashTimer -= uiDiff;

        if(m_uiUnholyPowerTimer <= uiDiff)
        {
            DoScriptText(SAY_POWER, m_creature);
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_UNHOLY_POWER : SPELL_UNHOLY_POWER_H);
            if(m_uiOverlordsBrandTimer < 1000)
                m_uiOverlordsBrandTimer = 1000;
            m_uiUnholyPowerTimer = urand(20000,25000);
            DoScriptText(EMOTE_POWER, m_creature);
        }else m_uiUnholyPowerTimer -= uiDiff;

        if(m_uiMarkOfRimefangTimer <= uiDiff)
        {
            if(!m_uiMarkPhase)
            {
                Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1);
                if(plr)
                {
                    DoScriptText(SAY_MARK, m_creature);
                    DoScriptText(EMOTE_MARK, m_creature, plr);
                    plr->CastSpell(plr, SPELL_MARK_OF_RIMEFANG, true);
                    pMarkTarget = plr;
                    m_uiMarkOfRimefangTimer = 7000;
                    m_uiMarkPhase = 1;
                }
                else m_uiMarkOfRimefangTimer = urand(7000, 12000);
            }
            else
            {
                m_uiMarkPhase = 0;
                pMarkTarget = NULL;
                m_uiMarkOfRimefangTimer = urand(7000, 12000);
            }
        }else m_uiMarkOfRimefangTimer -= uiDiff;

        if(m_uiOverlordsBrandTimer <= uiDiff)
        {
            Player* plr = m_creature->SelectAttackingPlayer(ATTACKING_TARGET_RANDOM, 1);
            if(plr)
                DoCast(plr, SPELL_OVERLORDS_BRAND);
            if(m_uiUnholyPowerTimer < 1500)
                m_uiUnholyPowerTimer = 1500;
            m_uiOverlordsBrandTimer = urand(12000, 17000);
        }else m_uiOverlordsBrandTimer -= uiDiff;


        // ===================================== Rimefang Spells
        if(pMarkTarget && pMarkTarget->IsInWorld() && pMarkTarget->isAlive())
        {
            if(m_uiHoarfrostTimer <= uiDiff)
            {
                pRimefang->CastSpell(pMarkTarget, m_bIsRegularMode ? SPELL_HOARFROST : SPELL_HOARFROST_H, true);
                m_uiHoarfrostTimer = 6000;
            }else m_uiHoarfrostTimer -= uiDiff;
        }

        if(m_uiIcyBlastTimer <= uiDiff)
        {
            Unit *target = NULL;
            //if(pMarkTarget && pMarkTarget->IsInWorld() && pMarkTarget->isAlive())
             //   target = pMarkTarget;
            //else
                target = getRangedTarget();
            if(target)
            {
                float x, y, z;
                target->GetPosition(x, y, z);
                Creature *icy_blast = m_creature->SummonCreature(NPC_ICY_BLAST, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
                pRimefang->CastSpell(icy_blast, SPELL_ICY_BLAST, true);
            }
            m_uiIcyBlastTimer = urand(8000, 12000);
        }else m_uiIcyBlastTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

const float dismountPos[3] = { 1028.41f, 167.93, 628.2};
const Coords fightPos[] =
{
    Coords(966.320f, 178.91f, 670.93f),
    Coords(1076.13f, 138.98f, 670.93f),
    Coords(1035.96f, 208.28f, 670.93f),
    Coords(988.350f, 113.54f, 670.93f),
    Coords(1012.25f, 160.87f, 670.93f),
};

#define FIGHT_POS 4

struct MANGOS_DLL_DECL boss_rimefang_posAI : public ScriptedAI
{
    boss_rimefang_posAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        if(m_pInstance && m_creature->isVehicle())
            m_pInstance->SetData64(NPC_RIMEFANG, m_creature->GetGUID());
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    Creature *pTyrannus;
    bool m_intro;

    uint32 flyTimer;
    bool m_end;
    
    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        SetCombatMovement(false);
        if(!m_creature->isVehicle())
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
            m_creature->ForcedDespawn(1000);
        }
        pTyrannus= NULL;
        m_intro = false;
        flyTimer = urand(10000, 20000);
        m_end = false;
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || m_end)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->isTargetableForAttack() && m_creature->IsHostileTo(pWho))
        {
            if (m_creature->IsWithinDistInMap(pWho, 40.0f) && m_creature->IsWithinLOSInMap(pWho))
            {
                if (!m_creature->getVictim())
                {
                    pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                    AttackStart(pWho);
                }
                else if (m_creature->GetMap()->IsDungeon())
                {
                    pWho->SetInCombatWith(m_creature);
                    m_creature->AddThreat(pWho);
                }
            }
        }
    }
    
    void EnterEvadeMode()
    {
    }

    void Aggro(Unit* pWho)
    {
        pTyrannus = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_TYRANNUS));
        if(pTyrannus)
        {
            m_intro = true; 
            pTyrannus->AI()->DoAction(ACTION_START_INTRO);
        }
    }

    void JustDied(Unit* pKiller)
    {
    }

    void DoRandomFlight()
    {
        uint8 tmp = urand(0, FIGHT_POS);
        PointPath path;
        path.resize(2);
        path.set(0, m_creature->GetPosition());
        path.set(1, fightPos[tmp]);
        uint32 time = path.GetTotalLength()/0.01f;
        m_creature->GetMotionMaster()->Clear(false, true);
        m_creature->ChargeMonsterMove(path, SPLINETYPE_NORMAL, SPLINEFLAG_FLYING,time);
    }

    void DoAction(uint32 action)
    {
        switch(action)
        {
            case ACTION_START_FIGHT:
                m_intro = false;
                ((Vehicle*)m_creature)->RemoveAllPassengers();
                pTyrannus->SendMonsterMove(dismountPos[0], dismountPos[1], dismountPos[2], SPLINETYPE_NORMAL, SPLINEFLAG_FORWARD, 0);
                pTyrannus->GetMap()->CreatureRelocation(pTyrannus, dismountPos[0], dismountPos[1], dismountPos[2], 0);
                DoRandomFlight();
                break;
            case ACTION_END:
                m_creature->RemoveAllAuras();
                m_creature->DeleteThreatList();
                m_creature->CombatStop(true);
                m_creature->SetVisibility(VISIBILITY_OFF);
                m_end = true;
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_end || m_intro || !m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(flyTimer <= uiDiff)
        {
            DoRandomFlight();
            flyTimer = urand(10000, 20000);
        }else flyTimer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_tyrannus(Creature* pCreature)
{
    return new boss_tyrannusAI(pCreature);
}

CreatureAI* GetAI_boss_rimefang_pos(Creature* pCreature)
{
    return new boss_rimefang_posAI(pCreature);
}

void AddSC_boss_scourgelord_tyrannus()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_tyrannus";
    newscript->GetAI = &GetAI_boss_tyrannus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_rimefang_pos";
    newscript->GetAI = &GetAI_boss_rimefang_pos;
    newscript->RegisterSelf();
}
