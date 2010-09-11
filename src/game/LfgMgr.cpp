/*
 * Copyright (C) 2005-2010 MaNGOS <http://getmangos.com/>
 *
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

#include "Common.h"
#include "SharedDefines.h"
#include "Player.h"
#include "World.h"
#include "LfgMgr.h"
#include "LfgGroup.h"
#include "DBCStores.h"
#include "ObjectMgr.h"
#include "ProgressBar.h"
#include "Unit.h"
#include "SpellAuras.h"

#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1( LfgMgr );

LfgMgr::LfgMgr()
{
    m_updateQueuesBaseTime = sWorld.getConfig(CONFIG_UINT32_LFG_QUEUE_UPDATETIME);
    m_updateQueuesTimer = m_updateQueuesBaseTime;
    m_updateProposalTimer = LFG_TIMER_UPDATE_PROPOSAL;
    m_deleteInvalidTimer = LFG_TIMER_DELETE_INVALID_GROUPS;
    log = sWorld.getConfig(CONFIG_BOOL_LFG_LOG);
}

LfgMgr::~LfgMgr()
{
    for(int i = 0; i < MAX_LFG_FACTION; ++i)
    {
        //dungeons...
        for(QueuedDungeonsMap::iterator itr = m_queuedDungeons[i].begin(); itr != m_queuedDungeons[i].end();++itr)
        {
            for(GroupsList::iterator grpitr = itr->second->groups.begin(); grpitr != itr->second->groups.end();++grpitr)
            {
                (*grpitr)->Disband(true);
                delete *grpitr;
            }
            delete itr->second;
        }
    }
}

void LfgMgr::Update(uint32 diff)
{
    //Update queues
    if (m_updateQueuesTimer <= diff)
        UpdateQueues(); // Timer will reset in UpdateQueues()
    else m_updateQueuesTimer -= diff;

    //Update formed groups
    if (m_updateProposalTimer <= diff)
    {
        UpdateFormedGroups(); 

        //Rolechecks
        for(GroupsList::iterator grpitr = rolecheckGroups.begin(); grpitr != rolecheckGroups.end(); ++grpitr)
            (*grpitr)->UpdateRoleCheck(diff);

        //Vote to kick
        GroupsList::iterator grpitr, grpitr_next;
        for(grpitr = voteKickGroups.begin(); grpitr != voteKickGroups.end(); grpitr = grpitr_next)
        {
            grpitr_next = grpitr;
            ++grpitr_next;
            if ((*grpitr)->UpdateVoteToKick(diff))
                voteKickGroups.erase(grpitr);
        }

        m_updateProposalTimer = LFG_TIMER_UPDATE_PROPOSAL;
    }
    else m_updateProposalTimer -= diff;

    //Delete invalid groups
  /*  if (m_deleteInvalidTimer <= diff)
    {
        for(GroupsList::iterator itr = groupsForDelete.begin(); itr != groupsForDelete.end(); ++itr)
        {
            (*itr)->Disband(true);
            delete *itr;
            groupsForDelete.erase(itr);
        }
        m_deleteInvalidTimer = LFG_TIMER_DELETE_INVALID_GROUPS;
    }else m_deleteInvalidTimer -= diff;*/

}

void LfgMgr::AddToQueue(Player *player, bool updateQueue)
{
    //ACE_Guard<ACE_Thread_Mutex> guard(m_queueLock);
    //Already checked that group is fine
    if (Group *group = player->GetGroup())
    {
        //TODO: group join to multiple dungeons
        if (!group->isLfgGroup())
        {
            LfgGroup* lfgGroup = new LfgGroup(true);
            Player *leader = sObjectMgr.GetPlayer(group->GetLeaderGUID());
            if (!leader || !leader->GetSession())
                return;
            LfgDungeonList::iterator itr = leader->m_lookingForGroup.queuedDungeons.begin();
            lfgGroup->SetDungeonInfo(*itr);                   
            lfgGroup->SetGroupId(sObjectMgr.GenerateGroupId()); 
            sObjectMgr.AddGroup(lfgGroup);

            for (GroupReference *grpitr = group->GetFirstMember(); grpitr != NULL; grpitr = grpitr->next())
            {
                Player *plr = grpitr->getSource();         
                if (!plr  || !plr ->GetSession())
                    continue;

                SendLfgUpdateParty(plr , LFG_UPDATETYPE_JOIN_PROPOSAL);
                LfgLog("Add member - join party not lfg");
                lfgGroup->AddMember(plr->GetGUID(), plr->GetName());
                lfgGroup->GetPremadePlayers()->insert(plr->GetGUID());
            }
            lfgGroup->SetLeader(group->GetLeaderGUID());

            lfgGroup->SendRoleCheckUpdate(LFG_ROLECHECK_INITIALITING);
            lfgGroup->UpdateRoleCheck();
            rolecheckGroups.insert(lfgGroup);
        }
        else
        {
            LfgGroup *lfgGroup = (LfgGroup*)group;
            if (!lfgGroup->IsInDungeon())
            {
                WorldPacket data(SMSG_LFG_JOIN_RESULT, 8);
                data << uint32(LFG_JOIN_INTERNAL_ERROR);                                  
                data << uint32(0);
                player->GetSession()->SendPacket(&data);
                return;
            }
            for (GroupReference *itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player *player = itr->getSource();         
                SendLfgUpdateParty(player, LFG_UPDATETYPE_JOIN_PROPOSAL);
            }

            lfgGroup->SendRoleCheckUpdate(LFG_ROLECHECK_INITIALITING);
            lfgGroup->UpdateRoleCheck();
            rolecheckGroups.insert(lfgGroup);
        }
    }
    else
    {
        WorldPacket data(SMSG_LFG_JOIN_RESULT, 8);
        data << uint32(LFG_JOIN_OK);                                  
        data << uint32(0);
        player->GetSession()->SendPacket(&data);

        SendLfgUpdatePlayer(player, LFG_UPDATETYPE_JOIN_PROPOSAL);
        
        for (LfgDungeonList::const_iterator it = player->m_lookingForGroup.queuedDungeons.begin(); it != player->m_lookingForGroup.queuedDungeons.end(); ++it)
        {
            if (IsPlayerInQueue(player->GetGUID(), (*it)->ID))
            {
                sLog.outError("LfgMgr: Player %s (GUID: %u) is already joined to queue for dungeon %u!", player->GetName(), player->GetGUID(), (*it)->ID);
                continue;
            }
            uint8 side = (player->GetTeam() == ALLIANCE) ? LFG_ALLIANCE : LFG_HORDE;
            
            if (m_queuedDungeons[side].find((*it)->ID) != m_queuedDungeons[side].end())
                m_queuedDungeons[side].find((*it)->ID)->second->players.insert(player->GetGUID());  //Insert player into queue, will be sorted on next queue update
            else  // None player is qeued into this dungeon
            {
                QueuedDungeonInfo *newDungeonQueue = new QueuedDungeonInfo();
                newDungeonQueue->dungeonInfo = *it;
                newDungeonQueue->players.insert(player->GetGUID());
                m_queuedDungeons[side].insert(std::pair<uint32, QueuedDungeonInfo*>((*it)->ID, newDungeonQueue));
                //fill some default data into wait times
                if (m_waitTimes[0].find((*it)->ID) == m_waitTimes[0].end())
                    for(int i = 0; i < LFG_WAIT_TIME_SLOT_MAX; ++i)
                        m_waitTimes[i].insert(std::make_pair<uint32, uint32>((*it)->ID, 0));
            }
        }
    }
    if (sWorld.getConfig(CONFIG_BOOL_LFG_IMMIDIATE_QUEUE_UPDATE) && updateQueue)
        UpdateQueues();
}

