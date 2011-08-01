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
SDName: Boss_Drakos
SD%Complete: 99%
SDComment: timers
SDCategory: Nexus, Oculus
EndScriptData */

#include "precompiled.h"
#include "oculus.h"
#include "Vehicle.h"

enum
{
    SAY_AGGRO           = -1578001,
    SAY_PULL1           = -1578002,
    SAY_PULL2           = -1578003,
    SAY_PULL3           = -1578004,
    SAY_PULL4           = -1578005,
    SAY_STOMP1          = -1578006,
    SAY_STOMP2          = -1578007,
    SAY_STOMP3          = -1578008,
    SAY_KILL1           = -1578009,
    SAY_KILL2           = -1578010,
    SAY_KILL3           = -1578011,
    SAY_DEATH           = -1578012,
    SAY_VAROS_TAUNT     = -1578013,

    SPELL_STOMP         = 50774, 
    SPELL_STOMP_H       = 59370,
    SPELL_PULL          = 51336,

    SPELL_SPHERE_PULSE  = 50757,
    SPELL_SPHERE_EXPLODE= 50759,

    NPC_SPHERE          = 28166,

    COUNT_SPHERE        = 2,
    COUNT_SPHERE_H      = 4,
    SPHERE_X_MIN        = 909,
    SPHERE_X_MAX        = 1008,
    SPHERE_Y_MIN        = 1013,
    SPHERE_Y_MAX        = 1084,
    SPHERE_Z            = 365
};

/*######
## boss_drakos
######*/

struct MANGOS_DLL_DECL boss_drakosAI : public ScriptedAI
{
    boss_drakosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiStompTimer;
    uint32 m_uiPullTimer;
    uint32 m_uiPullCastTimer;
    uint32 m_uiSphereTimer;


    void Reset()
    {
        m_uiStompTimer = urand(25000, 35000);
        m_uiPullTimer = urand(15000, 25000);
        m_uiPullCastTimer = m_uiPullTimer+2000;
        m_uiSphereTimer = urand(10000, 18000);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        m_pInstance->SetData(TYPE_DRAKOS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_DRAKOS, DONE);
            if(Creature *Varos = m_pInstance->instance->GetCreature(m_pInstance->GetData64(NPC_VAROS)))
                DoScriptText(SAY_VAROS_TAUNT, Varos);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        uint8 tmp = urand(0, 2);
        DoScriptText(SAY_KILL1-tmp, m_creature);
    }

    void SummonSphere()
    {
        float x,y,z;
        m_creature->GetPosition(x,y,z);
        uint8 count = m_bIsRegularMode ? COUNT_SPHERE : COUNT_SPHERE_H;
        for(uint8 i = 0; i < count; ++i)
            m_creature->SummonCreature(NPC_SPHERE, x, y, SPHERE_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 20000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiStompTimer <= uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_STOMP : SPELL_STOMP);
            uint8 tmp = urand(0, 2);
            DoScriptText(SAY_STOMP1-tmp, m_creature);
            m_uiStompTimer = urand(25000, 35000);
        }else m_uiStompTimer -= uiDiff;

        if(m_uiPullTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_PULL);
            uint8 tmp = urand(0, 3);
            DoScriptText(SAY_PULL1-tmp, m_creature);
            m_uiPullTimer = urand(15000, 25000);
        }else m_uiPullTimer -= uiDiff;

        if(m_uiPullCastTimer <= uiDiff)
        {
            float x,y,z;
            m_creature->GetPosition(x,y,z);
            Map* pMap = m_creature->GetMap();
            if (pMap && pMap->IsDungeon())
            {
                Map::PlayerList const &PlayerList = pMap->GetPlayers();

                if (!PlayerList.isEmpty())
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (i->getSource()->isAlive())
                            i->getSource()->NearTeleportTo(x, y, z, 0);
            }
            m_uiPullCastTimer = m_uiPullTimer+2000;
        }else m_uiPullCastTimer -= uiDiff;

        if(m_uiSphereTimer <= uiDiff)
        {
            SummonSphere();
            m_uiSphereTimer = urand(10000, 18000);
        }else m_uiSphereTimer -= uiDiff; 

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_unstable_sphere
######*/

struct MANGOS_DLL_DECL npc_unstable_sphereAI : public ScriptedAI
{
    npc_unstable_sphereAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiPulseTimer;
    uint32 m_uiDetonateTimer;


    void Reset()
    {
        m_uiPulseTimer = 0;
        m_uiDetonateTimer = urand(14000, 20000);
        Move();
    }

    void AttackStart(Unit *pWho)
    {
        return;
    }

