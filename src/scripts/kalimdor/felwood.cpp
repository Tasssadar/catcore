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
SDName: Felwood
SD%Complete: 95
SDComment: Quest support: related to 4101/4102 (To obtain Cenarion Beacon), 4506, 7603 (Summon Pollo Grande), 4261
SDCategory: Felwood
EndScriptData */

/* ContentData
npc_kitten
npcs_riverbreeze_and_silversky
npc_niby_the_almighty
npc_arei
EndContentData */

#include "precompiled.h"
#include "follower_ai.h"
#include "ObjectMgr.h"
#include "escort_ai.h"

/*####
# npc_kitten
####*/

enum
{
    EMOTE_SAB_JUMP              = -1000541,
    EMOTE_SAB_FOLLOW            = -1000542,

    SPELL_CORRUPT_SABER_VISUAL  = 16510,

    QUEST_CORRUPT_SABER         = 4506,
    NPC_WINNA                   = 9996,
    NPC_CORRUPT_SABER           = 10042
};

#define GOSSIP_ITEM_RELEASE     "I want to release the corrupted saber to Winna."

struct MANGOS_DLL_DECL npc_kittenAI : public FollowerAI
{
    npc_kittenAI(Creature* pCreature) : FollowerAI(pCreature)
    {
        if (pCreature->GetOwner() && pCreature->GetOwner()->GetTypeId() == TYPEID_PLAYER)
        {
            StartFollow((Player*)pCreature->GetOwner());
            SetFollowPaused(true);
            DoScriptText(EMOTE_SAB_JUMP, m_creature);

            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            //find a decent way to move to center of moonwell
        }

        m_uiMoonwellCooldown = 7500;
        Reset();
    }

    uint32 m_uiMoonwellCooldown;

    void Reset() { }

    void MoveInLineOfSight(Unit* pWho)
    {
        //should not have npcflag by default, so set when expected
        if (!m_creature->getVictim() && !m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP) && HasFollowState(STATE_FOLLOW_INPROGRESS) && pWho->GetEntry() == NPC_WINNA)
        {
            if (m_creature->IsWithinDistInMap(pWho, INTERACTION_DISTANCE))
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        }
    }

    void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (HasFollowState(STATE_FOLLOW_PAUSED))
            {
                if (m_uiMoonwellCooldown < uiDiff)
                {
                    m_creature->CastSpell(m_creature, SPELL_CORRUPT_SABER_VISUAL, false);
                    SetFollowPaused(false);
                }
                else
                    m_uiMoonwellCooldown -= uiDiff;
            }

            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_kitten(Creature* pCreature)
{
    return new npc_kittenAI(pCreature);
}

bool EffectDummyCreature_npc_kitten(Unit* pCaster, uint32 uiSpellId, SpellEffectIndex uiEffIndex, Creature* pCreatureTarget)
{
    //always check spellid and effectindex
    if (uiSpellId == SPELL_CORRUPT_SABER_VISUAL && uiEffIndex == EFFECT_INDEX_0)
    {
        // Not nice way, however using UpdateEntry will not be correct.
        if (const CreatureInfo* pTemp = GetCreatureTemplateStore(NPC_CORRUPT_SABER))
        {
            pCreatureTarget->SetEntry(pTemp->Entry);
            pCreatureTarget->SetDisplayId(pTemp->ModelId[0]);
            pCreatureTarget->SetName(pTemp->Name);
            pCreatureTarget->SetFloatValue(OBJECT_FIELD_SCALE_X, pTemp->scale);
        }

        if (Unit* pOwner = pCreatureTarget->GetOwner())
            DoScriptText(EMOTE_SAB_FOLLOW, pCreatureTarget, pOwner);

        //always return true when we are handling this spell and effect
        return true;
    }
    return false;
}

bool GossipHello_npc_corrupt_saber(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_CORRUPT_SABER) == QUEST_STATUS_INCOMPLETE)
    {
        if (GetClosestCreatureWithEntry(pCreature, NPC_WINNA, INTERACTION_DISTANCE))
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_RELEASE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

bool GossipSelect_npc_corrupt_saber(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (npc_kittenAI* pKittenAI = dynamic_cast<npc_kittenAI*>(pCreature->AI()))
            pKittenAI->SetFollowComplete();

        pPlayer->AreaExploredOrEventHappens(QUEST_CORRUPT_SABER);
    }

    return true;
}

/*######
## npcs_riverbreeze_and_silversky
######*/

enum
{
    QUEST_CLEANSING_FELWOOD_A = 4101,
    QUEST_CLEANSING_FELWOOD_H = 4102,

    NPC_ARATHANDIS_SILVERSKY  = 9528,
    NPC_MAYBESS_RIVERBREEZE   = 9529,

    SPELL_CENARION_BEACON     = 15120
};

#define GOSSIP_ITEM_BEACON  "Please make me a Cenarion Beacon"