void LfgMgr::RemoveFromQueue(Player *player, bool updateQueue)
{
  //  ACE_Guard<ACE_Thread_Mutex> guard(m_queueLock);
    if (!player)
        return;
    if (Group *group = player->GetGroup())
    {
        for(Group::member_citerator citr = group->GetMemberSlots().begin(); citr != group->GetMemberSlots().end(); ++citr)
        {
            Player *plr = sObjectMgr.GetPlayer(citr->guid);
            if (!plr || !plr->GetSession())
                continue;

            uint8 side = (player->GetTeam() == ALLIANCE) ? LFG_ALLIANCE : LFG_HORDE;
            for (LfgDungeonList::const_iterator it = player->m_lookingForGroup.queuedDungeons.begin(); it != player->m_lookingForGroup.queuedDungeons.end(); ++it)
            {
                QueuedDungeonsMap::iterator itr = m_queuedDungeons[side].find((*it)->ID);
                if (itr == m_queuedDungeons[side].end())                 // THIS SHOULD NEVER HAPPEN
                    continue;

                if (itr->second->groups.find(player->m_lookingForGroup.groups.find((*it)->ID)->second) != itr->second->groups.end())
                {
                    GroupsList::iterator grpitr = itr->second->groups.find(player->m_lookingForGroup.groups.find((*it)->ID)->second);
                    if ((*grpitr)->IsMember(player->GetGUID()))
                    {
                        for(PlayerList::iterator plritr = (*grpitr)->GetPremadePlayers()->begin(); plritr != (*grpitr)->GetPremadePlayers()->end(); ++plritr)
                        {
                            if(!group->isLfgGroup())
                            {
                                LfgLog("Remove member - remove from queue if join as not lfg party");
                                (*grpitr)->RemoveMember(*plritr, 0);
                            }
                            Player *member = sObjectMgr.GetPlayer(*plritr);
                            if (member && member->GetSession())
                            {
                                SendLfgUpdateParty(member, LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
                                if(!group->isLfgGroup())
                                    member->m_lookingForGroup.groups.clear();
                                member->m_lookingForGroup.queuedDungeons.clear();
                            }
                        }
                        (*grpitr)->GetPremadePlayers()->clear();
                    }

                    if ((*grpitr)->GetMembersCount() == 0)
                    {
                        delete *grpitr;
                        itr->second->groups.erase(grpitr);
                    }
                }
                if (itr->second->groups.empty() && itr->second->players.empty())
                {
                    delete itr->second;
                    m_queuedDungeons[side].erase(itr);
                }
            }
        }
    }
    else
    {
        SendLfgUpdatePlayer(player, LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
        uint8 side = (player->GetTeam() == ALLIANCE) ? LFG_ALLIANCE : LFG_HORDE;
        for (LfgDungeonList::const_iterator it = player->m_lookingForGroup.queuedDungeons.begin(); it != player->m_lookingForGroup.queuedDungeons.end(); ++it)
        {
            QueuedDungeonsMap::iterator itr = m_queuedDungeons[side].find((*it)->ID);
            if (itr == m_queuedDungeons[side].end())                 // THIS SHOULD NEVER HAPPEN
                continue;
            itr->second->players.erase(player->GetGUID());
            if (itr->second->groups.find(player->m_lookingForGroup.groups.find((*it)->ID)->second) != itr->second->groups.end())
            {
                GroupsList::iterator grpitr = itr->second->groups.find(player->m_lookingForGroup.groups.find((*it)->ID)->second);
                if ((*grpitr)->IsMember(player->GetGUID()))
                {
                    LfgLog("Remove member - remove from queue single");
                    (*grpitr)->RemoveMember(player->GetGUID(), 0);
                }

                if ((*grpitr)->GetMembersCount() == 0)
                {
                    delete *grpitr;
                    itr->second->groups.erase(grpitr);
                }
            }
            if (itr->second->groups.empty() && itr->second->players.empty())
            {
                delete itr->second;
                m_queuedDungeons[side].erase(itr);
            }
        }
        player->m_lookingForGroup.queuedDungeons.clear();
    }
    if (sWorld.getConfig(CONFIG_BOOL_LFG_IMMIDIATE_QUEUE_UPDATE) && updateQueue)
        UpdateQueues();
}

void LfgMgr::AddCheckedGroup(LfgGroup *group, bool toQueue)
{
    rolecheckGroups.erase(group);
    if (!toQueue)
    {
        Player *leader = sObjectMgr.GetPlayer(group->GetLeaderGUID());
        if (!leader || !leader->GetSession())
            return;
        if(leader->GetGroup() && !leader->GetGroup()->isLfgGroup())
        {
           Group *baseGrp = leader->GetGroup();
           for(Group::member_citerator citr = baseGrp->GetMemberSlots().begin(); citr != baseGrp->GetMemberSlots().end(); ++citr)
           {
               LfgLog("Remove member - Add checked Group");
               group->RemoveMember(citr->guid, 0);
               Player *member = sObjectMgr.GetPlayer(citr->guid);
               if(member && member->IsInWorld())
                   member->m_lookingForGroup.queuedDungeons.clear();
           }
           AddGroupToDelete(group);
           DeleteGroups();
        }
        return; 
    }

    Player *player = sObjectMgr.GetPlayer(group->GetLeaderGUID());
    uint8 side = (player->GetTeam() == ALLIANCE) ? LFG_ALLIANCE : LFG_HORDE;
    uint32 dungId = group->GetDungeonInfo()->ID;
    if (group->IsRandom())
        dungId = (group->GetRandomEntry() & 0x00FFFFFF);
            
    if (m_queuedDungeons[side].find(dungId) != m_queuedDungeons[side].end())
        m_queuedDungeons[side].find(dungId)->second->groups.insert(group);  //Insert player into queue, will be sorted on next queue update
    else  // None player is qeued into this dungeon
    {
        LFGDungeonEntry const* entry = sLFGDungeonStore.LookupEntry(dungId);
        QueuedDungeonInfo *newDungeonQueue = new QueuedDungeonInfo();
        newDungeonQueue->dungeonInfo = entry;
        newDungeonQueue->groups.insert(group);
        m_queuedDungeons[side].insert(std::pair<uint32, QueuedDungeonInfo*>(dungId, newDungeonQueue));
        //fill some default data into wait times
        if (m_waitTimes[0].find(dungId) == m_waitTimes[0].end())
            for(int i = 0; i < LFG_WAIT_TIME_SLOT_MAX; ++i)
                m_waitTimes[i].insert(std::make_pair<uint32, uint32>(dungId, 0));
    }
    if (sWorld.getConfig(CONFIG_BOOL_LFG_IMMIDIATE_QUEUE_UPDATE))
        UpdateQueues();
}

void LfgMgr::UpdateQueues()
{
   // ACE_Guard<ACE_Thread_Mutex> guard(m_queueLock);
    for(int i = 0; i < MAX_LFG_FACTION; ++i)
    {
        if (m_queuedDungeons[i].empty())
            continue;
        //dungeons...
        for(QueuedDungeonsMap::iterator itr = m_queuedDungeons[i].begin(); itr != m_queuedDungeons[i].end(); ++itr)
        {
            //Remove somehow unaviable players
            for(GroupsList::iterator offitr = itr->second->groups.begin(); offitr != itr->second->groups.end(); ++offitr)
                (*offitr)->RemoveOfflinePlayers();
            DeleteGroups();

            //First, try to merge groups
            for(GroupsList::iterator grpitr1 = itr->second->groups.begin(); grpitr1 != itr->second->groups.end(); ++grpitr1)
            {
                //We can expect that there will be less tanks and healers than dps
                // grpitr1 = Group which gets new members 
                // grpitr2 = Group from which we take members
                GroupsList::iterator grpitr2, grpitr2next;
                for(grpitr2 = itr->second->groups.begin(); grpitr2 != itr->second->groups.end(); grpitr2 = grpitr2next)
                {
                    grpitr2next = grpitr2;
                    ++grpitr2next;
                    if ((*grpitr1) == (*grpitr2) || !(*grpitr1) || !(*grpitr2))
                        continue;
                    Group::member_citerator citr, citr_next;
                    for(citr = (*grpitr2)->GetMemberSlots().begin(); citr != (*grpitr2)->GetMemberSlots().end(); citr = citr_next)
                    {
                        citr_next = citr;
                        ++citr_next;
                        Player *plr = sObjectMgr.GetPlayer(citr->guid);
                        if (!plr || !plr->GetSession() || !plr->IsInWorld())
                            continue;
                        uint8 rolesCount = 0;
                        uint8 playerRoles = plr->m_lookingForGroup.roles;

                        if ((*grpitr2)->GetMembersCount() > (*grpitr1)->GetMembersCount() || !(*grpitr1)->HasCorrectLevel(plr->getLevel()) 
                            || (*grpitr2)->GetPremadePlayers()->find(plr->GetGUID()) != (*grpitr2)->GetPremadePlayers()->end())
                            continue;

                        uint8 checkRole = TANK;
                        uint8 merge = 0;  // 0 = nothin, 1 = just remove and add as same role, 2 sort roles
                        uint64 mergeGuid = 0;
                        uint8 mergeAs = 0;
                        for(int ii = 0; ii < 3; ++ii, checkRole *= 2)
                        {
                            if (!(playerRoles & checkRole))
                                continue;

                            for(int y = 0; y < 3; ++y)
                            {
                                merge = 0;
                                switch(checkRole)
                                {
                                    case TANK: mergeGuid = (*grpitr1)->GetTank(); break;
                                    case HEALER: mergeGuid = (*grpitr1)->GetHeal(); break;
                                    case DAMAGE:
                                        int z;
                                        PlayerList::iterator dps;
                                        for(z = 0, dps = (*grpitr1)->GetDps()->begin(); z < 3; ++z)
                                        {                                          
                                            if (y == z)
                                            {
                                                if (dps != (*grpitr1)->GetDps()->end())
                                                    mergeGuid = *dps;
                                                else
                                                    mergeGuid = 0;
                                                break;
                                            }
                                            if (dps != (*grpitr1)->GetDps()->end())
                                                ++dps;
                                        }
                                        break;       
                                }
                                if (mergeGuid == 0 && playerRoles-checkRole <= LEADER)
                                    merge = 1;
                                else if ((*grpitr1)->GetPlayerRole(mergeGuid, false, true) != checkRole)
                                {
                                    uint8 role = TANK;
                                    for(int iii = 0; iii < 3 && merge == 0; ++iii, role*=2)
                                    {
                                        if (role == checkRole)
                                            continue;
                                        if (!((*grpitr1)->GetPlayerRole(mergeGuid, false, true) & role))
                                            continue;

                                        if((*grpitr1)->HasFreeRole(role))
                                        {
                                            merge = 2;
                                            mergeAs = role;
                                            break;
                                       }
                                    }
                                }

                                if (merge == 0)
                                    continue;

                                uint64 guid = plr->GetGUID();
                                LfgLog("Remove member - Merge group");
                                (*grpitr2)->RemoveMember(guid, 0);
                                if ((*grpitr1)->IsMember(guid))
                                {
                                    sLog.outError("LfgMgr: Player %s (GUID %u) is being added to group %u twice! (merge %u, checkRole %u, mergeAs %u, mergeGuid %u, ii %u, y %u)", plr->GetName(), plr->GetGUID(), (*grpitr1)->GetId(), merge, checkRole, mergeAs, mergeGuid, ii, y);
                                    continue;
                                }
                                LfgLog("Add member - merge group");
                                (*grpitr1)->AddMember(guid, plr->GetName());
                                if (merge == 1)
                                {
                                    (*grpitr1)->SetAsRole(checkRole, guid);
                                    break;
                                }
                                else
                                {
                                    (*grpitr1)->GetDps()->erase(mergeGuid);
                                    (*grpitr1)->SetAsRole(mergeAs, mergeGuid);
                                    (*grpitr1)->SetAsRole(checkRole, guid);
                                    break;
                                }
                            }
                        }
                    }
                    //Delete empty groups
                    if ((*grpitr2)->GetMembersCount() == 0)
                    { 
                        delete *grpitr2;
                        itr->second->groups.erase(grpitr2);
                    }
                }
                //Now lets check if theres dmg which can be tank or healer...
                Group::member_citerator citr, citr_next;
                for(citr = (*grpitr1)->GetMemberSlots().begin(); citr != (*grpitr1)->GetMemberSlots().end() && (!(*grpitr1)->GetTank() || !(*grpitr1)->GetHeal()); citr = citr_next)
                {
                    citr_next = citr;
                    ++citr_next;
                    Player *plr = sObjectMgr.GetPlayer(citr->guid);
                    if (!plr || !plr->GetSession() || !plr->IsInWorld())
                        continue;

                    //We want only damage which can be tank or healer
                    uint8 role = (*grpitr1)->GetPlayerRole(citr->guid, false, true);
                    if((*grpitr1)->GetPlayerRole(citr->guid, false) != DAMAGE || role == DAMAGE)
                        continue;                  

                    if(role & TANK && !(*grpitr1)->GetTank())
                    {
                        (*grpitr1)->GetDps()->erase(citr->guid);
                        (*grpitr1)->SetTank(citr->guid);
                    }
                    else if(role & HEALER && !(*grpitr1)->GetHeal())
                    {
                        (*grpitr1)->GetDps()->erase(citr->guid);
                        (*grpitr1)->SetHeal(citr->guid);
                    }
                }
            }
            //Players in queue for that dungeon...
            for(PlayerList::iterator plritr = itr->second->players.begin(); plritr != itr->second->players.end(); ++plritr)
            {
                Player *player = sObjectMgr.GetPlayer(*plritr);
                if (!player || !player->GetSession())
                    continue;
                uint64 guid = *plritr;
                //Try to put him into group with most players
                LfgGroup *bigGrp = NULL;
                uint8 maxPlayers = 0;
                uint8 role = 0;
                for(GroupsList::iterator grpitr = itr->second->groups.begin(); grpitr != itr->second->groups.end(); ++grpitr)
                {
                    if (!(*grpitr)->HasCorrectLevel(player->getLevel()) // Check level, this is needed only for Classic and BC normal I think...
                        || maxPlayers >= (*grpitr)->GetMembersCount())   // We want group with most players
                        continue;

                    uint8 checkRole = TANK;
                    bool correct = false;
                    for(;checkRole <= DAMAGE && !correct; checkRole*=2)
                    {
                        if (!(player->m_lookingForGroup.roles & checkRole) // Player must have this role
                            || !(*grpitr)->HasFreeRole(checkRole))        // and role must be free
                            continue;
                        correct = true;
                        role = checkRole;
                    }
                    if(correct)
                    {
                        maxPlayers = (*grpitr)->GetMembersCount();
                        bigGrp = *grpitr;
                    }
                }
                if(role)
                {
                    LfgLog("Add member - update queue - to party");
                    if(!bigGrp->AddMember(guid, player->GetName()))
                        continue;
                    bigGrp->SetAsRole(role, guid);
                }
                //Failed, so create new LfgGroup
                else
                {
                    LfgGroup *newGroup = new LfgGroup();
                    newGroup->SetDungeonInfo(itr->second->dungeonInfo);
                    newGroup->SetGroupId(sObjectMgr.GenerateGroupId());
                    sObjectMgr.AddGroup(newGroup);
                    LfgLog("Add member - update queue - create new party");
                    if (!newGroup->AddMember(guid, player->GetName()))
                    {
                        delete newGroup;
                        continue;
                    }
                    for(role = TANK; role <= DAMAGE; role*=2)
                    {
                        if (player->m_lookingForGroup.roles & role)
                        {
                            newGroup->SetAsRole(role, guid);
                            break;
                        }
                    }                  
                    //Insert into queue
                    itr->second->groups.insert(newGroup);
                }
                //Player is in the group now
                itr->second->players.erase(plritr);
            }
            //Send update to everybody in queue and move complete groups to waiting state
            for(GroupsList::iterator grpitr = itr->second->groups.begin(); grpitr != itr->second->groups.end(); ++grpitr)
            {
                (*grpitr)->SendLfgQueueStatus();
                //prepare complete groups
                if ((*grpitr)->GetMembersCount() == 5)
                {
                    //Update wait times
                    uint32 avgWaitTime = 0;
                    uint8 timescount = 0;
                    if (Player *tank = sObjectMgr.GetPlayer((*grpitr)->GetTank()))
                    {
                        uint32 waitTimeTank = m_waitTimes[LFG_WAIT_TIME_TANK].find(itr->second->dungeonInfo->ID)->second;
                        uint32 currentTankTime = getMSTimeDiff(tank->m_lookingForGroup.joinTime, getMSTime());
                        uint32 avgWaitTank = (waitTimeTank+currentTankTime)/2;            
                        //add 5s cooldown
                        if (currentTankTime > 5000)
                        {
                            avgWaitTime += avgWaitTank;
                            m_waitTimes[LFG_WAIT_TIME_TANK].find(itr->second->dungeonInfo->ID)->second = avgWaitTank;
                            ++timescount;
                        }
                    }
                    if (Player *heal = sObjectMgr.GetPlayer((*grpitr)->GetHeal()))
                    {
                        uint32 waitTimeHeal = m_waitTimes[LFG_WAIT_TIME_HEAL].find(itr->second->dungeonInfo->ID)->second;
                        uint32 currentHealTime = getMSTimeDiff(heal->m_lookingForGroup.joinTime, getMSTime());
                        uint32 avgTimeHeal = (waitTimeHeal+currentHealTime)/2;      
                        //add 5s cooldown
                        if (currentHealTime > 5000)
                        {
                            avgWaitTime += avgTimeHeal;
                            m_waitTimes[LFG_WAIT_TIME_HEAL].find(itr->second->dungeonInfo->ID)->second = avgTimeHeal;
                            ++timescount;
                        }
                    }
                    
                    for(PlayerList::iterator plritr = (*grpitr)->GetDps()->begin(); plritr != (*grpitr)->GetDps()->end(); ++plritr)
                    {
                        if (Player *dps = sObjectMgr.GetPlayer(*plritr))
                        {
                            uint32 waitTimeDps = m_waitTimes[LFG_WAIT_TIME_DPS].find(itr->second->dungeonInfo->ID)->second;
                            uint32 currTime = getMSTimeDiff(dps->m_lookingForGroup.joinTime, getMSTime());
                            uint32 avgWaitDps = (waitTimeDps+currTime)/2;
                            if (currTime > 5000)
                            {
                                avgWaitTime += avgWaitDps;
                                m_waitTimes[LFG_WAIT_TIME_DPS].find(itr->second->dungeonInfo->ID)->second = avgWaitDps;
                                ++timescount;
                            }
                        }
                    }

                    if (timescount != 0)
                        m_waitTimes[LFG_WAIT_TIME].find(itr->second->dungeonInfo->ID)->second = (avgWaitTime/timescount);
                    
                    //Send Info                   
                    (*grpitr)->SendGroupFormed();
                    
                    formedGroups[i].insert(*grpitr);
                    itr->second->groups.erase(grpitr);

                    //Delete empty dungeon queues
                    if (itr->second->groups.empty() && itr->second->players.empty())
                    {
                        delete itr->second;
                        m_queuedDungeons[i].erase(itr);
                    }
                } 
            }
        }
    }
    m_updateQueuesTimer = m_updateQueuesBaseTime;
}
void LfgMgr::UpdateFormedGroups()
{
    GroupsList removeFromFormed;
    for(int i = 0; i < MAX_LFG_FACTION; ++i)
    {
        GroupsList::iterator grpitr, grpitr_next;
        for(grpitr = formedGroups[i].begin(); grpitr != formedGroups[i].end(); grpitr = grpitr_next)
        {
            grpitr_next = grpitr;
            ++grpitr_next;

             if((*grpitr)->RemoveOfflinePlayers())
                 LfgLog("Formed Group %u delete", (*grpitr)->GetId());
             else
                 LfgLog("Formed Group %u keep", (*grpitr)->GetId());
        }
        DeleteGroups();
        for(grpitr = formedGroups[i].begin(); grpitr != formedGroups[i].end(); grpitr = grpitr_next)
        {
            LfgLog("Formed Group %u", (*grpitr)->GetId());
            //this return false if  time has passed or player offline
            if (!(*grpitr)->UpdateCheckTimer(LFG_TIMER_UPDATE_PROPOSAL))
            {
                LfgLog("Formed Group %u - times up", (*grpitr)->GetId());
                (*grpitr)->SendProposalUpdate(LFG_PROPOSAL_FAILED);
                
                //Send to players..
                for(Group::member_citerator citr = (*grpitr)->GetMemberSlots().begin(); citr != (*grpitr)->GetMemberSlots().end(); ++citr)
                {
                    Player *member = sObjectMgr.GetPlayer(citr->guid);
                    if (!member || !member->GetSession() || member->GetGUID() == (*grpitr)->GetLeaderGUID())
                        continue;
                    ProposalAnswersMap::iterator itr = (*grpitr)->GetProposalAnswers()->find(member->GetGUID());
                    if ((itr == (*grpitr)->GetProposalAnswers()->end() || itr->second == 0)
                        && (*grpitr)->GetPremadePlayers()->find(citr->guid) == (*grpitr)->GetPremadePlayers()->end())
                    {
                        SendLfgUpdatePlayer(member, LFG_UPDATETYPE_PROPOSAL_FAILED);
                        member->m_lookingForGroup.queuedDungeons.clear();
                        LfgLog("Remove member - afk rolecheck");
                        (*grpitr)->RemoveMember(member->GetGUID(), 0);
                    }
                    else
                        SendLfgUpdatePlayer(member, LFG_UPDATETYPE_ADDED_TO_QUEUE);
                }
                //Move group to queue
                if (m_queuedDungeons[i].find((*grpitr)->GetDungeonInfo()->ID) != m_queuedDungeons[i].end())
                {
                    QueuedDungeonsMap::iterator itr = m_queuedDungeons[i].find((*grpitr)->GetDungeonInfo()->ID);
                    itr->second->groups.insert((*grpitr));
                }
                else
                {
                    QueuedDungeonInfo *newInfo = new QueuedDungeonInfo();
                    newInfo->dungeonInfo = (*grpitr)->GetDungeonInfo();
                    newInfo->groups.insert(*grpitr);
                    m_queuedDungeons[i].insert(std::pair<uint32, QueuedDungeonInfo*>(newInfo->dungeonInfo->ID, newInfo));
                }
                (*grpitr)->ResetGroup();
                removeFromFormed.insert(*grpitr);

                if (sWorld.getConfig(CONFIG_BOOL_LFG_IMMIDIATE_QUEUE_UPDATE))
                    UpdateQueues();
                continue;
            }
            //all player responded
            if ((*grpitr)->GetProposalAnswers()->size() == 5)
            {
                uint32 type = LFG_PROPOSAL_SUCCESS;
                for(ProposalAnswersMap::iterator itr = (*grpitr)->GetProposalAnswers()->begin(); itr != (*grpitr)->GetProposalAnswers()->end(); ++itr)
                    if (itr->second != 1)
                        type = LFG_PROPOSAL_FAILED;

                (*grpitr)->SendProposalUpdate(type);          
                for(ProposalAnswersMap::iterator itr = (*grpitr)->GetProposalAnswers()->begin(); itr != (*grpitr)->GetProposalAnswers()->end(); ++itr)
                {
                    if (itr->second != 1 && (*grpitr)->GetPremadePlayers()->find(itr->first) == (*grpitr)->GetPremadePlayers()->end())
                    {
                        uint64 guid = itr->first;
                        if (Player *plr = sObjectMgr.GetPlayer(guid))
                        {
                            SendLfgUpdatePlayer(plr, LFG_UPDATETYPE_PROPOSAL_DECLINED);
                            plr->m_lookingForGroup.queuedDungeons.clear();
                        }
                         LfgLog("Remove member - fail rolecheck");
                        (*grpitr)->RemoveMember(guid, 0);
                    }
                } 
                //Failed, remove players which did not agree and move rest to queue
                if (type == LFG_PROPOSAL_FAILED)
                {
                    LfgLog("Formed Group %u - failed", (*grpitr)->GetId());
                    if (m_queuedDungeons[i].find((*grpitr)->GetDungeonInfo()->ID) != m_queuedDungeons[i].end())
                    {
                        QueuedDungeonsMap::iterator itr = m_queuedDungeons[i].find((*grpitr)->GetDungeonInfo()->ID);
                        itr->second->groups.insert((*grpitr));
                    }
                    else
                    {
                        QueuedDungeonInfo *newInfo = new QueuedDungeonInfo();
                        newInfo->dungeonInfo = (*grpitr)->GetDungeonInfo();
                        newInfo->groups.insert(*grpitr);
                        m_queuedDungeons[i].insert(std::pair<uint32, QueuedDungeonInfo*>(newInfo->dungeonInfo->ID, newInfo));
                    }
                    for(Group::member_citerator citr = (*grpitr)->GetMemberSlots().begin(); citr != (*grpitr)->GetMemberSlots().end(); ++citr)
                    {
                        Player *member = sObjectMgr.GetPlayer(citr->guid);
                        if (!member || !member->GetSession() || member->GetGUID() == (*grpitr)->GetLeaderGUID())
                            continue;
                        SendLfgUpdatePlayer(member, LFG_UPDATETYPE_ADDED_TO_QUEUE);
                    }
                    (*grpitr)->ResetGroup();

                    removeFromFormed.insert(*grpitr);
                    if (sWorld.getConfig(CONFIG_BOOL_LFG_IMMIDIATE_QUEUE_UPDATE))
                        UpdateQueues();
                }
                //We are good to go, sir
                else
                {
                    LfgLog("Formed Group %u - go", (*grpitr)->GetId());
                    for(Group::member_citerator citr = (*grpitr)->GetMemberSlots().begin(); citr != (*grpitr)->GetMemberSlots().end(); ++citr)
                    {
                        Player *member = sObjectMgr.GetPlayer(citr->guid);
                        if (!member || !member->GetSession())
                            continue;
                        SendLfgUpdatePlayer(member, LFG_UPDATETYPE_GROUP_FOUND);
                        SendLfgUpdatePlayer(member, LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
                        
                        //I think this is useless when you are not in group, but its sent by retail servers anyway...
                        SendLfgUpdateParty(member, LFG_UPDATETYPE_REMOVED_FROM_QUEUE);       
                    }
                    (*grpitr)->TeleportToDungeon();
                    removeFromFormed.insert(*grpitr);
                }
            }
        }
        for(GroupsList::iterator itr = removeFromFormed.begin(); itr != removeFromFormed.end(); ++itr)
            formedGroups[i].erase(*itr);
        removeFromFormed.clear();
    }
}

void LfgMgr::SendLfgPlayerInfo(Player *plr)
{
    LfgDungeonList *random = GetRandomDungeons(plr);
    LfgLocksList *locks = GetDungeonsLock(plr);
    uint32 rsize = random->size();

    WorldPacket data(SMSG_LFG_PLAYER_INFO);
    if (rsize == 0)
        data << uint8(0);
    else
    {
        data << uint8(rsize);                                      // Random Dungeon count
        for (LfgDungeonList::iterator itr = random->begin(); itr != random->end(); ++itr)
        {
            data << uint32((*itr)->Entry());                       // Entry(ID and type) of random dungeon
            BuildRewardBlock(data, (*itr)->ID, plr);
        }
        random->clear();
        delete random;
    }
    data << uint32(locks->size());
    for (LfgLocksList::iterator itr = locks->begin(); itr != locks->end(); ++itr)
    {
        data << uint32((*itr)->dungeonInfo->Entry());              // Dungeon entry + type
        data << uint32((*itr)->lockType);                          // Lock status
    }
    delete locks;
    plr->GetSession()->SendPacket(&data);
}

void LfgMgr::SendLfgUpdatePlayer(Player *plr, uint8 updateType)
{
    if (!plr || !plr->GetSession() || !plr->IsInWorld())
        return;

    bool queued = false;
    bool extrainfo = false;

    switch(updateType)
    {
        case LFG_UPDATETYPE_JOIN_PROPOSAL:
        case LFG_UPDATETYPE_ADDED_TO_QUEUE:
            queued = true;
            extrainfo = true;
            break;
        //case LFG_UPDATETYPE_CLEAR_LOCK_LIST: // TODO: Sometimes has extrainfo - Check ocurrences...
        case LFG_UPDATETYPE_PROPOSAL_FOUND:
            extrainfo = true;
            break;
    }

    WorldPacket data(SMSG_LFG_UPDATE_PLAYER, 2 + (extrainfo ? 1 : 0) * (4 + plr->m_lookingForGroup.queuedDungeons.size() * 4 + plr->m_lookingForGroup.comment.length()));
    data << uint8(updateType);                              // Lfg Update type
    data << uint8(extrainfo);                               // Extra info
    if (extrainfo)
    {
        data << uint8(queued);                              // Join the queue
        data << uint8(0);                                   // unk - Always 0
        data << uint8(0);                                   // unk - Always 0

        data << uint8(plr->m_lookingForGroup.queuedDungeons.size());

        for (LfgDungeonList::const_iterator it = plr->m_lookingForGroup.queuedDungeons.begin(); it != plr->m_lookingForGroup.queuedDungeons.end(); ++it)
            data << uint32((*it)->Entry());
        data << plr->m_lookingForGroup.comment;
    }
    plr->GetSession()->SendPacket(&data);
}

void LfgMgr::SendLfgUpdateParty(Player *plr, uint8 updateType)
{
    if (!plr || !plr->GetSession() || !plr->IsInWorld())
        return;
    bool join = false;
    bool extrainfo = false;
    bool queued = false;

    switch(updateType)
    {
        case LFG_UPDATETYPE_JOIN_PROPOSAL:
            extrainfo = true;
            break;
        case LFG_UPDATETYPE_ADDED_TO_QUEUE:
            extrainfo = true;
            join = true;
            queued = true;
            break;
        case LFG_UPDATETYPE_CLEAR_LOCK_LIST:
            // join = true;  // TODO: Sometimes queued and extrainfo - Check ocurrences...
            queued = true;
            break;
        case LFG_UPDATETYPE_PROPOSAL_FOUND:
            extrainfo = true;
            join = true;
            break;
    }

    WorldPacket data(SMSG_LFG_UPDATE_PARTY, 2 + (extrainfo ? 1 : 0) * (8 + plr->m_lookingForGroup.queuedDungeons.size() * 4 + plr->m_lookingForGroup.comment.length()));
    data << uint8(updateType);                              // Lfg Update type
    data << uint8(extrainfo);                               // Extra info
    if (extrainfo)
    {
        data << uint8(join);                                // LFG Join
        data << uint8(queued);                              // Join the queue
        data << uint8(0);                                   // unk - Always 0
        data << uint8(0);                                   // unk - Always 0
        for (uint8 i = 0; i < 3; ++i)
            data << uint8(0);                               // unk - Always 0

        data << uint8(plr->m_lookingForGroup.queuedDungeons.size());

        for (LfgDungeonList::const_iterator it = plr->m_lookingForGroup.queuedDungeons.begin(); it != plr->m_lookingForGroup.queuedDungeons.end(); ++it)
            data << uint32((*it)->Entry());
        data << plr->m_lookingForGroup.comment;
    }
    plr->GetSession()->SendPacket(&data);
}

void LfgMgr::BuildRewardBlock(WorldPacket &data, uint32 dungeon, Player *plr)
{
    LfgReward *reward = GetDungeonReward(dungeon, plr->m_lookingForGroup.DoneDungeon(dungeon, plr), plr->getLevel());

    if (!reward)
        return;

    data << uint8(plr->m_lookingForGroup.DoneDungeon(dungeon, plr));  // false = its first run this day, true = it isnt
    if (data.GetOpcode() == SMSG_LFG_PLAYER_REWARD)
        data << uint32(0);             // ???
    data << uint32(reward->questInfo->GetRewOrReqMoney());
    data << uint32((plr->getLevel() == sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL)) ? 0 : reward->questInfo->XPValue( plr ));
    data << uint32(0);                                      // some "variable" money?
    data << uint32(0);                                      // some "variable" xp?
    
    ItemPrototype const *rewItem = sObjectMgr.GetItemPrototype(reward->questInfo->RewItemId[0]);   // Only first item is for dungeon finder
    if (!rewItem)
        data << uint8(0);   // have not reward item
    else
    {
        data << uint8(1);   // have reward item
        data << uint32(rewItem->ItemId);
        data << uint32(rewItem->DisplayInfoID);
        data << uint32(reward->questInfo->RewItemCount[0]);
    }
}

LfgReward* LfgMgr::GetDungeonReward(uint32 dungeon, bool done, uint8 level)
{
    LFGDungeonEntry const *dungeonInfo = sLFGDungeonStore.LookupEntry(dungeon);
    if (!dungeonInfo)
        return NULL;

    for(LfgRewardList::iterator itr = m_rewardsList.begin(); itr != m_rewardsList.end(); ++itr)
    {
        if ((*itr)->type == dungeonInfo->type && (*itr)->GroupType == dungeonInfo->grouptype &&
            (*itr)->isDaily() == done)
        {
            Quest *rewQuest = (*itr)->questInfo;
            if (level >= (*itr)->questInfo->GetMinLevel() &&  level <= (*itr)->questInfo->GetQuestLevel())  // ...mostly, needs some adjusting in db, blizz q level are without order
                return *itr;
        }
    }
    return NULL;
}

LfgDungeonList* LfgMgr::GetRandomDungeons(Player *plr)
{
    LfgDungeonList *dungeons = new LfgDungeonList();
    LFGDungeonEntry const *currentRow;
    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        currentRow = sLFGDungeonStore.LookupEntry(i);
        if (currentRow && currentRow->type == LFG_TYPE_RANDOM &&
            currentRow->minlevel <= plr->getLevel() && currentRow->maxlevel >= plr->getLevel() &&
            currentRow->expansion <= plr->GetSession()->Expansion())
            dungeons->insert(currentRow);
    }
    return dungeons;
}

LfgLocksList* LfgMgr::GetDungeonsLock(Player *plr)
{
    LfgLocksList* locks = new LfgLocksList();
    LFGDungeonEntry const *currentRow;
    LfgLockStatusType type;
    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        currentRow = sLFGDungeonStore.LookupEntry(i);
        if (!currentRow)
            continue;

        uint32 minlevel, maxlevel;
        //Take level from db where possible
        if (InstanceTemplate const *instance = sObjectMgr.GetInstanceTemplate(currentRow->map))
        {
            minlevel = instance->levelMin == 0 ? currentRow->minlevel : instance->levelMin;
            maxlevel = instance->levelMax == 0 ? currentRow->maxlevel : instance->levelMax;
        }
        else
        {
            minlevel = currentRow->minlevel;
            maxlevel = currentRow->maxlevel;
        }
        type = LFG_LOCKSTATUS_OK;
        DungeonInfoMap::iterator itr = m_dungeonInfoMap.find(currentRow->ID);
        InstancePlayerBind *playerBind = plr->GetBoundInstance(currentRow->map, Difficulty(currentRow->heroic));

        if (currentRow->expansion > plr->GetSession()->Expansion())
            type = LFG_LOCKSTATUS_INSUFFICIENT_EXPANSION;
        else if (minlevel > plr->getLevel())
            type = LFG_LOCKSTATUS_TOO_LOW_LEVEL;
        else if (plr->getLevel() > maxlevel)
            type = LFG_LOCKSTATUS_TOO_HIGH_LEVEL;
        else if ((playerBind && playerBind->perm) || (itr != m_dungeonInfoMap.end() && itr->second->locked) || itr == m_dungeonInfoMap.end())
            type = LFG_LOCKSTATUS_RAID_LOCKED;
        else if (AreaTrigger const* at = sObjectMgr.GetMapEntranceTrigger(currentRow->map))
        {
            uint32 quest_id = currentRow->isHeroic() ? at->requiredQuestHeroic : at->requiredQuest;
            uint32 itemId1 = currentRow->isHeroic() ? at->heroicKey : at->requiredItem;
            uint32 itemId2 = currentRow->isHeroic() ? at->heroicKey2 : at->requiredItem2;
            if (quest_id && !plr->GetQuestRewardStatus(quest_id))
                type = LFG_LOCKSTATUS_QUEST_NOT_COMPLETED; 
            else if ((itemId1 && !plr->HasItemCount(itemId1, 1)) || (itemId2 && !plr->HasItemCount(itemId2, 1)))
                type = LFG_LOCKSTATUS_MISSING_ITEM;
        }
        //others to be done

        if (type != LFG_LOCKSTATUS_OK)
        {
            LfgLockStatus *lockStatus = new LfgLockStatus();
            lockStatus->dungeonInfo = currentRow;
            lockStatus->lockType = type;
            locks->push_back(lockStatus);
        } 
    }
    return locks;
}

