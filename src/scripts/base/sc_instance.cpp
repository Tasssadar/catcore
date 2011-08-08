/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#include "precompiled.h"

//Optional uiWithRestoreTime. If not defined, autoCloseTime will be used (if not 0 by default in *_template)
void ScriptedInstance::DoUseDoorOrButton(uint64 uiGuid, uint32 uiWithRestoreTime, bool bUseAlternativeState)
{
    if (!uiGuid)
        return;

    GameObject* pGo = instance->GetGameObject(uiGuid);

    if (pGo)
    {
        if (pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR || pGo->GetGoType() == GAMEOBJECT_TYPE_BUTTON)
        {
            if (pGo->getLootState() == GO_READY)
                pGo->UseDoorOrButton(uiWithRestoreTime,bUseAlternativeState);
            else if (pGo->getLootState() == GO_ACTIVATED)
                pGo->ResetDoorOrButton();
        }
        else
            error_log("SD2: Script call DoUseDoorOrButton, but gameobject entry %u is type %u.",pGo->GetEntry(),pGo->GetGoType());
    }
}

void ScriptedInstance::OpenDoor(uint64 uiGuid)
{
    if (!uiGuid)
        return;

    GameObject* pGo = instance->GetGameObject(uiGuid);

    if (pGo)
    {
        if (pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR)
        {
            pGo->SetLootState(GO_READY);
            pGo->SetGoState(GO_STATE_ACTIVE);
        }
        else
            error_log("SD2: Script call OpenDoor can be used only on doors, but gameobject entry %u is type %u.", pGo->GetEntry(),pGo->GetGoType());
    }
    else
        error_log("SD2: Doors with guid %u not found, cant open them !", uiGuid);
}

void ScriptedInstance::CloseDoor(uint64 uiGuid)
{
    if (!uiGuid)
        return;

    GameObject* pGo = instance->GetGameObject(uiGuid);

    if (pGo)
    {
        if (pGo->GetGoType() == GAMEOBJECT_TYPE_DOOR)
        {
            pGo->SetLootState(GO_READY);
            pGo->SetGoState(GO_STATE_READY);
        }
        else
            error_log("SD2: Script call CloseDoor, but gameobject entry %u is type %u.",pGo->GetEntry(),pGo->GetGoType());
    }
}
void ScriptedInstance::HandleDoorsByData(uint64 uiGuid, uint8 uiData)
{
    if (uiData == IN_PROGRESS)
        CloseDoor(uiGuid);
    else if (uiData == FAIL || uiData == DONE)
        OpenDoor(uiGuid);
}
void ScriptedInstance::DoRespawnGameObject(uint64 uiGuid, uint32 uiTimeToDespawn)
{
    if (GameObject* pGo = instance->GetGameObject(uiGuid))
    {
        //not expect any of these should ever be handled
        if (pGo->GetGoType()==GAMEOBJECT_TYPE_FISHINGNODE || pGo->GetGoType()==GAMEOBJECT_TYPE_DOOR ||
            pGo->GetGoType()==GAMEOBJECT_TYPE_BUTTON || pGo->GetGoType()==GAMEOBJECT_TYPE_TRAP)
            return;

        if (pGo->isSpawned())
            return;

        pGo->SetRespawnTime(uiTimeToDespawn);
    }
}

void ScriptedInstance::DoUpdateWorldState(uint32 uiStateId, uint32 uiStateData)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    if (!lPlayers.isEmpty())
    {
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            if (Player* pPlayer = itr->getSource())
                pPlayer->SendUpdateWorldState(uiStateId, uiStateData);
        }
    }
    else
        debug_log("SD2: DoUpdateWorldState attempt send data but no players in map.");
}

void ScriptedInstance::CompleteAchievement(uint16 uiAchievementId, Player* pKiller, bool bWholeRaid)
{
    if(!pKiller)
        return;

    if(bWholeRaid && pKiller->GetGroup())
    {
        Group *pGroup = pKiller->GetGroup();
        for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player *pl = itr->getSource();
            if (pl->GetMapId() != pKiller->GetMapId() && pl->isAlive())
                continue;
            
            pl->CompletedAchievement(uiAchievementId);
        }
    }else
        pKiller->CompletedAchievement(uiAchievementId);
}

void ScriptedInstance::DoCompleteAchievement(uint32 uiAchievmentId)
{
    Map::PlayerList const& lPlayers = instance->GetPlayers();

    if (!lPlayers.isEmpty())
    {
        for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
            if (Player* pPlayer = itr->getSource())
                pPlayer->CompletedAchievement(uiAchievmentId);
        }
    }
    else
        debug_log("SD2: DoCompleteAchievement attempt set data but no players in map.");
}

Creature* ScriptedInstance::GetCreature(uint32 Data)
{
    uint64 guid = GetData64(Data);
    return instance ? instance->GetCreature(guid) : NULL;

}

GameObject* ScriptedInstance::GetGameObject(uint32 Data)
{
    uint64 guid = GetData64(Data);
    return instance ? instance->GetGameObject(guid) : NULL;
}
