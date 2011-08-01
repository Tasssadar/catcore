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
SDName: instance_oculus
SD%Complete: 75%
SDComment:
SDCategory: The Nexus, Oculus
EndScriptData */

#include "precompiled.h"
#include "oculus.h"

struct MANGOS_DLL_DECL instance_oculus : public ScriptedInstance
{
    instance_oculus(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;

    uint64 m_uiDrakosGUID;
    uint64 m_uiVarosGUID;
    uint64 m_uiUromGUID;
    uint64 m_uiEregosGUID;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        m_uiDrakosGUID = 0;
        m_uiVarosGUID = 0;
        m_uiUromGUID = 0;
        m_uiEregosGUID = 0;
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;
        }

        return false;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_DRAKOS:
                m_uiDrakosGUID = pCreature->GetGUID();
                break;
            case NPC_VAROS:
                m_uiVarosGUID = pCreature->GetGUID();
                break;
            case NPC_UROM:
                m_uiUromGUID = pCreature->GetGUID();
                break;
            case NPC_EREGOS:
                m_uiEregosGUID = pCreature->GetGUID();
                break;
        }
    }

    uint64 GetData64(uint32 uiType)
    {
        switch(uiType)
        {
            case NPC_DRAKOS: return m_uiDrakosGUID;
            case NPC_VAROS:  return m_uiVarosGUID;
            case NPC_UROM:   return m_uiUromGUID;
            case NPC_EREGOS: return m_uiEregosGUID;
        }
        return 0;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_DRAKOS:
                return m_auiEncounter[0];
            case TYPE_VAROS:
                return m_auiEncounter[1];
            case TYPE_UROM:
                return m_auiEncounter[2];
            case TYPE_EREGOS:
                return m_auiEncounter[3];
        }

        return 0;
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        debug_log("SD2: Instance Nexus: SetData received for type %u with data %u", uiType, uiData);

        switch(uiType)
        {
            case TYPE_DRAKOS:
                m_auiEncounter[0] = uiData;
                break;
            case TYPE_VAROS:
                m_auiEncounter[1] = uiData;
                break;
            case TYPE_UROM:
                m_auiEncounter[2] = uiData;
                break;
            case TYPE_EREGOS:
                m_auiEncounter[3] = uiData;
                break;
            default:
                error_log("SD2: Instance Oculus: ERROR SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
                break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
        else if(uiData == IN_PROGRESS)
            DismountPlayers();
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
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }
    
    void DismountPlayers()
    {
        Map::PlayerList const &PlayerList = instance->GetPlayers();

        if (!PlayerList.isEmpty())
            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                if (i->getSource()->isAlive())
                    i->getSource()->ExitVehicle();
    }
};

InstanceData* GetInstanceData_instance_oculus(Map* pMap)
{
    return new instance_oculus(pMap);
}

void AddSC_instance_oculus()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "instance_oculus";
    newscript->GetInstanceData = &GetInstanceData_instance_oculus;
    newscript->RegisterSelf();
}