bool GossipHello_npcs_riverbreeze_and_silversky(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    switch (pCreature->GetEntry())
    {
        case NPC_ARATHANDIS_SILVERSKY:
            if (pPlayer->GetQuestRewardStatus(QUEST_CLEANSING_FELWOOD_A))
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                pPlayer->SEND_GOSSIP_MENU(2848, pCreature->GetGUID());
            }else if (pPlayer->GetTeam() == HORDE)
                pPlayer->SEND_GOSSIP_MENU(2845, pCreature->GetGUID());
            else
                pPlayer->SEND_GOSSIP_MENU(2844, pCreature->GetGUID());
            break;
        case NPC_MAYBESS_RIVERBREEZE:
            if (pPlayer->GetQuestRewardStatus(QUEST_CLEANSING_FELWOOD_H))
            {
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_BEACON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
                pPlayer->SEND_GOSSIP_MENU(2849, pCreature->GetGUID());
            }else if (pPlayer->GetTeam() == ALLIANCE)
                pPlayer->SEND_GOSSIP_MENU(2843, pCreature->GetGUID());
            else
                pPlayer->SEND_GOSSIP_MENU(2842, pCreature->GetGUID());
            break;
    }
   
    return true;
}

bool GossipSelect_npcs_riverbreeze_and_silversky(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer, SPELL_CENARION_BEACON, false);
    }
    return true;
}

/*######
## npc_niby_the_almighty (summons el pollo grande)
######*/
enum
{
    QUEST_KROSHIUS     = 7603,

    NPC_IMPSY          = 14470,

    SPELL_SUMMON_POLLO = 23056,

    SAY_NIBY_1         = -1000566,
    SAY_NIBY_2         = -1000567,
    EMOTE_IMPSY_1      = -1000568,
    SAY_IMPSY_1        = -1000569,
    SAY_NIBY_3         = -1000570
};

struct MANGOS_DLL_DECL npc_niby_the_almightyAI : public ScriptedAI
{
    npc_niby_the_almightyAI(Creature* pCreature) : ScriptedAI(pCreature){ Reset(); }

    uint32 m_uiSummonTimer;
    uint8  m_uiSpeech;

    bool m_bEventStarted;

    void Reset()
    {
        m_uiSummonTimer = 500;
        m_uiSpeech = 0;

        m_bEventStarted = false;
    }