    void Move()
    {
        float x = urand(SPHERE_X_MIN, SPHERE_X_MAX);
        float y = urand(SPHERE_Y_MIN, SPHERE_Y_MAX);
        m_creature->GetMotionMaster()->MovePoint(1, x, y, SPHERE_Z);
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId)
    {
        if (uiMoveType != POINT_MOTION_TYPE || uiPointId != 1)
            return;

        Move();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiPulseTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_SPHERE_PULSE);
            m_uiPulseTimer = 1000;
        }else m_uiPulseTimer -= uiDiff;

        if(m_uiDetonateTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_SPHERE_EXPLODE);
            m_creature->ForcedDespawn(500);
            m_uiDetonateTimer = 15000;
        }else m_uiDetonateTimer -= uiDiff;
    }
};

/*######
## item_dragon_essence
######*/

enum
{
    NPC_RUBY_DRAKE          = 27756,
    NPC_AMBER_DRAKE         = 27755,
    NPC_EMERALD_DRAKE       = 27692,

    ITEM_EMERALD_ESSENCE    = 37815,
    ITEM_RUBY_ESSENCE       = 37860,
    ITEM_AMBER_ESSENCE      = 37859 
};   

bool ItemUse_item_dragon_essence(Player* pPlayer, Item* pItem, const SpellCastTargets &pTargets)
{
    if(pPlayer->isInCombat())
        return false;

    uint32 entry = 0;
    switch(pItem->GetEntry())
    {
        case ITEM_RUBY_ESSENCE:
            entry = NPC_RUBY_DRAKE;
            break;
        case ITEM_AMBER_ESSENCE:
            entry = NPC_AMBER_DRAKE;
            break;
        case ITEM_EMERALD_ESSENCE:
            entry = NPC_EMERALD_DRAKE;
            break;
        default:
            return false;
    }
    float x,y,z;
    pPlayer->GetPosition(x,y,z);
    z += 1.5f;
    Vehicle *pDrake = pPlayer->SummonVehicle(entry, x, y, z, 0);
    if(!pDrake)
        return false;
    pPlayer->EnterVehicle(pDrake, -1, true);
    return false;
}

CreatureAI* GetAI_boss_drakos(Creature* pCreature)
{
    return new boss_drakosAI(pCreature);
}

CreatureAI* GetAI_npc_unstable_sphere(Creature* pCreature)
{
    return new npc_unstable_sphereAI(pCreature);
}

void AddSC_boss_drakos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_drakos";
    newscript->GetAI = &GetAI_boss_drakos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_unstable_sphere";
    newscript->GetAI = &GetAI_npc_unstable_sphere;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "item_dragon_essence";
    newscript->pItemUse = &ItemUse_item_dragon_essence;
    newscript->RegisterSelf();
}
/*
UPDATE `creature_template` SET `modelid_A` = '11686', `modelid_H` = '11686', unit_flags=33554434,
type_flags = 0, ScriptName="npc_unstable_sphere" WHERE `entry` =28166;

DELETE FROM creature WHERE id=28166;

UPDATE `creature_template` SET ScriptName="boss_drakos" WHERE `entry` = 27654;
UPDATE instance_template SET script="instance_oculus" WHERE map=578;
UPDATE item_template SET ScriptName="item_dragon_essence" WHERE entry IN (37815, 37860, 37859);

DELETE FROM script_texts WHERE entry BETWEEN -1578014 AND -1578000;
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES 
('-1578001', 'The prisoners shall not go free. The word of Malygos is law!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13594', '1', '0', '0', NULL),
('-1578002', 'It is too late to run!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13598', '1', '0', '0', NULL),
('-1578003', 'Gather \'round...', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13599', '1', '0', '0', NULL),
('-1578004', 'None shall escape!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13600', '1', '0', '0', NULL),
('-1578005', 'I condemn you to death!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13601', '1', '0', '0', NULL),
('-1578006', 'Tremble, worms!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13595', '1', '0', '0', NULL),
('-1578007', 'I will crush you!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13596', '1', '0', '0', NULL),
('-1578008', 'Can you fly?', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13597', '1', '0', '0', NULL),
('-1578009', 'A fitting punishment!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13602', '1', '0', '0', NULL),
('-1578010', 'Sentence: executed!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13603', '1', '0', '0', NULL),
('-1578011', 'Another casualty of war!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13604', '1', '0', '0', NULL),
('-1578012', 'The war... goes on.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13605', '1', '0', '0', NULL),
('-1578013', 'Intruders, your victory will be short-lived. I am Commander Varos Cloudstrider. My drakes control the skies and protect this conduit. I will see to it personally that the Oculus does not fall into your hands!', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '13648', '1', '0', '0', NULL);
*/
 