/*
CREATE TABLE `quest_lfg_relation` (
`type` TINYINT( 3 ) UNSIGNED NOT NULL DEFAULT '0',
`groupType` TINYINT( 3 ) UNSIGNED NOT NULL DEFAULT '0',
`questEntry` INT( 11 ) UNSIGNED NOT NULL DEFAULT '0',
`flags` INT( 11 ) UNSIGNED NOT NULL DEFAULT '0',
INDEX ( `type` , `groupType` ) ,
UNIQUE (`questEntry`)
) ENGINE = InnoDB;
*/

void LfgMgr::LoadDungeonRewards()
{
    // In case of reload
    for(LfgRewardList::iterator itr = m_rewardsList.begin(); itr != m_rewardsList.end(); ++itr)
    {
        delete (*itr)->questInfo;
        delete *itr; 
    }
    m_rewardsList.clear();

    uint32 count = 0;
    //                                                0     1          2           3       
    QueryResult *result = WorldDatabase.Query("SELECT type, groupType, questEntry, flags FROM quest_lfg_relation");

    if ( !result )
    {
        barGoLink bar( 1 );

        bar.step();

        sLog.outString();
        sLog.outString( ">> Loaded %u LFG dungeon quest relations", count );
        return;
    }

    barGoLink bar( (int)result->GetRowCount() );

    do
    {
        Field *fields = result->Fetch();

        bar.step();
        
        LfgReward *reward = new LfgReward();
        reward->type                  = fields[0].GetUInt8();
        reward->GroupType             = fields[1].GetUInt8();
        reward->flags                 = fields[3].GetUInt32();

        if (Quest *rewardQuest = const_cast<Quest*>(sObjectMgr.GetQuestTemplate(fields[2].GetUInt32())))
            reward->questInfo = rewardQuest;
        else
        {
            sLog.outErrorDb("Entry listed in 'quest_lfg_relation' has non-exist quest %u, skipping.", fields[2].GetUInt32());
            delete reward;
            continue;
        }
        m_rewardsList.push_back(reward);
        ++count;
    } while( result->NextRow() );

    delete result;

    sLog.outString();
    sLog.outString( ">> Loaded %u LFG dungeon quest relations.", count );
}
/*
CREATE TABLE IF NOT EXISTS `lfg_dungeon_info` (
  `ID` mediumint(8) NOT NULL DEFAULT '0' COMMENT 'ID from LfgDugeons.dbc',
  `name` text,
  `lastBossId` int(11) NOT NULL DEFAULT '0' COMMENT 'Entry from creature_template',
  `start_map` mediumint(8) NOT NULL DEFAULT '0',
  `start_x` float NOT NULL DEFAULT '0',
  `start_y` float NOT NULL DEFAULT '0',
  `start_z` float NOT NULL DEFAULT '0',
  `start_o` int(11) NOT NULL,
  `locked` tinyint(3) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
*/
void LfgMgr::LoadDungeonsInfo()
{
    // In case of reload
    for(DungeonInfoMap::iterator itr = m_dungeonInfoMap.begin(); itr != m_dungeonInfoMap.end(); ++itr)
        delete itr->second;
    m_dungeonInfoMap.clear();

    //Fill locked for dungeons without info in db
    LFGDungeonEntry const *currentRow;
    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        currentRow = sLFGDungeonStore.LookupEntry(i);
        if (!currentRow)
            continue;
        DungeonInfo *info = new DungeonInfo();
        info->ID = currentRow->ID;
        info->locked = true;
        m_dungeonInfoMap.insert(std::make_pair<uint32, DungeonInfo*>(info->ID, info));
    }
    uint32 count = 0;
    //                                                0   1     2           3          4        5        6        7        8
    QueryResult *result = WorldDatabase.Query("SELECT ID, name, lastBossId, start_map, start_x, start_y, start_z, start_o, locked  FROM lfg_dungeon_info");

    if ( !result )
    {
        barGoLink bar( 1 );
        bar.step();
        sLog.outString();
        sLog.outString( ">> Loaded %u LFG dungeon info entries.", count );
        return;
    }
    barGoLink bar( (int)result->GetRowCount() );
    do
    {
        Field *fields = result->Fetch();

        bar.step();
        
        DungeonInfo *info = new DungeonInfo();
        info->ID                      = fields[0].GetUInt32();
        info->name                    = fields[1].GetCppString();
        info->lastBossId              = fields[2].GetUInt32();
        info->start_map               = fields[3].GetUInt32();
        info->start_x                 = fields[4].GetFloat();
        info->start_y                 = fields[5].GetFloat();
        info->start_z                 = fields[6].GetFloat();
        info->start_o                 = fields[7].GetFloat();
        info->locked                  = fields[8].GetBool();
       
        if (!sLFGDungeonStore.LookupEntry(info->ID))
        {
            sLog.outErrorDb("Entry listed in 'lfg_dungeon_info' has non-exist LfgDungeon.dbc id %u, skipping.", info->ID);
            delete info;
            continue;
        }
        if (!sObjectMgr.GetCreatureTemplate(info->lastBossId) && info->lastBossId != 0)
        {
            sLog.outErrorDb("Entry listed in 'lfg_dungeon_info' has non-exist creature_template entry %u, skipping.", info->lastBossId);
            delete info;
            continue;   
        }
        m_dungeonInfoMap.find(info->ID)->second = info;
        ++count;
    } while( result->NextRow() );

    delete result;

    sLog.outString();
    sLog.outString( ">> Loaded %u LFG dungeon info entries.", count );
}
uint32 LfgMgr::GetAvgWaitTime(uint32 dugeonId, uint8 slot, uint8 roles)
{
    switch(slot)
    {
        case LFG_WAIT_TIME:
        case LFG_WAIT_TIME_TANK:
        case LFG_WAIT_TIME_HEAL:
        case LFG_WAIT_TIME_DPS:
            return (m_waitTimes[slot].find(dugeonId)->second / 1000);  // No check required, if this method is called, some data is already in array
        case LFG_WAIT_TIME_AVG:
        {
            if (roles & TANK)
            {
                if (!(roles & HEALER) && !(roles & DAMAGE))
                    return (m_waitTimes[LFG_WAIT_TIME_TANK].find(dugeonId)->second / 1000);
            }
            else if (roles & HEALER)
            {
                if (!(roles & DAMAGE))
                    return (m_waitTimes[LFG_WAIT_TIME_HEAL].find(dugeonId)->second / 1000);
            }
            else if (roles & DAMAGE)
                return (m_waitTimes[LFG_WAIT_TIME_DPS].find(dugeonId)->second / 1000);
            return (m_waitTimes[LFG_WAIT_TIME].find(dugeonId)->second / 1000);
        }
    }
}