    void StartEvent()
    {
        Reset();
        m_bEventStarted = true;
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bEventStarted)
        {
            if (m_uiSummonTimer <= uiDiff)
            {
                switch (m_uiSpeech)
                {
                    case 1:
                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetMotionMaster()->MovePoint(0, 5407.19f, -753.00f, 350.82f);
                        m_uiSummonTimer = 6200;
                        break;
                    case 2:
                        m_creature->SetFacingTo(1.2f);
                        DoScriptText(SAY_NIBY_1, m_creature);
                        m_uiSummonTimer = 3000;
                        break;
                    case 3:
                        DoScriptText(SAY_NIBY_2, m_creature);
                        DoCastSpellIfCan(m_creature, SPELL_SUMMON_POLLO);
                        m_uiSummonTimer = 2000;
                        break;
                    case 4:
                        if (Creature* pImpsy = GetClosestCreatureWithEntry(m_creature, NPC_IMPSY, 20.0))
                        {
                            DoScriptText(EMOTE_IMPSY_1, pImpsy);
                            DoScriptText(SAY_IMPSY_1, pImpsy);
                            m_uiSummonTimer = 2500;
                        }
                        else
                        {
                            //Skip Speech 5
                            m_uiSummonTimer = 40000;
                            ++m_uiSpeech;
                        }
                        break;
                    case 5:
                        DoScriptText(SAY_NIBY_3, m_creature);
                        m_uiSummonTimer = 40000;
                        break;
                    case 6:
                        m_creature->GetMotionMaster()->MoveTargetedHome();
                        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                        m_bEventStarted = false;
                }
                ++m_uiSpeech;
            }
            else
                m_uiSummonTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_niby_the_almighty(Creature* pCreature)
{
    return new npc_niby_the_almightyAI(pCreature);
}

bool ChooseReward_npc_niby_the_almighty(Player* pPlayer, Creature* pCreature, const Quest* pQuest, uint32 slot)
{
    if (pQuest->GetQuestId() == QUEST_KROSHIUS)
    {
        if (npc_niby_the_almightyAI* pNibyAI = dynamic_cast<npc_niby_the_almightyAI*>(pCreature->AI()))
        {
            pNibyAI->StartEvent();
        }
    }
    return true;
}
/*########
## npc_arei
########*/

#define QUEST_ANCIENT_SPIRIT    4261

#define C_TOXIC_HORROR			7132
#define C_IRONTREE_STOMPER		7139

#define S_AREI_TRANSFORM       14888

#define GOSSIP_SAVED            2809

#define SAY_START			-1361000
#define SAY_AGGRO			-1361001
#define SAY_FRAG			-1361002
#define SAY_END1			-1361003
#define SAY_END2			-1361004
#define SAY_END3			-1361005
#define SAY_END4			-1361006


const float SpawnPoints[5][3]=
{
	{6172.33f,-1276.82f,374.6550f},
	{6156.52f,-1293.24f,375.9467f},
	{6603.18f,-1218.79f,448.3316f},
	{6577.01f,-1235.22f,446.2619f},
	{6564.91f,-1184.70f,446.2590f}
};

struct MANGOS_DLL_DECL npc_areiAI : public npc_escortAI
{
    npc_areiAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

    bool Saved;
    uint8 RPphase;
    uint16 RPtimer;
    void WaypointReached(uint32 i)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        Creature* pCreature;

        switch(i)
        {
            case 10:
                for (uint8 i = 0; i < 2 ; i++ )
                {
                    if (pCreature=m_creature->SummonCreature(C_TOXIC_HORROR,SpawnPoints[i][0],SpawnPoints[i][1],SpawnPoints[i][2],0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10000))
                        pCreature->AI()->AttackStart(m_creature);
                }
                break;
            case 23:
                for (uint8 i = 2; i < 5 ; i++ )
                {
                    if (pCreature=m_creature->SummonCreature(C_IRONTREE_STOMPER,SpawnPoints[i][0],SpawnPoints[i][1],SpawnPoints[i][2],0,TEMPSUMMON_CORPSE_TIMED_DESPAWN,10000))
                        pCreature->AI()->AttackStart(m_creature);
                }
                break;
            case 24:
                RPtimer=500;
                RPphase=0;
                break;
        }
    }

    void Aggro(Unit* who)
    {
        if (!(rand()%5))
            DoScriptText(SAY_AGGRO, m_creature, who);
    }

    void KilledUnit(Unit* who)
    {
        if (!(rand()%5))
            DoScriptText(SAY_FRAG, m_creature, who);
    }

    void UpdateEscortAI(const uint32 diff)
    {
        if (RPtimer)
        {
            if (RPtimer<=diff)
            {
                Player* pPlayer = GetPlayerForEscort();

                if (!pPlayer)
                    return;

                switch (RPphase)
                {
                    case 0:
                        DoScriptText(SAY_END1,m_creature, pPlayer);
                        RPtimer=4500;
                        RPphase++;
                        break;
                    case 1:
                        DoScriptText(SAY_END2,m_creature, pPlayer);
                        RPtimer=2500;
                        RPphase++;
                        break;
                    case 2:
                        DoCast(m_creature,S_AREI_TRANSFORM);
                        RPphase++;
                        RPtimer=3000;
                        break;
                    case 3:
                        DoScriptText(SAY_END3,m_creature,pPlayer);
                        RPphase++;
                        RPtimer=6000;
                        break;
                    case 4:
                        DoScriptText(SAY_END4,m_creature,pPlayer);
                        RPtimer=0;
                        Saved=true;
                        pPlayer->GroupEventHappens(QUEST_ANCIENT_SPIRIT,m_creature);
                        break;
                }
            }
            else RPtimer-=diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
    void Reset()
    {
        Saved=false;
        RPtimer=0;
    }
};

bool QuestAccept_npc_arei(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_ANCIENT_SPIRIT)
    {
        if (npc_areiAI* pEscortAI = dynamic_cast<npc_areiAI*>(pCreature->AI()))
            pEscortAI->Start(true, pPlayer->GetGUID(), pQuest);
        DoScriptText(SAY_START, pCreature, pPlayer);
    }
    return true;
}

CreatureAI* GetAI_npc_arei(Creature* pCreature)
{
    return new npc_areiAI(pCreature);
}

void AddSC_felwood()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "npc_kitten";
    newscript->GetAI = &GetAI_npc_kitten;
    newscript->pEffectDummyCreature = &EffectDummyCreature_npc_kitten;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_corrupt_saber";
    newscript->pGossipHello = &GossipHello_npc_corrupt_saber;
    newscript->pGossipSelect = &GossipSelect_npc_corrupt_saber;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npcs_riverbreeze_and_silversky";
    newscript->pGossipHello = &GossipHello_npcs_riverbreeze_and_silversky;
    newscript->pGossipSelect = &GossipSelect_npcs_riverbreeze_and_silversky;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_niby_the_almighty";
    newscript->GetAI = &GetAI_npc_niby_the_almighty;
    newscript->pChooseReward = &ChooseReward_npc_niby_the_almighty;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_arei";
    newscript->GetAI = &GetAI_npc_arei;
    newscript->pQuestAccept = &QuestAccept_npc_arei;
    newscript->RegisterSelf();
}
