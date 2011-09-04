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
SDName: instance_trial_of_the_crusader
SD%Complete: 80%
SDComment: by /dev/rsa
SDCategory: Trial of the Crusader
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

#define MAX_WIPES 50

struct MANGOS_DLL_DECL instance_trial_of_the_crusader : public ScriptedInstance
{
    instance_trial_of_the_crusader(Map* pMap) : ScriptedInstance(pMap)
    {
        mapDifficulty = pMap->GetDifficulty();
        isHeroic = pMap->IsHeroicRaid();
        Initialize();
    }

    uint32 EncounterData[MAX_ENCOUNTER];
    uint32 WipeCounter;
    uint32 m_auiNorthrendBeasts;

    GuidMap CreatureGuidMap;
    GuidMap GameObjectGuidMap;

    Difficulty mapDifficulty;
    bool isHeroic;
    std::string m_strInstData;
    bool needsave;

    void Initialize()
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            EncounterData[i] = NOT_STARTED;

        WipeCounter = 0;

        needsave = false;
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < MAX_ENCOUNTER ; ++i)
            if (EncounterData[i] == IN_PROGRESS)
                return true;

        return false;
    }

    void OnPlayerEnter(Player *m_player)
    {
        if (isHeroic)
        {
            m_player->SendUpdateWorldState(UPDATE_STATE_UI_SHOW,1);
            m_player->SendUpdateWorldState(UPDATE_STATE_UI_COUNT, MAX_WIPES-WipeCounter);
        }
        else
            m_player->SendUpdateWorldState(UPDATE_STATE_UI_SHOW,0);
    }

    void OnCreatureCreate(Creature* pCreature)
    {
         switch(pCreature->GetEntry())
         {
             case NPC_BARRENT:
             case NPC_TIRION:
             case NPC_FIZZLEBANG:
             case NPC_GARROSH:
             case NPC_RINN:
             case NPC_LICH_KING_0:
             case NPC_LICH_KING_1:
             case NPC_GORMOK:
             case NPC_ACIDMAW:
             case NPC_DREADSCALE:
             case NPC_ICEHOWL:
             case NPC_JARAXXUS:
             case NPC_DARKBANE:
             case NPC_LIGHTBANE:
             case NPC_ANUBARAK:
                CreatureGuidMap[pCreature->GetEntry()] = pCreature->GetGUID();
                break;
            default:
                break;
         }

         for(uint8 spec = CHAMPION_DEATH_KNIGHT; spec < CHAMPION_COUNT; ++spec)
             if (pCreature->GetEntry() == FChampIDs[spec][FACTION_ALLIANCE] ||
                 pCreature->GetEntry() == FChampIDs[spec][FACTION_HORDE])
                 CreatureGuidMap[pCreature->GetEntry()] = pCreature->GetGUID();
    }

    void OnObjectCreate(GameObject *pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_ARGENT_COLISEUM_FLOOR:
            case GO_MAIN_GATE_DOOR:
            case GO_GATE_EAST:
            case GO_GATE_SOUTH:
            case GO_GATE_NORTH:
                GameObjectGuidMap[pGo->GetEntry()] = pGo->GetGUID();
                break;
            default:
                break;
        }
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case NPC_BARRENT:
            case NPC_TIRION:
            case NPC_FIZZLEBANG:
            case NPC_GARROSH:
            case NPC_RINN:
            case NPC_LICH_KING_0:
            case NPC_LICH_KING_1:
            case NPC_GORMOK:
            case NPC_ACIDMAW:
            case NPC_DREADSCALE:
            case NPC_ICEHOWL:
            case NPC_JARAXXUS:
            case NPC_DARKBANE:
            case NPC_LIGHTBANE:
            case NPC_ANUBARAK:
                return CreatureGuidMap[uiData];
            case GO_ARGENT_COLISEUM_FLOOR:
            case GO_MAIN_GATE_DOOR:
            case GO_GATE_EAST:
            case GO_GATE_SOUTH:
            case GO_GATE_NORTH:
                return GameObjectGuidMap[uiData];
            default:
                break;
        }

        for(uint8 spec = CHAMPION_DEATH_KNIGHT; spec < CHAMPION_COUNT; ++spec)
            for(uint8 faction = FACTION_ALLIANCE; faction < FACTION_COUNT; ++faction)
                if (uiData == FChampIDs[spec][faction])
                    return CreatureGuidMap[uiData];

        return 0;
    }
    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case TYPE_BEASTS:
            case TYPE_JARAXXUS:
            case TYPE_CRUSADERS:
            case TYPE_VALKIRIES:
            case TYPE_LICH_KING:
            case TYPE_ANUBARAK:
            {
                if (Creature* pAnnouncer = GetCreature(NPC_BARRENT))
                    ((npc_toc_announcerAI*)pAnnouncer->AI())->DataSet(uiType,uiData);

                needsave = true;

                if (uiData > SPECIAL)
                {
                    uiData = IN_PROGRESS;
                    needsave = false;
                }

                EncounterData[uiType] = uiData;
                HandleDoorsByData(GameObjectGuidMap[GO_GATE_EAST], uiData);
                HandleDoorsByData(GameObjectGuidMap[GO_GATE_SOUTH], uiData);
                HandleDoorsByData(GameObjectGuidMap[GO_GATE_NORTH], uiData);

                if (uiData == FAIL)
                {
                    ++WipeCounter;

                    // update wipe counter for all players in instance
                    Map::PlayerList const &PlayerList = instance->GetPlayers();
                    if (!PlayerList.isEmpty())
                        for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                            if (Player* plr = i->getSource())
                                if (plr->IsInWorld())
                                    plr->SendUpdateWorldState(UPDATE_STATE_UI_COUNT, MAX_WIPES-WipeCounter);
                }

                break;
            }
            default:
                break;
        }

        if (needsave)
            _Save();
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_BEASTS:
            case TYPE_JARAXXUS:
            case TYPE_CRUSADERS:
            case TYPE_VALKIRIES:
            case TYPE_LICH_KING:
            case TYPE_ANUBARAK:
                return EncounterData[uiType];
            default:
                break;
        }
        return 0;
    }

    const char* Save()
    {
        return m_strInstData.c_str();
    }

    void _Save()
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            saveStream << EncounterData[i] << " ";

        saveStream << WipeCounter << " ";
        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
        needsave = false;
    }

    void Load(const char* strIn)
    {
        if (!strIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(strIn);

        std::istringstream loadStream(strIn);

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            loadStream >> EncounterData[i];

            if (EncounterData[i] == IN_PROGRESS)
                EncounterData[i] = NOT_STARTED;
        }

        loadStream >> WipeCounter;

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_trial_of_the_crusader(Map* pMap)
{
    return new instance_trial_of_the_crusader(pMap);
}

void AddSC_instance_trial_of_the_crusader()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_trial_of_the_crusader";
    newscript->GetInstanceData = &GetInstanceData_instance_trial_of_the_crusader;
    newscript->RegisterSelf();
}
