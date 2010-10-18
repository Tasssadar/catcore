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
    for(uint8 i = 0; i < MAX_LFG_FACTION; ++i)
        m_updateQueuesTimer[i] = m_updateQueuesBaseTime;
    m_updateProposalTimer = LFG_TIMER_UPDATE_PROPOSAL;
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
    for(uint8 i = 0; i < MAX_LFG_FACTION; ++i)
    {
        if (m_updateQueuesTimer[i] <= diff)
            UpdateQueue(i); // Timer will reset in UpdateQueue()
        else m_updateQueuesTimer[i] -= diff;
    }

    //Update formed groups
    if (m_updateProposalTimer <= diff)
    {
        UpdateFormedGroups(); 

        GroupsList::iterator grpitr, grpitr_next;
        //Rolechecks
        GroupsList tmpGroupList = rolecheckGroups;
        for(grpitr = tmpGroupList.begin(); grpitr != tmpGroupList.end(); grpitr = grpitr_next)
        {
            grpitr_next = grpitr;
            ++grpitr_next;
            //TODO: Find out why this is happening
            if(!(*grpitr)->isLfgGroup())
            {
                rolecheckGroups.erase(*grpitr);
                continue;
            }
            (*grpitr)->UpdateRoleCheck(diff);
        }
        DeleteGroups();

        //Vote to kick
        tmpGroupList.clear();
        tmpGroupList = voteKickGroups;
        for(grpitr = tmpGroupList.begin(); grpitr != tmpGroupList.end(); grpitr = grpitr_next)
        {
            grpitr_next = grpitr;
            ++grpitr_next;
            if (!(*grpitr)->isLfgGroup() || (*grpitr)->UpdateVoteToKick(diff))
                voteKickGroups.erase(*grpitr);
        }
        tmpGroupList.clear();

        m_updateProposalTimer = LFG_TIMER_UPDATE_PROPOSAL;
    }
    else m_updateProposalTimer -= diff;
}

void LfgMgr::AddToQueue(Player *player, bool updateQueue)
{
    //Already checked that group is fine
    //TODO: join to multiple dungeons
    uint8 side = GetSideForPlayer(player);
    if (Group *group = player->GetGroup())
    {
        LfgGroup* lfgGroup = NULL;
        if(group->isLfgGroup())
        {
            lfgGroup = (LfgGroup*)group;
            LfgLog("lfg group join dung %u, groupid %u", lfgGroup->GetDungeonInfo()->ID, lfgGroup->GetId());
        }
        else
        {
            lfgGroup = new LfgGroup(true, (side == LFG_MIXED));
            Player *leader = sObjectMgr.GetPlayer(group->GetLeaderGUID());
            if (!leader || !leader->GetSession())
                return;
            LfgDungeonList::iterator itr = leader->m_lookingForGroup.queuedDungeons.begin();
            lfgGroup->SetDungeonInfo(*itr);                   
            lfgGroup->SetGroupId(sObjectMgr.GenerateGroupId()); 
            sObjectMgr.AddGroup(lfgGroup);
        }       

        for (GroupReference *grpitr = group->GetFirstMember(); grpitr != NULL; grpitr = grpitr->next())
        {
            Player *plr = grpitr->getSource();         
            if (!plr  || !plr ->GetSession())
                continue;

            SendLfgUpdateParty(plr , LFG_UPDATETYPE_JOIN_PROPOSAL);
            LfgLog("Add member - join party not lfg");
            if (!group->isLfgGroup())
            {
                lfgGroup->AddMember(plr->GetGUID(), plr->GetName());
                lfgGroup->GetPremadePlayers()->insert(plr->GetGUID());
            }
        }
        if (!group->isLfgGroup())
            lfgGroup->SetLeader(group->GetLeaderGUID());
        else
            lfgGroup->AddLfgFlag(LFG_GRP_BONUS);

        lfgGroup->SendRoleCheckUpdate(LFG_ROLECHECK_INITIALITING);
        lfgGroup->UpdateRoleCheck();
        rolecheckGroups.insert(lfgGroup);
    }
    else
    {
        if(IsPlayerInQueue(player->GetGUID()))
        {
            SendJoinResult(player, LFG_JOIN_INTERNAL_ERROR);
            LfgLog("Player %u cant join lfg because he is queued in %u!", player->GetGUID(), IsPlayerInQueue(player->GetGUID()));
            return;
        }

        SendJoinResult(player, LFG_JOIN_OK);
        SendLfgUpdatePlayer(player, LFG_UPDATETYPE_JOIN_PROPOSAL);
        
        for (LfgDungeonList::const_iterator it = player->m_lookingForGroup.queuedDungeons.begin(); it != player->m_lookingForGroup.queuedDungeons.end(); ++it)
        {
            QueuedDungeonsMap::iterator queue = GetOrCreateQueueEntry(*it, side);
            queue->second->players.insert(player->GetGUID());
            m_queuedPlayers.insert(std::make_pair<uint64, uint32>(player->GetGUID(), (*it)->ID));
        }
    }
    if (sWorld.getConfig(CONFIG_BOOL_LFG_IMMIDIATE_QUEUE_UPDATE) && updateQueue)
        UpdateQueue(side);
}