void LfgMgr::DeleteGroups()
{
    for(GroupsList::iterator group = groupsForDelete.begin(); group != groupsForDelete.end(); ++group)
    {
        for(int i = 0; i < MAX_LFG_FACTION; ++i)
        {
            for(QueuedDungeonsMap::iterator itr = m_queuedDungeons[i].begin(); itr != m_queuedDungeons[i].end(); ++itr)
                itr->second->groups.erase(*group);

            formedGroups[i].erase(*group);
        }
        (*group)->Disband(true);
        delete *group;
    }
    groupsForDelete.clear();
}

void LfgMgr::RemovePlayer(Player *player)
{
    for(int i = 0; i < MAX_LFG_FACTION; ++i)
    {
        for(GroupsList::iterator itr = formedGroups[i].begin(); itr != formedGroups[i].end(); ++itr)
        {
            if ((*itr)->IsMember(player->GetGUID()) && (*itr)->GetGroupType() & GROUPTYPE_LFD_1)
            {
                LfgLog("Remove member - LfgMgr::RemovePlayer");
                (*itr)->RemoveMember(player->GetGUID(), 0);
            }

            if ((*itr)->GetMembersCount() == 0)
                AddGroupToDelete(*itr);
        }
    }
    if (!player->m_lookingForGroup.queuedDungeons.empty())
        RemoveFromQueue(player);
}

bool LfgMgr::IsPlayerInQueue(uint64 guid, uint32 id)
{
    Player *player = sObjectMgr.GetPlayer(guid);
    if (!player || !player->GetSession())
        return false;

    uint8 side = (player->GetTeam() == ALLIANCE) ? LFG_ALLIANCE : LFG_HORDE;

    if (m_queuedDungeons[side].find(id) != m_queuedDungeons[side].end())
    {
        if (m_queuedDungeons[side].find(id)->second->players.find(guid) != m_queuedDungeons[side].find(id)->second->players.end())
            return true;
        for(GroupsList::iterator itr = m_queuedDungeons[side].find(id)->second->groups.begin(); itr != m_queuedDungeons[side].find(id)->second->groups.end(); ++itr)
        {
            if ((*itr)->IsMember(guid))
                return true;
        }
    }
    return false;
}