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
SDName: instance_ahnkahet
SD%Complete: 0
SDComment:
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"
#include "forge_of_souls.h"

const float guidePos [4] = { 4903.63, 2207.12, 638.75, 0.442 };//{ 441.39f, 213.32f, 528.71f, 0.104f };

struct MANGOS_DLL_DECL instance_forge_of_souls : public ScriptedInstance
{
    instance_pit_of_saron(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_uiFaction;
    uint32 m_eventState;

    void Initialize()
    {
        m_uiFaction = 3;
    }

    void OnPlayerEnter(Player *plr)
    {
        if(m_uiFaction == 3)
        {
            if(plr->GetTeam() == HORDE)
                m_uiFaction = 1;
            else
                m_uiFaction = 0;
        }
        if(m_eventState == 0)
        {
            Creature *pGuide = m_creature->SummonCreature(m_uiFaction ? NPC_SYLVANAS : NPC_JAINA, guidePos[0], guidePos[1], guidePos[2],
                                                          guidePos[3], TEMPSUMMON_DEAD_DESPAWN, 0);
            float x = guidePos[0] + cos(guidePos[4])*15.0f;
            float y = guidePos[1] + sin(guidePos[4])*15.0f;
            pGuide->GetMotionMaster()->MovePoint(1, x, y, z);
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        debug_log("SD2: Instance Ahn'Kahet: SetData received for type %u with data %u",uiType,uiData);

        switch(uiType)
        {
            case TYPE_EVENT_STATE:
                m_eventState = uiData;
                break;
            default:
                error_log("SD2: Instance Forge of souls: ERROR SetData = %u for type %u does not exist/not implemented.",uiType,uiData);
                break;
        }

        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_uiFaction << " " << m_eventState << " ";

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* chrIn)
    {
        if (!chrIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(chrIn);

        std::istringstream loadStream(chrIn);
        loadStream >> m_uiFaction >> m_eventState;

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }
        if (m_auiEncounter[1] == DONE && m_auiEncounter[0] == DONE)
            DoUseDoorOrButton(m_uiIceWallGUID);

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_FACTION:
                return m_uiFaction;
            case TYPE_EVENT_STATE:
                return m_eventState;
        }
        return 0;
    }
};

InstanceData* GetInstanceData_instance_forge_of_souls(Map* pMap)
{
    return new instance_forge_of_souls(pMap);
}

void AddSC_instance_forge_of_souls()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "instance_forge_of_souls";
    newscript->GetInstanceData = &GetInstanceData_instance_forge_of_souls;
    newscript->RegisterSelf();
}
