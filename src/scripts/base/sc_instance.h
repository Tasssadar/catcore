/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
* This program is free software licensed under GPL version 2
* Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_INSTANCE_H
#define SC_INSTANCE_H

#include "InstanceData.h"
#include "Map.h"

enum EncounterState
{
    NOT_STARTED   = 0,
    IN_PROGRESS   = 1,
    FAIL          = 2,
    DONE          = 3,
    SPECIAL       = 4
};

enum InstanceSide
{
    INSTANCE_SIDE_NONE  = 0,
    INSTANCE_SIDE_ALI   = 1,
    INSTANCE_SIDE_HORDE = 2
};

#define OUT_SAVE_INST_DATA             debug_log("SD2: Saving Instance Data for Instance %s (Map %d, Instance Id %d)", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_SAVE_INST_DATA_COMPLETE    debug_log("SD2: Saving Instance Data for Instance %s (Map %d, Instance Id %d) completed.", instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA(a)          debug_log("SD2: Loading Instance Data for Instance %s (Map %d, Instance Id %d). Input is '%s'", instance->GetMapName(), instance->GetId(), instance->GetInstanceId(), a)
#define OUT_LOAD_INST_DATA_COMPLETE    debug_log("SD2: Instance Data Load for Instance %s (Map %d, Instance Id: %d) is complete.",instance->GetMapName(), instance->GetId(), instance->GetInstanceId())
#define OUT_LOAD_INST_DATA_FAIL        error_log("SD2: Unable to load Instance Data for Instance %s (Map %d, Instance Id: %d).",instance->GetMapName(), instance->GetId(), instance->GetInstanceId())

class MANGOS_DLL_DECL ScriptedInstance : public InstanceData
{
    public:

        ScriptedInstance(Map* pMap) : InstanceData(pMap) {}
        ~ScriptedInstance() {}

        //change active state of doors or buttons
        void DoUseDoorOrButton(uint64 uiGuid, uint32 uiWithRestoreTime = 0, bool bUseAlternativeState = false);

        // open doors
        void OpenDoor(uint64 uiGuid);

        // close doors
        void CloseDoor(uint64 uiGuid);

        // Handle doors
        void HandleDoorsByData(uint64 uiGuid, uint8 uiData);

        // Respawns a GO having negative spawntimesecs in gameobject-table
        void DoRespawnGameObject(uint64 uiGuid, uint32 uiTimeToDespawn = MINUTE);

        // sends world state update to all players in instance
        void DoUpdateWorldState(uint32 uiStateId, uint32 uiStateData);

        // complete achievement
        void CompleteAchievement(uint16 uiAchievementId, Player* pKiller, bool bWholeRaid);

        // sends completed achievments to all players in instance
        void DoCompleteAchievement(uint32 uiAchievmentId);

        // get creature from instanceData
        Creature* GetCreature(uint32 Data);

        // get gameobject from instanceData
        GameObject* GetGameObject(uint32 Data);

        // get random player in map
        Player* GetRandomPlayerInMap();

        // get team in instance
        InstanceSide GetInstanceSide();

        // Update Achievement Criteria for all players in instance
        void DoUpdateAchievementCriteria(AchievementCriteriaTypes type, uint32 miscValue1 = 0, uint32 miscValue2 = 0, Unit* unit = NULL);
};
#endif
