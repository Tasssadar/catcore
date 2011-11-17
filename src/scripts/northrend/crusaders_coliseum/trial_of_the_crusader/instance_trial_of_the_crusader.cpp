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
SD%Complete: nevim%
SDComment:
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
    uint32 AttemptsRemaining;
    uint32 ChampionSpawnMask;

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

        AttemptsRemaining = 0;
        ChampionSpawnMask = 0;

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
        if (!m_player)
            return;

        if (isHeroic)
        {
            m_player->SendUpdateWorldState(UPDATE_STATE_UI_SHOW,1);
            m_player->SendUpdateWorldState(UPDATE_STATE_UI_COUNT, AttemptsRemaining);
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
             case NPC_WRYNN:
             case NPC_LICH_KING:
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

         for(uint8 spec = CHAMPION_DEATH_KNIGHT; spec < CHAMPION_ALL_COUNT; ++spec)
             if (pCreature->GetEntry() == FChampIDs[spec][FACTION_ALLIANCE] ||
                 pCreature->GetEntry() == FChampIDs[spec][FACTION_HORDE])
                 CreatureGuidMap[pCreature->GetEntry()] = pCreature->GetGUID();
    }

    void OnObjectCreate(GameObject *pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_ARGENT_COLISEUM_FLOOR:
                GameObjectGuidMap[pGo->GetEntry()] = pGo->GetGUID();
                if (EncounterData[TYPE_LICH_KING] == DONE &&
                    EncounterData[TYPE_ANUBARAK] != DONE)
                    pGo->Delete();
                break;
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
            case NPC_WRYNN:
            case NPC_LICH_KING:
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

        for(uint8 spec = CHAMPION_DEATH_KNIGHT; spec < CHAMPION_ALL_COUNT; ++spec)
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
                if (uiData == EncounterData[uiType])
                    break;

                if (Creature* pAnnouncer = GetCreature(NPC_BARRENT))
                    ((npc_toc_announcerAI*)pAnnouncer->AI())->DataSet(uiType,uiData);

                needsave = true;

                if (uiData > SPECIAL)
                {
                    uiData = IN_PROGRESS;
                    needsave = false;
                }
                else if (uiData == FAIL && isHeroic)
                {
                    --AttemptsRemaining;

                    // update wipe counter for all players in instance
                    Map::PlayerList const &PlayerList = instance->GetPlayers();
                    for(Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        OnPlayerEnter(i->getSource());
                }

                EncounterData[uiType] = uiData;
                HandleDoorsByData(GameObjectGuidMap[GO_GATE_EAST], uiData);
                HandleDoorsByData(GameObjectGuidMap[GO_GATE_SOUTH], uiData);
                HandleDoorsByData(GameObjectGuidMap[GO_GATE_NORTH], uiData);
                break;
            }
            case TYPE_CHAMPION_SPAWN_MASK:
                ChampionSpawnMask = uiData;
                needsave = true;
                break;
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
            case TYPE_COUNTER:
                return AttemptsRemaining;
            case TYPE_CHAMPION_SPAWN_MASK:
                return ChampionSpawnMask;
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

        saveStream << AttemptsRemaining << " ";
        saveStream << ChampionSpawnMask << " ";

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

        loadStream >> AttemptsRemaining;
        loadStream >> ChampionSpawnMask;

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
