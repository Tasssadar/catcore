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
#include "pit_of_saron.h"

struct MANGOS_DLL_DECL instance_pit_of_saron : public ScriptedInstance
{
    instance_pit_of_saron(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;

    uint64 m_uiGarfrostGUID;
    uint64 m_uiIckGUID;
    uint64 m_uiTyrannusGUID;
    uint64 m_uiIceWallGUID;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        m_uiGarfrostGUID = 0;
        m_uiIckGUID = 0;
        m_uiTyrannusGUID = 0;
        m_uiIceWallGUID = 0;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_GARFROST:            m_uiGarfrostGUID = pCreature->GetGUID(); break;
            case NPC_TYRANNUS:            m_uiTyrannusGUID = pCreature->GetGUID(); break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_ICE_WALL:
                m_uiIceWallGUID = pGo->GetGUID();
                if (m_auiEncounter[1] == DONE && m_auiEncounter[0] == DONE)
                    DoUseDoorOrButton(m_uiIceWallGUID);
                break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        debug_log("SD2: Instance Ahn'Kahet: SetData received for type %u with data %u",uiType,uiData);

        switch(uiType)
        {
            case TYPE_GARFROST:
                m_auiEncounter[0] = uiData;
                if (uiData == DONE && m_auiEncounter[1] == DONE)
                    DoUseDoorOrButton(m_uiIceWallGUID);
                break;
            case TYPE_ICK_AND_KRICK:
                m_auiEncounter[1] = uiData;
                if (uiData == DONE && m_auiEncounter[0] == DONE)
                    DoUseDoorOrButton(m_uiIceWallGUID);
                break;
            case TYPE_TYRANNUS:
                m_auiEncounter[2] = uiData;
                break;
            default:
                error_log("SD2: Instance Pit of Saron: ERROR SetData = %u for type %u does not exist/not implemented.",uiType,uiData);
                break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " ";

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
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_GARFROST:
                return m_auiEncounter[0];
            case TYPE_ICK_AND_KRICK:
                return m_auiEncounter[1];
            case TYPE_TYRANNUS:
                return m_auiEncounter[2];
        }
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        return 0;
    }
};

InstanceData* GetInstanceData_instance_pit_of_saron(Map* pMap)
{
    return new instance_pit_of_saron(pMap);
}

void AddSC_instance_pit_of_saron()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "instance_pit_of_saron";
    newscript->GetInstanceData = &GetInstanceData_instance_pit_of_saron;
    newscript->RegisterSelf();
}
