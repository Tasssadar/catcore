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
SDName: Grizzly_Hills
SD%Complete:
SDComment:
SDCategory: Grizzly Hills
EndScriptData */

#include "precompiled.h"
#include "escort_ai.h"

/*#####
### Harrison Jones
#####*/

enum{
    GO_HARRISONS_CAGE         = 188465,
    GO_ADARRAHS_CAGE          = 188487,
    GO_FIRE_DOOR              = 188480,
    QUEST_DUN_DA_DUN          = 12082,
    NPC_ADARRAH               = 24405,
    NPC_TECAHUNA              = 26865,

    SAY_01                    = -1535000,
    SAY_02                    = -1535001,
    SAY_03                    = -1535002,
    SAY_04                    = -1535003,
    SAY_05                    = -1535005,
    SAY_06                    = -1535006,
    SAY_07                    = -1535007,
    SAY_08                    = -1535008,
    SAY_09                    = -1535009,
    SAY_10                    = -1535010,

    ADARRAH_SAY01            = -1535004

};

struct MANGOS_DLL_DECL npc_harrisonAI : public npc_escortAI
{
    npc_harrisonAI(Creature* pCreature) : npc_escortAI(pCreature)
    { 
        m_creature->SetActiveObjectState(true);
        m_creature->SetSpeedRate(MOVE_RUN, 1);
        Reset(); 
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiSay;
    uint32 m_uiSayTimer;

    void Reset()
    {
        if(!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiSay = 0;
            m_uiSayTimer = 0;
        }
    }

    void JustDied(Unit* pTarget)
    {
        if(Player* pPlayer = GetPlayerForEscort())
            pPlayer->SetQuestStatus(QUEST_DUN_DA_DUN,QUEST_STATUS_FAILED);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if(pSummoned->GetEntry() != NPC_TECAHUNA)
        pSummoned->AI()->AttackStart(m_creature);
    }

    void WaypointReached(uint32 uiWPid)
    {
        switch(uiWPid)
        {
            case 0:
                DoScriptText(SAY_01,m_creature);
                break;
            case 3:
                m_uiSay = 1;
                m_uiSayTimer = 2000;
                break;
            case 5:
                m_uiSay = 3;
                m_uiSayTimer = 1000;
                break;
            case 7:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case 8:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                break;
            case 9:
                m_uiSay = 6;
                m_uiSayTimer = 1000;
                break;
            case 11:
                {
                GameObject* pFireDoor = GetClosestGameObjectWithEntry(m_creature,GO_FIRE_DOOR,20.0f);
                if(m_creature->GetMap()->GetGameObject(pFireDoor->GetGUID()))
                    pFireDoor->SetGoState(GO_STATE_READY);
                DoScriptText(SAY_07, m_creature);
                }
                break;
            case 12:
                m_creature->SummonCreature(NPC_TECAHUNA, 4909.49f, -4821.20f, 32.60f, 2.36f, TEMPSUMMON_CORPSE_TIMED_DESPAWN,5000);
                SetRun(true);
                break;
            case 13:
                m_uiSay = 8;
                m_uiSayTimer = 500;
                break;
            case 14:
                {
                GameObject* pFireDoor = GetClosestGameObjectWithEntry(m_creature,GO_FIRE_DOOR,50.0f);
                if(m_creature->GetMap()->GetGameObject(pFireDoor->GetGUID()))
                    pFireDoor->SetGoState(GO_STATE_ACTIVE);
                }
                break;
            case 27:
                DoScriptText(SAY_10, m_creature);
                if(Player* pPlayer = GetPlayerForEscort())
                pPlayer->GroupEventHappens(QUEST_DUN_DA_DUN,m_creature);
                break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if(m_uiSay)
        {
            if(m_uiSayTimer <= diff)
            {
                switch(m_uiSay)
                {
                    case 1:
                        DoScriptText(SAY_02,m_creature);
                        m_uiSay = 2;
                        m_uiSayTimer = 3000;
                        break;
                    case 2:
                        DoScriptText(SAY_03,m_creature);
                        m_uiSay = 20;
                        break;
                    case 3:
                        {
                        GameObject* pAdarrahsCage = GetClosestGameObjectWithEntry(m_creature,GO_ADARRAHS_CAGE,10.0f);
                        if(m_creature->GetMap()->GetGameObject(pAdarrahsCage->GetGUID()))
                            pAdarrahsCage->SetGoState(GO_STATE_ACTIVE);
                        m_uiSay = 4;
                        m_uiSayTimer = 1500;
                        }
                        break;
                    case 4:
                        DoScriptText(SAY_04,m_creature);
                        m_uiSay = 5;
                        m_uiSayTimer = 1500;
                        break;
                    case 5:
                        {
                        Creature* pAdarrah = GetClosestCreatureWithEntry(m_creature, NPC_ADARRAH, 10.0f);
                        DoScriptText(ADARRAH_SAY01, pAdarrah);
                        pAdarrah->RemoveSplineFlag(SPLINEFLAG_WALKMODE);
                        pAdarrah->GetMotionMaster()->MovePoint(0,4860.0f,-4800.0f,33.2f);
                        pAdarrah->GetMotionMaster()->MovementExpired();
                        pAdarrah->ForcedDespawn(4000);
                        m_uiSay = 20;
                        }
                        break;
                    case 6:
                        DoScriptText(SAY_05, m_creature);
                        m_uiSay = 7;
                        m_uiSayTimer = 3000;
                        break;
                    case 7:
                        DoScriptText(SAY_06, m_creature);
                        m_uiSay = 20;
                        break;
                    case 8:
                        DoScriptText(SAY_08, m_creature);
                        m_uiSay = 9;
                        m_uiSayTimer = 4000;
                        break;
                    case 9:
                        DoScriptText(SAY_09, m_creature);
                        m_uiSay = 10;
                        m_uiSayTimer = 4500;
                        break;
                    case 10:
                        {
                        Creature* pTecahuna = GetClosestCreatureWithEntry(m_creature, NPC_TECAHUNA, 20.0f);
                        if(m_creature->GetMap()->GetCreature(pTecahuna->GetGUID()))
                            m_creature->AI()->AttackStart(pTecahuna);
                        m_uiSay = 20;
                        }
                        break;
                }
            } else m_uiSayTimer -= diff;
        } 
        npc_escortAI::UpdateAI(diff);
    }
};

bool QuestAccept_npc_harrison(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_DUN_DA_DUN)
    {
        pCreature->setFaction(FACTION_ESCORT_N_NEUTRAL_PASSIVE);
        pCreature->SetSplineFlags(SPLINEFLAG_WALKMODE);

        GameObject* pHarrisonsCage = GetClosestGameObjectWithEntry(pCreature,GO_HARRISONS_CAGE,5.0f);
        if(pCreature->GetMap()->GetGameObject(pHarrisonsCage->GetGUID()))
            pHarrisonsCage->SetGoState(GO_STATE_ACTIVE);
       
        if (npc_harrisonAI* pEscortAI = ((npc_harrisonAI*)pCreature->AI()))
            pEscortAI->Start(false, pPlayer->GetGUID(), pQuest);
    }
    return true;
}

CreatureAI* GetAI_npc_harrison(Creature* pCreature)
{
    return new npc_harrisonAI(pCreature);
}

void AddSC_grizzly_hills()
{
    Script *pNewScript;
    
    pNewScript = new Script;
    pNewScript->Name = "npc_harrison";
    pNewScript->pQuestAccept = &QuestAccept_npc_harrison;
    pNewScript->GetAI = &GetAI_npc_harrison;
    pNewScript->RegisterSelf(); 
}