void LfgMgr::RemoveFromQueue(Player *player, bool updateQueue)
{
  //  ACE_Guard<ACE_Thread_Mutex> guard(m_queueLock);
    if (!player)
        return;
    uint8 side = GetSideForPlayer(player);
    if (Group *group = player->GetGroup())
    {
        LfgDungeonList queued = player->m_lookingForGroup.queuedDungeons;
        for (LfgDungeonList::const_iterator it = queued.begin(); it != queued.end(); ++it)
        {
            QueuedDungeonsMap::iterator itr = m_queuedDungeons[side].find((*it)->ID);
            if (itr == m_queuedDungeons[side].end())                 // THIS SHOULD NEVER HAPPEN
                continue;

            GroupMap::iterator grp = player->m_lookingForGroup.groups.find((*it)->ID);
            if(grp == player->m_lookingForGroup.groups.end())
                continue;
            LfgGroup *lfgGroup = sObjectMgr.GetLfgGroupById(grp->second);
            if(!lfgGroup)
                continue;

            LfgLog("Remove group %u from queue, id %u", lfgGroup->GetId(), grp->first);
            uint64 guid = 0;
            for(PlayerList::iterator plritr = lfgGroup->GetPremadePlayers()->begin(); plritr != lfgGroup->GetPremadePlayers()->end(); ++plritr)
            {
                guid = *plritr;
                if(!group->isLfgGroup())
                {
                    LfgLog("Remove member - remove from queue if join as not lfg party");
                    lfgGroup->RemoveMember(guid, 0);
                    m_queuedPlayers.erase(guid);
                }
                Player *member = sObjectMgr.GetPlayer(guid);
                if (member && member->GetSession())
                {
                    SendLfgUpdateParty(member, LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
                    SendLfgUpdatePlayer(member, LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
                    if(!group->isLfgGroup())
                        member->m_lookingForGroup.groups.clear();
                    member->m_lookingForGroup.queuedDungeons.clear();
                }
            }
            Group::member_citerator mitr, mitr_next;
            for(mitr = lfgGroup->GetMemberSlots().begin(); mitr != lfgGroup->GetMemberSlots().end(); mitr = mitr_next)
            {
                mitr_next = mitr;
                ++mitr_next;
                if(lfgGroup->GetPremadePlayers()->find(mitr->guid) != lfgGroup->GetPremadePlayers()->end())
                    continue;

                itr->second->players.insert(mitr->guid);
                lfgGroup->RemoveMember(mitr->guid, 0);           
            }
            lfgGroup->GetPremadePlayers()->clear();

            if (lfgGroup->GetMembersCount() == 0)
            {
                lfgGroup->Disband(true);
                itr->second->groups.erase(lfgGroup);
                formedGroups[side].erase(lfgGroup);
                sObjectMgr.RemoveGroup(lfgGroup);
                delete lfgGroup;
            }
            if (itr->second->groups.empty() && itr->second->players.empty())
            {
                delete itr->second;
                m_queuedDungeons[side].erase(itr);
            }
        }
    }
    else
    {
        SendLfgUpdatePlayer(player, LFG_UPDATETYPE_REMOVED_FROM_QUEUE); 
        for (LfgDungeonList::const_iterator it = player->m_lookingForGroup.queuedDungeons.begin(); it != player->m_lookingForGroup.queuedDungeons.end(); ++it)
        {
            QueuedDungeonsMap::iterator itr = m_queuedDungeons[side].find((*it)->ID);
            if (itr == m_queuedDungeons[side].end())                 // THIS SHOULD NEVER HAPPEN
                continue;
            itr->second->players.erase(player->GetGUID());
            GroupMap::iterator grp = player->m_lookingForGroup.groups.find((*it)->ID);
            if(grp == player->m_lookingForGroup.groups.end())
                continue;
            LfgGroup *group = sObjectMgr.GetLfgGroupById(grp->second);
            if(!group)
                continue;
            GroupsList::iterator grpitr = itr->second->groups.find(group);
            if (grpitr != itr->second->groups.end())
            {
                if ((*grpitr)->IsMember(player->GetGUID()))
                {
                    LfgLog("Remove member - remove from queue single");
                    (*grpitr)->RemoveMember(player->GetGUID(), 0);
                    m_queuedPlayers.erase(player->GetGUID());
                }

                if ((*grpitr)->GetMembersCount() == 0)
                {
                    sObjectMgr.RemoveGroup(*grpitr);
                    itr->second->groups.erase(*grpitr);
                    delete *grpitr;
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
        UpdateQueue(side);
}

void LfgMgr::AddCheckedGroup(LfgGroup *group, bool toQueue)
{
    rolecheckGroups.erase(group);
    if (!toQueue)
    {
        LfgLog("Group %u UpdateRoleCheck fail", group->GetId());
        Player *leader = NULL;

        for(Group::member_citerator citr = group->GetMemberSlots().begin(); citr != group->GetMemberSlots().end(); ++citr)
        {
            leader = sObjectMgr.GetPlayer(citr->guid);
            if (leader && leader->GetSession() && leader->GetGroup() && !leader->GetGroup()->isLfgGroup())
                break;
        }
        if (!leader || !leader->GetSession() || !leader->GetGroup() || leader->GetGroup()->isLfgGroup())
            return;

        Group *baseGrp = leader->GetGroup();
        for(Group::member_citerator citr = baseGrp->GetMemberSlots().begin(); citr != baseGrp->GetMemberSlots().end(); ++citr)
        {
            LfgLog("Remove member %u from group %u- Add checked Group", citr->guid, group->GetId());
            group->RemoveMember(citr->guid, 0);
            Player *member = sObjectMgr.GetPlayer(citr->guid);
            if(member && member->IsInWorld())
                member->m_lookingForGroup.queuedDungeons.clear();
        }
        AddGroupToDelete(group);
        return; 
    }

    Player *player = sObjectMgr.GetPlayer(group->GetLeaderGUID());
    uint8 side = GetSideForPlayer(player);

    MoveGroupToQueue(group, side, group->GetDungeonInfo(group->IsRandom())->ID);

    if (sWorld.getConfig(CONFIG_BOOL_LFG_IMMIDIATE_QUEUE_UPDATE))
        UpdateQueue(side);
}

void LfgMgr::UpdateQueue(uint8 side)
{
    if (m_queuedDungeons[side].empty())
    {
        m_updateQueuesTimer[side] = m_updateQueuesBaseTime;
        return;
    }
    //dungeons...
    for(QueuedDungeonsMap::iterator itr = m_queuedDungeons[side].begin(); itr != m_queuedDungeons[side].end(); ++itr)
    {
        //Remove somehow unaviable players
        LfgLog("Remove offline - update");
        for(GroupsList::iterator offitr = itr->second->groups.begin(); offitr != itr->second->groups.end(); ++offitr)
            (*offitr)->RemoveOfflinePlayers();
        DeleteGroups();

        //Try to merge groups
        MergeGroups(&itr->second->groups, itr->second->dungeonInfo, side);
        
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
            uint8 checkRole = TANK;
            bool correct = false;
            for(GroupsList::iterator grpitr = itr->second->groups.begin(); grpitr != itr->second->groups.end(); ++grpitr)
            {
                if (!(*grpitr)->HasCorrectLevel(player->getLevel()) // Check level, this is needed only for Classic and BC normal I think...
                    || maxPlayers >= (*grpitr)->GetMembersCount() || (*grpitr)->GetMembersCount() >= LFG_GROUP)   // We want group with most players
                    continue;

                checkRole = TANK;
                correct = false;
                for(; checkRole <= DAMAGE && !correct; checkRole*=2)
                {
                    if (!(player->m_lookingForGroup.roles & checkRole) // Player must have this role
                        || !(*grpitr)->HasFreeRole(checkRole))        // and role must be free
                        continue;
                    correct = true;
                    role = checkRole;
                }
                if(correct)
                {
                    maxPlayers = (*grpitr)->GetMembersCount() + (*grpitr)->HasBonus();
                    bigGrp = *grpitr;
                }
            }
            if(role)
            {
                LfgLog("Add member - update queue - to party, role: %u ", role);
                if(!bigGrp->AddMember(guid, player->GetName()))
                    continue;
                bigGrp->SetAsRole(role, guid);
            }
            //Failed, so create new LfgGroup
            else
            {
                LfgGroup *newGroup = new LfgGroup(false, true);
                newGroup->SetDungeonInfo(itr->second->dungeonInfo);
                newGroup->SetGroupId(sObjectMgr.GenerateGroupId());
                sObjectMgr.AddGroup(newGroup);
                if (!newGroup->AddMember(guid, player->GetName()))
                {
                    sObjectMgr.RemoveGroup(newGroup);
                    delete newGroup;                 
                    continue;
                }
                for(role = TANK; role <= DAMAGE; role*=2)
                {
                    if (player->m_lookingForGroup.roles & role)
                    {
                        LfgLog("Add member - update queue - create new party", role);
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
            if ((*grpitr)->GetMembersCount() == LFG_GROUP)
            {
                //Update wait time
                UpdateWaitTime(*grpitr, itr->second->dungeonInfo->ID);

                for(Group::member_citerator citr = (*grpitr)->GetMemberSlots().begin(); citr != (*grpitr)->GetMemberSlots().end(); ++citr)
                    m_queuedPlayers.erase(citr->guid);

                //Send Info                   
                (*grpitr)->SendGroupFormed();
                
                formedGroups[side].insert(*grpitr);
                itr->second->groups.erase(grpitr);

                //Delete empty dungeon queues
                if (itr->second->groups.empty() && itr->second->players.empty())
                {
                    delete itr->second;
                    m_queuedDungeons[side].erase(itr);
                }
            } 
        }
    }
    m_updateQueuesTimer[side] = m_updateQueuesBaseTime;
}

void LfgMgr::MergeGroups(GroupsList *groups, LFGDungeonEntry const *info, uint8 side)
{
    for(GroupsList::iterator grpitr1 = groups->begin(); grpitr1 != groups->end(); ++grpitr1)
    {
        // We can expect that there will be less tanks and healers than dps
        // grpitr1 = Group which gets new members 
        // grpitr2 = Group from which we take members
        GroupsList::iterator grpitr2, grpitr2next;
        for(grpitr2 = groups->begin(); grpitr2 != groups->end(); grpitr2 = grpitr2next)
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
                uint8 playerRoles = (*grpitr2)->GetPlayerRole(citr->guid, false, true);

                if ((*grpitr2)->GetMembersCount() > (*grpitr1)->GetMembersCount() || !(*grpitr1)->HasCorrectLevel(plr->getLevel()) 
                    || (*grpitr2)->GetPremadePlayers()->find(plr->GetGUID()) != (*grpitr2)->GetPremadePlayers()->end())
                    continue;

                uint8 checkRole = TANK;
                uint8 merge = 0;  // 0 = nothin, 1 = just remove and add as same role, 2 sort roles
                uint64 mergeGuid = 0;
                uint8 mergeAs = 0;
                for(int ii = 0; ii < 3 && merge == 0; ++ii, checkRole *= 2)
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
                        if ((*grpitr1)->HasFreeRole(checkRole) && playerRoles == checkRole)
                            merge = 1;
                        else if ((*grpitr1)->GetPlayerRole(mergeGuid, false, true) != checkRole)
                        {
                            for(uint8 role = TANK; role <= DAMAGE && merge == 0; role*=2)
                            {
                                if (role == checkRole || !((*grpitr1)->GetPlayerRole(mergeGuid, false, true) & role))
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
                            LfgLog("Set role merge 1 to %u", checkRole);
                            (*grpitr1)->SetAsRole(checkRole, guid);
                            break;
                        }
                        else
                        {
                            if((*grpitr1)->GetPlayerRole(mergeGuid, false) & DAMAGE)
                                (*grpitr1)->GetDps()->erase(mergeGuid);
                            LfgLog("Set role merge 2 to %u", mergeAs);
                            (*grpitr1)->SetAsRole(mergeAs, mergeGuid);
                            LfgLog("Set role merge 2 to %u", checkRole);
                            (*grpitr1)->SetAsRole(checkRole, guid);
                            break;
                        }
                    }
                }
            }
            //Delete empty groups
            if ((*grpitr2)->GetMembersCount() == 0)
            { 
                (*grpitr2)->Disband(true);
                sObjectMgr.RemoveGroup(*grpitr2);
                groups->erase(*grpitr2);
                delete *grpitr2;
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
    //Try to merge specific with randoms
    if(!info->isRandom())
        return;

    LfgDungeonList *options = GetRandomOptions(info->ID);
    QueuedDungeonsMap::iterator queue;
    GroupsList toRemove;  // We gonna need to delete some empty groups
    for(LfgDungeonList::iterator itr = options->begin(); itr != options->end(); ++itr)
    {
        queue = m_queuedDungeons[side].find((*itr)->ID);
        if(queue == m_queuedDungeons[side].end() || queue->second->groups.empty())
            continue;

        for(GroupsList::iterator grpitr1 = groups->begin(); grpitr1 != groups->end(); ++grpitr1)
        {
            if((*grpitr1)->GetMembersCount() >= LFG_GROUP)
                continue;
            for(GroupsList::iterator grpitr2 = queue->second->groups.begin(); grpitr2 != queue->second->groups.end(); ++grpitr2)
            {
                if((*grpitr1)->GetMembersCount() + (*grpitr2)->GetMembersCount() != LFG_GROUP || 
                    (*grpitr2)->GetMembersCount() >= LFG_GROUP)
                    continue;

                ProposalAnswersMap canMove;
                LfgLog("RANDOM <-> SPECIFIC merge group %u with specific %u in dung %u", (*grpitr1)->GetId(), (*grpitr2)->GetId(), (*itr)->ID);
               
                for(Group::member_citerator citr = (*grpitr1)->GetMemberSlots().begin(); citr != (*grpitr1)->GetMemberSlots().end(); ++citr)
                {
                    Player *plr = sObjectMgr.GetPlayer(citr->guid);
                    if (!plr || !plr->GetSession() || !plr->IsInWorld())
                        continue;

                    if(canMove.size() + (*grpitr2)->GetMembersCount() == LFG_GROUP)
                        break;

                    if((*grpitr1)->GetPremadePlayers()->find(citr->guid) == (*grpitr1)->GetPremadePlayers()->end() &&
                        (*grpitr2)->HasCorrectLevel(plr->getLevel()))
                    {
                        bool can = false;
                        for(uint8 role = TANK; role <= DAMAGE && !can; role*=2)
                        {
                            can = false;
                            if(((*grpitr1)->GetPlayerRole(citr->guid, false, true) & role) && (*grpitr2)->HasFreeRole(role))
                            {
                                can = true;
                                uint8 damage = 0;
                                for(ProposalAnswersMap::iterator ritr = canMove.begin(); ritr != canMove.end() && can; ++ritr)
                                {
                                    if(ritr->second == role && role != DAMAGE)
                                        can = false;
                                    else if (ritr->second == role)
                                        ++damage;
                                }
                                if(damage < 3 && can)
                                    canMove.insert(std::make_pair<uint64, uint8>(citr->guid, role));
                            }
                        }
                    }
                }
                if(canMove.empty() || canMove.size() + (*grpitr2)->GetMembersCount() != LFG_GROUP)
                    continue;

                // now we have players which can be moved
                for(ProposalAnswersMap::iterator plritr = canMove.begin(); plritr != canMove.end(); ++plritr)
                {
                    Player *plr = sObjectMgr.GetPlayer(plritr->first);
                    if (!plr || !plr->GetSession() || !plr->IsInWorld())
                        continue;
                    if((*grpitr2)->GetMembersCount() >= LFG_GROUP)
                        break;

                    (*grpitr1)->RemoveMember(plritr->first, 0);
                    (*grpitr2)->AddMember(plritr->first, plr->GetName());
                    (*grpitr2)->SetAsRole(plritr->first, plritr->second);
                    (*grpitr2)->GetRandomPlayers()->insert(plritr->first);
                }
                (*grpitr2)->SetOriginalDungeonInfo(info);
                if((*grpitr1)->GetMembersCount() != 0)
                {
                    LfgLog("RANDOM <-> SPECIFIC MERGE FAIL - random group %u is not empty after merge with %u", (*grpitr1)->GetId(), (*grpitr2)->GetId());
                    break;
                }
                toRemove.insert(*grpitr1);
                break; // <-- need to jump to next grpitr1!
            }
        }
    }
    GroupsList::iterator itr, itr_next;
    for(itr = toRemove.begin(); itr != toRemove.end(); itr = itr_next)
    {
        itr_next = itr;
        ++itr_next;
        LfgLog("RANDOM <-> SPECIFIC delete empty group %u", (*itr)->GetId());
        (*itr)->Disband(true);
        sObjectMgr.RemoveGroup(*itr);
        groups->erase(*itr);
        delete *itr;
    }
    toRemove.clear();
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
            bool leave = false;
            for(ProposalAnswersMap::iterator itr = (*grpitr)->GetProposalAnswers()->begin(); itr != (*grpitr)->GetProposalAnswers()->end(); ++itr)
            {
                if (itr->second != 1 && (*grpitr)->GetPremadePlayers()->find(itr->first) == (*grpitr)->GetPremadePlayers()->end())
                {
                    leave = true;
                    break;
                }
            } 
            //this return false if  time has passed or player offline
            if (!(*grpitr)->UpdateCheckTimer(LFG_TIMER_UPDATE_PROPOSAL) || leave)
            {
                LfgLog("Formed Group %u - times up", (*grpitr)->GetId());
                (*grpitr)->SendProposalUpdate(LFG_PROPOSAL_FAILED);
                
                //Send to players..
                PlayerList toRemove;
                for(Group::member_citerator citr = (*grpitr)->GetMemberSlots().begin(); citr != (*grpitr)->GetMemberSlots().end(); ++citr)
                {
                    Player *member = sObjectMgr.GetPlayer(citr->guid);
                    if (!member || !member->GetSession() || member->GetGUID() == (*grpitr)->GetLeaderGUID())
                        continue;
                    ProposalAnswersMap::iterator itr = (*grpitr)->GetProposalAnswers()->find(member->GetGUID());
                    if (((itr == (*grpitr)->GetProposalAnswers()->end() && !leave) || itr->second == 0)
                        && (*grpitr)->GetPremadePlayers()->find(citr->guid) == (*grpitr)->GetPremadePlayers()->end())
                    {
                        SendLfgUpdatePlayer(member, LFG_UPDATETYPE_PROPOSAL_FAILED);
                        member->m_lookingForGroup.queuedDungeons.clear();
                        LfgLog("Remove member - afk rolecheck");
                        toRemove.insert(member->GetGUID());
                    }
                }
                for(PlayerList::iterator rm = toRemove.begin(); rm != toRemove.end(); ++rm)
                    (*grpitr)->RemoveMember(*rm, 0);
                toRemove.clear();
                removeFromFormed.insert(*grpitr);
                
                // if specific and random merged, return players to random
                for(PlayerList::iterator rnd = (*grpitr)->GetRandomPlayers()->begin(); rnd != (*grpitr)->GetRandomPlayers()->end(); ++rnd)
                {
                    if(!(*grpitr)->IsMember(*rnd) || !sObjectMgr.GetPlayer(*rnd))
                        continue;
                    (*grpitr)->RemoveMember(*rnd, 0);
                    QueuedDungeonsMap::iterator queue = GetOrCreateQueueEntry((*grpitr)->GetDungeonInfo(true),i);
                    queue->second->players.insert(*rnd);
                    m_queuedPlayers.insert(std::make_pair<uint64, uint32>(*rnd, (*grpitr)->GetDungeonInfo(true)->ID));
                }
                (*grpitr)->GetRandomPlayers()->clear();
                (*grpitr)->SetOriginalDungeonInfo(NULL);

                //Remove empty group
                if((*grpitr)->GetMembersCount() == 0)
                {
                    AddGroupToDelete(*grpitr);
                    continue;
                }
                (*grpitr)->AddLfgFlag(LFG_GRP_BONUS);
                //Move group to queue
                MoveGroupToQueue(*grpitr, i);
                continue;
            }
            //all player responded
            if ((*grpitr)->GetProposalAnswers()->size() == LFG_GROUP)
            {
                (*grpitr)->SendProposalUpdate(LFG_PROPOSAL_SUCCESS);          
                //We are good to go, sir
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
        for(GroupsList::iterator itr = removeFromFormed.begin(); itr != removeFromFormed.end(); ++itr)
            formedGroups[i].erase(*itr);
        if(!removeFromFormed.empty() && sWorld.getConfig(CONFIG_BOOL_LFG_IMMIDIATE_QUEUE_UPDATE))
            UpdateQueue(i);
        removeFromFormed.clear();
    }
}

void LfgMgr::MoveGroupToQueue(LfgGroup *group, uint8 side, uint32 DungId)
{
    LFGDungeonEntry const *entry = DungId ? sLFGDungeonStore.LookupEntry(DungId) : group->GetDungeonInfo();
    QueuedDungeonsMap::iterator itr = GetOrCreateQueueEntry(entry, side);

    for(Group::member_citerator citr = group->GetMemberSlots().begin(); citr != group->GetMemberSlots().end(); ++citr)
    {
        Player *member = sObjectMgr.GetPlayer(citr->guid);
        if (!member || !member->GetSession())
            continue;
        SendLfgUpdateParty(member,  LFG_UPDATETYPE_ADDED_TO_QUEUE);
        SendLfgUpdatePlayer(member, LFG_UPDATETYPE_ADDED_TO_QUEUE);
        m_queuedPlayers.insert(std::make_pair<uint64, uint32>(member->GetGUID(), entry->ID));
    }
    
    itr->second->groups.insert(group);
    group->ResetGroup();
}

void LfgMgr::SendLfgPlayerInfo(Player *plr)
{
    LfgDungeonList *random = GetRandomDungeons(plr);
    LfgLocksList *locks = GetDungeonsLock(plr);

    WorldPacket data(SMSG_LFG_PLAYER_INFO);
    data << uint8(random->size());                                          // Random Dungeon count
    for (LfgDungeonList::iterator itr = random->begin(); itr != random->end(); ++itr)
    {
        data << uint32((*itr)->Entry());                       // Entry(ID and type) of random dungeon
        BuildRewardBlock(data, (*itr)->ID, plr);
    }
    random->clear();
    delete random;

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

    if(plr->IsBeingTeleported() && (updateType == LFG_UPDATETYPE_ADDED_TO_QUEUE || updateType == LFG_UPDATETYPE_REMOVED_FROM_QUEUE))
        plr->m_lookingForGroup.sendAtMapAdd[0] = updateType;
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
    if(plr->IsBeingTeleported() && (updateType == LFG_UPDATETYPE_ADDED_TO_QUEUE || updateType == LFG_UPDATETYPE_REMOVED_FROM_QUEUE))
        plr->m_lookingForGroup.sendAtMapAdd[1] = updateType;
}

void LfgMgr::BuildRewardBlock(WorldPacket &data, uint32 dungeon, Player *plr)
{
    LfgReward *reward = GetDungeonReward(dungeon, plr->m_lookingForGroup.DoneDungeon(dungeon, plr), plr->getLevel());

    data << uint8(plr->m_lookingForGroup.DoneDungeon(dungeon, plr));  // false = its first run this day, true = it isnt
    if (data.GetOpcode() == SMSG_LFG_PLAYER_REWARD)
        data << uint32(plr->GetGroup()->GetMembersCount()-1);         // strangers

    if (!reward)
    {
        data << uint32(0);  // money
        data << uint32(0);  // xp 
        data << uint32(0);  // money something
        data << uint32(0);  // xp something
        data << uint8(0);   // no item 
        return;
    }

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
        if ((*itr)->type != dungeonInfo->type || (*itr)->GroupType != dungeonInfo->grouptype ||
            (*itr)->isDaily() != done)
            continue;

        //World event check
        if(dungeonInfo->grouptype == LFG_GROUPTYPE_WORLD_EVENT &&
            (*itr)->DungeonId != dungeonInfo->ID && (*itr)->DungeonId != -1)
            continue;

        Quest *rewQuest = (*itr)->questInfo;
        if (level >= (*itr)->questInfo->GetMinLevel() &&  level <= (*itr)->questInfo->GetQuestLevel())  // ...mostly, needs some adjusting in db, blizz q level are without order
            return *itr;
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
        if (currentRow && (currentRow->type == LFG_TYPE_RANDOM || currentRow->grouptype == LFG_GROUPTYPE_WORLD_EVENT) &&
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
       /* if (InstanceTemplate const *instance = sObjectMgr.GetInstanceTemplate(currentRow->map))
        {
            minlevel = instance->levelMin == 0 ? currentRow->minlevel : instance->levelMin;
            maxlevel = instance->levelMax == 0 ? currentRow->maxlevel : instance->levelMax;
        }
        else*/
        // Or better not....
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
        delete *itr; 

    m_rewardsList.clear();

    uint32 count = 0;
    //                                                0     1          2           3      4 
    QueryResult *result = WorldDatabase.Query("SELECT type, groupType, questEntry, flags, DungeonId FROM quest_lfg_relation");

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
        reward->DungeonId             = fields[4].GetInt32();

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

        DungeonInfoMap::iterator itr =  m_dungeonInfoMap.find(fields[0].GetUInt32());
        if(itr == m_dungeonInfoMap.end())
        {
            sLog.outErrorDb("Entry listed in 'lfg_dungeon_info' has non-exist LfgDungeon.dbc id %u, skipping.", fields[0].GetUInt32());
            continue;
        }
        if (!sObjectMgr.GetCreatureTemplate(fields[2].GetUInt32()) && fields[2].GetUInt32() != 0)
        {
            sLog.outErrorDb("Entry listed in 'lfg_dungeon_info' has non-exist creature_template entry %u, skipping.", fields[2].GetUInt32());
            continue;   
        }

        itr->second->name             = fields[1].GetCppString();
        itr->second->lastBossId       = fields[2].GetUInt32();
        itr->second->start_map        = fields[3].GetUInt32();
        itr->second->start_x          = fields[4].GetFloat();
        itr->second->start_y          = fields[5].GetFloat();
        itr->second->start_z          = fields[6].GetFloat();
        itr->second->start_o          = fields[7].GetFloat();
        itr->second->locked           = fields[8].GetBool();

        ++count;
    } while( result->NextRow() );

    delete result;

    sLog.outString();
    sLog.outString( ">> Loaded %u LFG dungeon info entries.", count );
}

void LfgMgr::AssembleRandomInfo()
{
    // In case of reload
    for(LfgDungeonMap::iterator itr = m_randomsList.begin(); itr != m_randomsList.end(); ++itr)
        delete itr->second;
    m_randomsList.clear();
 
    LFGDungeonEntry const *random = NULL;
    DungeonInfo* randomInfo = NULL;
    LFGDungeonEntry const *cur = NULL;
    DungeonInfo* curInfo = NULL;

    uint32 count = 0;
    barGoLink bar( sLFGDungeonStore.GetNumRows() );

    for (uint32 i = 0; i < sLFGDungeonStore.GetNumRows(); ++i)
    {
        bar.step();
        random = sLFGDungeonStore.LookupEntry(i);
        if (!random || random->type != LFG_TYPE_RANDOM)
            continue;

        randomInfo = GetDungeonInfo(random->ID);
        
        if(!randomInfo || randomInfo->locked)
            continue;

        LfgDungeonList* list = new LfgDungeonList();
        for (uint32 y = 0; y < sLFGDungeonStore.GetNumRows(); ++y)
        {
            cur = sLFGDungeonStore.LookupEntry(y);
            if(!cur || cur->type == LFG_TYPE_RANDOM || cur->grouptype != random->grouptype)
                continue;
            curInfo = GetDungeonInfo(cur->ID);
            if(!curInfo || curInfo->locked)
                continue;

            list->insert(cur);
        }
        if(!list->empty())
        {
            m_randomsList.insert(std::make_pair<uint32, LfgDungeonList*>(random->ID, list));
            ++count;
        }
    }
    sLog.outString();
    sLog.outString( ">> Assembled %u random dungeon options.", count );
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

void LfgMgr::UpdateWaitTime(LfgGroup *group, uint32 dungeonId)
{
    uint32 avgWaitTime = 0;
    uint8 timescount = 0;
    uint32 time = getMSTime();
    for(uint8 role = TANK; role <= DAMAGE; role *= 2)
    {
        //Tank and heal
        if(role != DAMAGE)
        {
            uint64 guid = (role == TANK) ? group->GetTank() : group->GetHeal();
            uint8 slot = (role == TANK) ? LFG_WAIT_TIME_TANK : LFG_WAIT_TIME_HEAL;
            Player *member = sObjectMgr.GetPlayer(guid);
            if(!member || !member->GetSession())
                continue;
            uint32 currentTime = getMSTimeDiff(member->m_lookingForGroup.joinTime, time);
            if (currentTime < 5000) // 5s cooldown
                continue;
            WaitTimeMap::iterator waitItr = m_waitTimes[slot].find(dungeonId);
            avgWaitTime += (waitItr->second+currentTime)/2;
            waitItr->second = (waitItr->second+currentTime)/2;
            ++timescount;
            continue;
        }
        //Damage
        for(PlayerList::iterator plritr = group->GetDps()->begin(); plritr != group->GetDps()->end(); ++plritr)
        {
            Player *dps = sObjectMgr.GetPlayer(*plritr);
            if(!dps || !dps->GetSession())
                continue;

            WaitTimeMap::iterator waitItr = m_waitTimes[LFG_WAIT_TIME_DPS].find(dungeonId);
            uint32 currTime = getMSTimeDiff(dps->m_lookingForGroup.joinTime, time);
            if (currTime < 5000) // 5s cooldown
                continue;

            avgWaitTime += (waitItr->second+currTime)/2;
            waitItr->second = (waitItr->second+currTime)/2;
            ++timescount;
        }
    }
    if (timescount != 0)
        m_waitTimes[LFG_WAIT_TIME].find(dungeonId)->second = (avgWaitTime/timescount);
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
        sObjectMgr.RemoveGroup(*group);
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
            if ((*itr)->IsMember(player->GetGUID()) && !(*itr)->IsPremade())
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
    m_queuedPlayers.erase(player->GetGUID());
}

void LfgMgr::SendJoinResult(Player *player, uint8 result, uint32 value)
{
    if(!player)
        return;
    WorldPacket data(SMSG_LFG_JOIN_RESULT, 8);
    data << uint32(result);
    data << uint32(value);
    player->GetSession()->SendPacket(&data);
}

uint8 LfgMgr::GetSideForPlayer(Player *player)
{
    if(!sWorld.getConfig(CONFIG_BOOL_LFG_ALLOW_MIXED) || 
        player->getLevel() > sWorld.getConfig(CONFIG_UINT32_LFG_MIXED_MAXLEVEL) ||
        player->getLevel() < sWorld.getConfig(CONFIG_UINT32_LFG_MIXED_MINLEVEL))
        return (player->GetTeam() == ALLIANCE) ? LFG_ALLIANCE : LFG_HORDE;
    else
        return LFG_MIXED;
}

QueuedDungeonsMap::iterator LfgMgr::GetOrCreateQueueEntry(LFGDungeonEntry const *info, uint8 side)
{
    QueuedDungeonsMap::iterator itr = m_queuedDungeons[side].find(info->ID);
    if (itr != m_queuedDungeons[side].end())
        return itr;
    else
    {
        QueuedDungeonInfo *newInfo = new QueuedDungeonInfo();
        newInfo->dungeonInfo = info;
        m_queuedDungeons[side].insert(std::pair<uint32, QueuedDungeonInfo*>(info->ID, newInfo));
        //fill some default data into wait times
        if (m_waitTimes[0].find(info->ID) == m_waitTimes[0].end())
            for(int i = 0; i < LFG_WAIT_TIME_SLOT_MAX; ++i)
                m_waitTimes[i].insert(std::make_pair<uint32, uint32>(info->ID, 0));
        return m_queuedDungeons[side].find(info->ID);
    }
    return m_queuedDungeons[side].end();
}

void LfgMgr::SendLfgContinue(Player *player)
{
    if(player->m_lookingForGroup.sendAtMapAdd[0] != 0)
        SendLfgUpdatePlayer(player, player->m_lookingForGroup.sendAtMapAdd[0]);
    if(player->m_lookingForGroup.sendAtMapAdd[1] != 0)
        SendLfgUpdateParty(player, player->m_lookingForGroup.sendAtMapAdd[1]);
    player->m_lookingForGroup.sendAtMapAdd[0] = 0;
    player->m_lookingForGroup.sendAtMapAdd[1] = 0;

    if((!player->GetGroup() || !player->GetGroup()->isLfgGroup()) && !player->m_lookingForGroup.groups.empty())
    {
        LfgGroup *group = NULL;
        uint8 side = GetSideForPlayer(player);
        for(GroupMap::iterator itr = player->m_lookingForGroup.groups.begin(); itr != player->m_lookingForGroup.groups.end(); ++itr)
        {
            group = sObjectMgr.GetLfgGroupById(itr->second);
            if(!group || formedGroups[side].find(group) == formedGroups[side].end())
                continue;

            if(group->GetRoleAnswers()->find(player->GetGUID()) == group->GetRoleAnswers()->end())
                SendLfgUpdatePlayer(player, LFG_UPDATETYPE_PROPOSAL_FOUND);
        }
    }
}
