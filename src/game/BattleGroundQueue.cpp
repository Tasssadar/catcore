#include "Common.h"
#include "SharedDefines.h"
#include "Player.h"
#include "BattleGroundMgr.h"
#include "BattleGroundAV.h"
#include "BattleGroundAB.h"
#include "BattleGroundEY.h"
#include "BattleGroundWS.h"
#include "BattleGroundNA.h"
#include "BattleGroundBE.h"
#include "BattleGroundAA.h"
#include "BattleGroundRL.h"
#include "BattleGroundSA.h"
#include "BattleGroundDS.h"
#include "BattleGroundRV.h"
#include "BattleGroundIC.h"
#include "BattleGroundRB.h"
#include "MapManager.h"
#include "Map.h"
#include "ObjectMgr.h"
#include "ProgressBar.h"
#include "Chat.h"
#include "ArenaTeam.h"
#include "World.h"
#include "WorldPacket.h"
#include "GameEventMgr.h"

/*********************************************************/
/***            BATTLEGROUND QUEUE SYSTEM              ***/
/*********************************************************/

BattleGroundQueue::BattleGroundQueue(uint32 queueTypeId)
{
    for(uint32 i = 0; i < BG_TEAMS_COUNT; ++i)
    {
        for(uint32 j = 0; j < MAX_BATTLEGROUND_BRACKETS; ++j)
        {
            m_SumOfWaitTimes[i][j] = 0;
            m_WaitTimeLastPlayer[i][j] = 0;
            for(uint32 k = 0; k < COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME; ++k)
                m_WaitTimes[i][j][k] = 0;
        }
    }

    m_QueueTypeId = BattleGroundQueueTypeId(queueTypeId);
}

BattleGroundQueue::~BattleGroundQueue()
{
    m_QueuedPlayers.clear();
    for (int i = 0; i < MAX_BATTLEGROUND_BRACKETS; ++i)
    {
        for(uint32 j = 0; j < BG_QUEUE_GROUP_TYPES_COUNT; ++j)
        {
            for(GroupsQueueType::iterator itr = m_QueuedGroups[i][j].begin(); itr!= m_QueuedGroups[i][j].end(); ++itr)
                delete (*itr);

            m_QueuedGroups[i][j].clear();
        }

        for(GroupsQueueType::iterator itr = m_QueuedRatedArenas[i].begin(); itr!= m_QueuedRatedArenas[i].end(); ++itr)
            delete (*itr);

        m_QueuedRatedArenas[i].clear();
    }
}

/*********************************************************/
/***      BATTLEGROUND QUEUE SELECTION POOLS           ***/
/*********************************************************/

void BattleGroundQueue::SelectionPool::Init()
{
    SelectedGroups.clear();
    PlayerCount = 0;
}

bool BattleGroundQueue::SelectionPool::KickGroup(uint32 size)
{
    //find maxgroup or LAST group with size == size and kick it
    bool found = false;
    GroupsQueueType::iterator groupToKick = SelectedGroups.begin();
    for (GroupsQueueType::iterator itr = groupToKick; itr != SelectedGroups.end(); ++itr)
    {
        if (abs((int32)((*itr)->Players.size() - size)) <= 1)
        {
            groupToKick = itr;
            found = true;
        }
        else if (!found && (*itr)->Players.size() >= (*groupToKick)->Players.size())
            groupToKick = itr;
    }
    //if pool is empty, do nothing
    if (GetPlayerCount())
    {
        //update player count
        GroupQueueInfo* ginfo = (*groupToKick);
        SelectedGroups.erase(groupToKick);
        PlayerCount -= ginfo->Players.size();
        //return false if we kicked smaller group or there are enough players in selection pool
        if (ginfo->Players.size() <= size + 1)
            return false;
    }
    return true;
}

bool BattleGroundQueue::SelectionPool::AddGroup(GroupQueueInfo *ginfo, uint32 desiredCount)
{
    //if group is larger than desired count - don't allow to add it to pool
    if (!ginfo->IsInvitedToBGInstanceGUID && desiredCount >= PlayerCount + ginfo->Players.size())
    {
        SelectedGroups.push_back(ginfo);
        // increase selected players count
        PlayerCount += ginfo->Players.size();
        return true;
    }
    if (PlayerCount < desiredCount)
        return true;
    return false;
}

/*********************************************************/
/***               BATTLEGROUND QUEUES                 ***/
/*********************************************************/

// add group or player (grp == NULL) to bg queue with the given leader and bg specifications
GroupQueueInfo * BattleGroundQueue::AddGroup(Player *leader, Group* grp, BattleGroundTypeId BgTypeId, PvPDifficultyEntry const*  bracketEntry, uint8 ArenaType, bool isRated, bool isPremade, uint32 arenaRating, uint32 arenateamid, uint32 ArenaMMR)
{
    BattleGroundBracketId bracketId =  bracketEntry->GetBracketId();

    // create new ginfo
    GroupQueueInfo* ginfo = new GroupQueueInfo;
    ginfo->BgTypeId                  = BgTypeId;
    ginfo->ArenaType                 = ArenaType;
    ginfo->ArenaTeamId               = arenateamid;
    ginfo->OpponentTeamId            = 0;
    ginfo->IsRated                   = isRated;
    ginfo->IsInvitedToBGInstanceGUID = 0;
    ginfo->JoinTime                  = getMSTime();
    ginfo->RemoveInviteTime          = 0;
    ginfo->Team                      = leader->GetTeam();
    ginfo->ArenaTeamRating           = arenaRating;
    ginfo->ArenaTeamMMR              = ArenaMMR;
    ginfo->OpponentsMMR              = 0;
    ginfo->CurrentMaxChanceDiff      = sBattleGroundMgr.GetStartMaxChanceDiff();
    ginfo->LastUpdatedTime           = 0;
    ginfo->DiscartedTime             = 0;

    ginfo->Players.clear();

    //compute index (if group is premade or joined a rated match) to queues
    uint32 index = 0;
    if (!isRated && !isPremade)
        index += BG_TEAMS_COUNT;
    if (ginfo->Team == HORDE)
        index++;
    DEBUG_LOG("Adding Group to BattleGroundQueue bgTypeId : %u, bracket_id : %u, index : %u", BgTypeId, bracketId, index);
    // --- TEAM BG ---
    if (!ArenaType && !isRated && !isPremade)
    {
        eConfigBoolValues targetConfig;
        switch(BgTypeId)
        {
            case BATTLEGROUND_AB: targetConfig = CONFIG_BOOL_TEAM_BG_ALLOW_AB; break;
            case BATTLEGROUND_AV: targetConfig = CONFIG_BOOL_TEAM_BG_ALLOW_AV; break;
            case BATTLEGROUND_EY: targetConfig = CONFIG_BOOL_TEAM_BG_ALLOW_EOS; break;
            case BATTLEGROUND_WS: targetConfig = CONFIG_BOOL_TEAM_BG_ALLOW_WSG; break;
            case BATTLEGROUND_RB: targetConfig = CONFIG_BOOL_TEAM_BG_ALLOW_RANDOM; break;
            default: targetConfig = 0; break;
        }
        if (targetConfig != 0 && sWorld.getConfig(targetConfig))
        {
            uint32 qHorde = 0;
            uint32 qAlliance = 0;
            GroupsQueueType::const_iterator itr;
            for(itr = m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_ALLIANCE].begin(); itr != m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_ALLIANCE].end(); ++itr)
                if (!(*itr)->IsInvitedToBGInstanceGUID)
                    qAlliance += (*itr)->Players.size();
            for(itr = m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_HORDE].begin(); itr != m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_HORDE].end(); ++itr)
                if (!(*itr)->IsInvitedToBGInstanceGUID)
                    qHorde += (*itr)->Players.size();
            //If theres more ali then horde, then change index to horde
            if (qAlliance > qHorde+1)
            {
                index = 3;  // Set horde
                ginfo->Team = HORDE;
            }
            else if (qAlliance+1 < qHorde)
            {
                index = 2;  // Set Aliance
                ginfo->Team = ALLIANCE;
            }
        }
    }

    uint32 lastOnlineTime = getMSTime();

    //announce world (this don't need mutex)
    if (isRated && sWorld.getConfig(CONFIG_BOOL_ARENA_QUEUE_ANNOUNCER_JOIN))
    {
        sWorld.SendWorldText(LANG_ARENA_QUEUE_ANNOUNCE_WORLD_JOIN, ginfo->ArenaType, ginfo->ArenaType, ginfo->ArenaTeamRating);
    }

    //add players from group to ginfo
    {
        //ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Lock);
        if (grp)
        {
            for(GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player *member = itr->getSource();
                if (!member)
                    continue;   // this should never happen
                PlayerQueueInfo& pl_info = m_QueuedPlayers[member->GetGUID()];
                pl_info.LastOnlineTime   = lastOnlineTime;
                pl_info.GroupInfo        = ginfo;
                // add the pinfo to ginfo's list
                ginfo->Players[member->GetGUID()]  = &pl_info;
            }
        }
        else
        {
            PlayerQueueInfo& pl_info = m_QueuedPlayers[leader->GetGUID()];
            pl_info.LastOnlineTime   = lastOnlineTime;
            pl_info.GroupInfo        = ginfo;
            ginfo->Players[leader->GetGUID()]  = &pl_info;
        }

        //add GroupInfo to m_QueuedGroups
        if (ArenaType && isRated)
            m_QueuedRatedArenas[bracketId].push_back(ginfo);
        else
            m_QueuedGroups[bracketId][index].push_back(ginfo);

        //announce to world, this code needs mutex
        if (!ArenaType && !isRated && !isPremade && sWorld.getConfig(CONFIG_UINT32_BATTLEGROUND_QUEUE_ANNOUNCER_JOIN))
        {
            if (BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(ginfo->BgTypeId))
            {
                char const* bgName = bg->GetName();
                uint32 MinPlayers = bg->GetMinPlayersPerTeam();
                uint32 qHorde = 0;
                uint32 qAlliance = 0;
                uint32 q_min_level = bracketEntry->minLevel;
                uint32 q_max_level = bracketEntry->maxLevel;
                GroupsQueueType::const_iterator itr;
                for(itr = m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_ALLIANCE].begin(); itr != m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_ALLIANCE].end(); ++itr)
                    if (!(*itr)->IsInvitedToBGInstanceGUID)
                        qAlliance += (*itr)->Players.size();
                for(itr = m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_HORDE].begin(); itr != m_QueuedGroups[bracketId][BG_QUEUE_NORMAL_HORDE].end(); ++itr)
                    if (!(*itr)->IsInvitedToBGInstanceGUID)
                        qHorde += (*itr)->Players.size();

                // Show queue status to player only (when joining queue)
                if (sWorld.getConfig(CONFIG_UINT32_BATTLEGROUND_QUEUE_ANNOUNCER_JOIN)==1)
                {
                    ChatHandler(leader).PSendSysMessage(LANG_BG_QUEUE_ANNOUNCE_SELF, bgName, q_min_level, q_max_level,
                        qAlliance, (MinPlayers > qAlliance) ? MinPlayers - qAlliance : (uint32)0, qHorde, (MinPlayers > qHorde) ? MinPlayers - qHorde : (uint32)0);
                }
                // System message
                else
                {
                    sWorld.SendWorldText(LANG_BG_QUEUE_ANNOUNCE_WORLD, bgName, q_min_level, q_max_level,
                        qAlliance, (MinPlayers > qAlliance) ? MinPlayers - qAlliance : (uint32)0, qHorde, (MinPlayers > qHorde) ? MinPlayers - qHorde : (uint32)0);
                }
            }
        }
        //release mutex
    }

    return ginfo;
}

void BattleGroundQueue::PlayerInvitedToBGUpdateAverageWaitTime(GroupQueueInfo* ginfo, BattleGroundBracketId bracket_id)
{
    uint32 timeInQueue = getMSTimeDiff(ginfo->JoinTime, getMSTime());
    uint8 team_index = BG_TEAM_ALLIANCE;                    //default set to BG_TEAM_ALLIANCE - or non rated arenas!
    if (!ginfo->ArenaType)
    {
        if (ginfo->Team == HORDE)
            team_index = BG_TEAM_HORDE;
    }
    else
    {
        if (ginfo->IsRated)
            team_index = BG_TEAM_HORDE;                     //for rated arenas use BG_TEAM_HORDE
    }

    //store pointer to arrayindex of player that was added first
    uint32* lastPlayerAddedPointer = &(m_WaitTimeLastPlayer[team_index][bracket_id]);
    //remove his time from sum
    m_SumOfWaitTimes[team_index][bracket_id] -= m_WaitTimes[team_index][bracket_id][(*lastPlayerAddedPointer)];
    //set average time to new
    m_WaitTimes[team_index][bracket_id][(*lastPlayerAddedPointer)] = timeInQueue;
    //add new time to sum
    m_SumOfWaitTimes[team_index][bracket_id] += timeInQueue;
    //set index of last player added to next one
    (*lastPlayerAddedPointer)++;
    (*lastPlayerAddedPointer) %= COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME;
}

uint32 BattleGroundQueue::GetAverageQueueWaitTime(GroupQueueInfo* ginfo, BattleGroundBracketId bracket_id)
{
    uint8 team_index = BG_TEAM_ALLIANCE;                    //default set to BG_TEAM_ALLIANCE - or non rated arenas!
    if (!ginfo->ArenaType)
    {
        if (ginfo->Team == HORDE)
            team_index = BG_TEAM_HORDE;
    }
    else
    {
        if (ginfo->IsRated)
            team_index = BG_TEAM_HORDE;                     //for rated arenas use BG_TEAM_HORDE
    }
    //check if there is enought values(we always add values > 0)
    if (m_WaitTimes[team_index][bracket_id][COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME - 1] )
        return (m_SumOfWaitTimes[team_index][bracket_id] / COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME);
    else
        //if there aren't enough values return 0 - not available
        return 0;
}

//remove player from queue and from group info, if group info is empty then remove it too
void BattleGroundQueue::RemovePlayer(const uint64& guid, bool decreaseInvitedCount)
{
    //Player *plr = sObjectMgr.GetPlayer(guid);
    //ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_Lock);

    int32 bracket_id = -1;                                     // signed for proper for-loop finish
    QueuedPlayersMap::iterator itr;

    //remove player from map, if he's there
    itr = m_QueuedPlayers.find(guid);
    if (itr == m_QueuedPlayers.end())
    {
        sLog.outError("BattleGroundQueue: couldn't find player to remove GUID: %u", GUID_LOPART(guid));
        return;
    }

    GroupQueueInfo* group = itr->second.GroupInfo;
    GroupsQueueType::iterator group_itr, group_itr_tmp;
    // mostly people with the highest levels are in battlegrounds, thats why
    // we count from MAX_BATTLEGROUND_QUEUES - 1 to 0
    // variable index removes useless searching in other team's queue
    uint32 index = (group->Team == HORDE) ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;

    for (int32 bracket_id_tmp = MAX_BATTLEGROUND_BRACKETS - 1; bracket_id_tmp >= 0 && bracket_id == -1; --bracket_id_tmp)
    {
        //we must check premade and normal team's queue - because when players from premade are joining bg,
        //they leave groupinfo so we can't use its players size to find out index
        for (uint32 j = index; j < BG_QUEUE_GROUP_TYPES_COUNT; j += BG_QUEUE_NORMAL_ALLIANCE)
        {
            for(group_itr_tmp = m_QueuedGroups[bracket_id_tmp][j].begin(); group_itr_tmp != m_QueuedGroups[bracket_id_tmp][j].end(); ++group_itr_tmp)
            {
                if ((*group_itr_tmp) == group)
                {
                    bracket_id = bracket_id_tmp;
                    group_itr = group_itr_tmp;
                    //we must store index to be able to erase iterator
                    index = j;
                    break;
                }
            }
        }
        for(group_itr_tmp = m_QueuedRatedArenas[bracket_id_tmp].begin(); group_itr_tmp != m_QueuedRatedArenas[bracket_id_tmp].end(); ++group_itr_tmp)
        {
            if ((*group_itr_tmp) == group)
            {
                bracket_id = bracket_id_tmp;
                group_itr = group_itr_tmp;
                break;
            }
        }
    }
    //player can't be in queue without group, but just in case
    if (bracket_id == -1)
    {
        sLog.outError("BattleGroundQueue: ERROR Cannot find groupinfo for player GUID: %u", GUID_LOPART(guid));
        return;
    }
    DEBUG_LOG("BattleGroundQueue: Removing player GUID %u, from bracket_id %u", GUID_LOPART(guid), (uint32)bracket_id);

    // ALL variables are correctly set
    // We can ignore leveling up in queue - it should not cause crash
    // remove player from group
    // if only one player there, remove group

    // remove player queue info from group queue info
    std::map<uint64, PlayerQueueInfo*>::iterator pitr = group->Players.find(guid);
    if (pitr != group->Players.end())
        group->Players.erase(pitr);

    // if invited to bg, and should decrease invited count, then do it
    if (decreaseInvitedCount && group->IsInvitedToBGInstanceGUID)
    {
        BattleGround* bg = sBattleGroundMgr.GetBattleGround(group->IsInvitedToBGInstanceGUID, group->BgTypeId);
        if (bg)
            bg->DecreaseInvitedCount(group->Team);
    }

    // remove player queue info
    m_QueuedPlayers.erase(itr);

    // announce to world if arena team left queue for rated match, show only once
    if (group->ArenaType && group->IsRated && group->Players.empty() && sWorld.getConfig(CONFIG_BOOL_ARENA_QUEUE_ANNOUNCER_EXIT))
        sWorld.SendWorldText(LANG_ARENA_QUEUE_ANNOUNCE_WORLD_EXIT, group->ArenaType, group->ArenaType, group->ArenaTeamRating);

    //if player leaves queue and he is invited to rated arena match, then he have to loose
    if (group->IsInvitedToBGInstanceGUID && group->IsRated && decreaseInvitedCount)
    {
        ArenaTeam * at = sObjectMgr.GetArenaTeamById(group->ArenaTeamId);
        if (at)
        {
            DEBUG_LOG("UPDATING memberLost's personal arena rating for %u by opponents rating: %u", GUID_LOPART(guid), group->OpponentsMMR);
            Player *plr = sObjectMgr.GetPlayer(guid);
            if (plr)
                at->MemberPlayed(plr, group->OpponentsMMR, false);
            else
                at->OfflineMemberLost(guid, group->OpponentsMMR);
            at->SaveToDB();
        }
    }

    // remove group queue info if needed
    if (group->Players.empty())
    {
        m_QueuedGroups[bracket_id][index].erase(group_itr);
        delete group;
    }
    // if group wasn't empty, so it wasn't deleted, and player have left a rated
    // queue -> everyone from the group should leave too
    // don't remove recursively if already invited to bg!
    else if (!group->IsInvitedToBGInstanceGUID && group->IsRated)
    {
        // remove next player, this is recursive
        // first send removal information
        if (Player *plr2 = sObjectMgr.GetPlayer(group->Players.begin()->first))
        {
            BattleGround * bg = sBattleGroundMgr.GetBattleGroundTemplate(group->BgTypeId);
            BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(group->BgTypeId, group->ArenaType);
            uint32 queueSlot = plr2->GetBattleGroundQueueIndex(bgQueueTypeId);
            plr2->RemoveBattleGroundQueueId(bgQueueTypeId); // must be called this way, because if you move this call to
                                                            // queue->removeplayer, it causes bugs
            WorldPacket data;
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_NONE, 0, 0, 0);
            plr2->GetSession()->SendPacket(&data);
        }
        // then actually delete, this may delete the group as well!
        RemovePlayer(group->Players.begin()->first, decreaseInvitedCount);
    }
}

//returns true when player pl_guid is in queue and is invited to bgInstanceGuid
bool BattleGroundQueue::IsPlayerInvited(const uint64& pl_guid, const uint32 bgInstanceGuid, const uint32 removeTime)
{
    //ACE_Guard<ACE_Recursive_Thread_Mutex> g(m_Lock);
    QueuedPlayersMap::const_iterator qItr = m_QueuedPlayers.find(pl_guid);
    return ( qItr != m_QueuedPlayers.end()
        && qItr->second.GroupInfo->IsInvitedToBGInstanceGUID == bgInstanceGuid
        && qItr->second.GroupInfo->RemoveInviteTime == removeTime );
}

bool BattleGroundQueue::GetPlayerGroupInfoData(const uint64& guid, GroupQueueInfo* ginfo)
{
    //ACE_Guard<ACE_Recursive_Thread_Mutex> g(m_Lock);
    QueuedPlayersMap::const_iterator qItr = m_QueuedPlayers.find(guid);
    if (qItr == m_QueuedPlayers.end())
        return false;
    *ginfo = *(qItr->second.GroupInfo);
    return true;
}

bool BattleGroundQueue::InviteGroupToBG(GroupQueueInfo * ginfo, BattleGround * bg, uint32 side)
{
    // set side if needed
    if (side)
        ginfo->Team = side;

    if (!ginfo->IsInvitedToBGInstanceGUID)
    {
        // not yet invited
        // set invitation
        ginfo->IsInvitedToBGInstanceGUID = bg->GetInstanceID();
        BattleGroundTypeId bgTypeId = bg->GetTypeID();
        BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(bgTypeId, bg->GetArenaType());
        BattleGroundBracketId bracket_id = bg->GetBracketId();

        // set ArenaTeamId for rated matches
        if (bg->isArena() && bg->isRated())
            bg->SetArenaTeamIdForTeam(ginfo->Team, ginfo->ArenaTeamId);

        ginfo->RemoveInviteTime = getMSTime() + INVITE_ACCEPT_WAIT_TIME;

        // loop through the players
        for(std::map<uint64,PlayerQueueInfo*>::iterator itr = ginfo->Players.begin(); itr != ginfo->Players.end(); ++itr)
        {
            // get the player
            Player* plr = sObjectMgr.GetPlayer(itr->first);
            // if offline, skip him, this should not happen - player is removed from queue when he logs out
            if (!plr)
                continue;

            // invite the player
            PlayerInvitedToBGUpdateAverageWaitTime(ginfo, bracket_id);
            //sBattleGroundMgr.InvitePlayer(plr, bg, ginfo->Team);

            // set invited player counters
            bg->IncreaseInvitedCount(ginfo->Team);

            plr->SetInviteForBattleGroundQueueType(bgQueueTypeId, ginfo->IsInvitedToBGInstanceGUID);

            // create remind invite events
            BGQueueInviteEvent* inviteEvent = new BGQueueInviteEvent(plr->GetGUID(), ginfo->IsInvitedToBGInstanceGUID, bgTypeId, ginfo->ArenaType, ginfo->RemoveInviteTime);
            plr->m_Events.AddEvent(inviteEvent, plr->m_Events.CalculateTime(INVITATION_REMIND_TIME));
            // create automatic remove events
            BGQueueRemoveEvent* removeEvent = new BGQueueRemoveEvent(plr->GetGUID(), ginfo->IsInvitedToBGInstanceGUID, bgTypeId, bgQueueTypeId, ginfo->RemoveInviteTime);
            plr->m_Events.AddEvent(removeEvent, plr->m_Events.CalculateTime(INVITE_ACCEPT_WAIT_TIME));

            WorldPacket data;

            uint32 queueSlot = plr->GetBattleGroundQueueIndex(bgQueueTypeId);

            DEBUG_LOG("Battleground: invited plr %s (%u) to BG instance %u queueindex %u bgtype %u, I can't help it if they don't press the enter battle button.",plr->GetName(),plr->GetGUIDLow(),bg->GetInstanceID(),queueSlot,bg->GetTypeID());

            // send status packet
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_JOIN, INVITE_ACCEPT_WAIT_TIME, 0, ginfo->ArenaType);
            plr->GetSession()->SendPacket(&data);
        }
        return true;
    }

    return false;
}

/*
This function is inviting players to already running battlegrounds
Invitation type is based on config file
large groups are disadvantageous, because they will be kicked first if invitation type = 1
*/
void BattleGroundQueue::FillPlayersToBG(BattleGround* bg, BattleGroundBracketId bracket_id)
{
    int32 hordeFree = bg->GetFreeSlotsForTeam(HORDE);
    int32 aliFree   = bg->GetFreeSlotsForTeam(ALLIANCE);

    //iterator for iterating through bg queue
    GroupsQueueType::const_iterator Ali_itr = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE].begin();
    //count of groups in queue - used to stop cycles
    uint32 aliCount = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE].size();
    //index to queue which group is current
    uint32 aliIndex = 0;
    for (; aliIndex < aliCount && m_SelectionPools[BG_TEAM_ALLIANCE].AddGroup((*Ali_itr), aliFree); aliIndex++)
        ++Ali_itr;
    //the same thing for horde
    GroupsQueueType::const_iterator Horde_itr = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_HORDE].begin();
    uint32 hordeCount = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_HORDE].size();
    uint32 hordeIndex = 0;
    for (; hordeIndex < hordeCount && m_SelectionPools[BG_TEAM_HORDE].AddGroup((*Horde_itr), hordeFree); hordeIndex++)
        ++Horde_itr;

    //if ofc like BG queue invitation is set in config, then we are happy
    if (sWorld.getConfig(CONFIG_UINT32_BATTLEGROUND_INVITATION_TYPE) == 0)
        return;

    /*
    if we reached this code, then we have to solve NP - complete problem called Subset sum problem
    So one solution is to check all possible invitation subgroups, or we can use these conditions:
    1. Last time when BattleGroundQueue::Update was executed we invited all possible players - so there is only small possibility
        that we will invite now whole queue, because only 1 change has been made to queues from the last BattleGroundQueue::Update call
    2. Other thing we should consider is group order in queue
    */

    // At first we need to compare free space in bg and our selection pool
    int32 diffAli   = aliFree   - int32(m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount());
    int32 diffHorde = hordeFree - int32(m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount());
    while( abs(diffAli - diffHorde) > 1 && (m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount() > 0 || m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount() > 0) )
    {
        //each cycle execution we need to kick at least 1 group
        if (diffAli < diffHorde)
        {
            //kick alliance group, add to pool new group if needed
            if (m_SelectionPools[BG_TEAM_ALLIANCE].KickGroup(diffHorde - diffAli))
            {
                for (; aliIndex < aliCount && m_SelectionPools[BG_TEAM_ALLIANCE].AddGroup((*Ali_itr), (aliFree >= diffHorde) ? aliFree - diffHorde : 0); aliIndex++)
                    ++Ali_itr;
            }
            //if ali selection is already empty, then kick horde group, but if there are less horde than ali in bg - break;
            if (!m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount())
            {
                if (aliFree <= diffHorde + 1)
                    break;
                m_SelectionPools[BG_TEAM_HORDE].KickGroup(diffHorde - diffAli);
            }
        }
        else
        {
            //kick horde group, add to pool new group if needed
            if (m_SelectionPools[BG_TEAM_HORDE].KickGroup(diffAli - diffHorde))
            {
                for (; hordeIndex < hordeCount && m_SelectionPools[BG_TEAM_HORDE].AddGroup((*Horde_itr), (hordeFree >= diffAli) ? hordeFree - diffAli : 0); hordeIndex++)
                    ++Horde_itr;
            }
            if (!m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount())
            {
                if (hordeFree <= diffAli + 1)
                    break;
                m_SelectionPools[BG_TEAM_ALLIANCE].KickGroup(diffAli - diffHorde);
            }
        }
        //count diffs after small update
        diffAli   = aliFree   - int32(m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount());
        diffHorde = hordeFree - int32(m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount());
    }
}

// this method checks if premade versus premade battleground is possible
// then after 30 mins (default) in queue it moves premade group to normal queue
// it tries to invite as much players as it can - to MaxPlayersPerTeam, because premade groups have more than MinPlayersPerTeam players
bool BattleGroundQueue::CheckPremadeMatch(BattleGroundBracketId bracket_id, uint32 MinPlayersPerTeam, uint32 MaxPlayersPerTeam)
{
    //check match
    if (!m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].empty() && !m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].empty())
    {
        //start premade match
        //if groups aren't invited
        GroupsQueueType::const_iterator ali_group, horde_group;
        for( ali_group = m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].begin(); ali_group != m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].end(); ++ali_group)
            if (!(*ali_group)->IsInvitedToBGInstanceGUID)
                break;
        for( horde_group = m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].begin(); horde_group != m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].end(); ++horde_group)
            if (!(*horde_group)->IsInvitedToBGInstanceGUID)
                break;

        if (ali_group != m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE].end() && horde_group != m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_HORDE].end())
        {
            m_SelectionPools[BG_TEAM_ALLIANCE].AddGroup((*ali_group), MaxPlayersPerTeam);
            m_SelectionPools[BG_TEAM_HORDE].AddGroup((*horde_group), MaxPlayersPerTeam);
            //add groups/players from normal queue to size of bigger group
            uint32 maxPlayers = std::max(m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount(), m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount());
            GroupsQueueType::const_iterator itr;
            for(uint32 i = 0; i < BG_TEAMS_COUNT; i++)
            {
                for(itr = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + i].begin(); itr != m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + i].end(); ++itr)
                {
                    //if itr can join BG and player count is less that maxPlayers, then add group to selectionpool
                    if (!(*itr)->IsInvitedToBGInstanceGUID && !m_SelectionPools[i].AddGroup((*itr), maxPlayers))
                        break;
                }
            }
            //premade selection pools are set
            return true;
        }
    }
    // now check if we can move group from Premade queue to normal queue (timer has expired) or group size lowered!!
    // this could be 2 cycles but i'm checking only first team in queue - it can cause problem -
    // if first is invited to BG and seconds timer expired, but we can ignore it, because players have only 80 seconds to click to enter bg
    // and when they click or after 80 seconds the queue info is removed from queue
    uint32 time_before = getMSTime() - sWorld.getConfig(CONFIG_UINT32_BATTLEGROUND_PREMADE_GROUP_WAIT_FOR_MATCH);
    for(uint32 i = 0; i < BG_TEAMS_COUNT; i++)
    {
        if (!m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE + i].empty())
        {
            GroupsQueueType::iterator itr = m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE + i].begin();
            if (!(*itr)->IsInvitedToBGInstanceGUID && ((*itr)->JoinTime < time_before || (*itr)->Players.size() < MinPlayersPerTeam))
            {
                //we must insert group to normal queue and erase pointer from premade queue
                m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + i].push_front((*itr));
                m_QueuedGroups[bracket_id][BG_QUEUE_PREMADE_ALLIANCE + i].erase(itr);
            }
        }
    }
    //selection pools are not set
    return false;
}

// this method tries to create battleground or arena with MinPlayersPerTeam against MinPlayersPerTeam
bool BattleGroundQueue::CheckNormalMatch(BattleGround* bg_template, BattleGroundBracketId bracket_id, uint32 minPlayers, uint32 maxPlayers)
{
    GroupsQueueType::const_iterator itr_team[BG_TEAMS_COUNT];
    for(uint32 i = 0; i < BG_TEAMS_COUNT; i++)
    {
        itr_team[i] = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + i].begin();
        for(; itr_team[i] != m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + i].end(); ++(itr_team[i]))
        {
            if (!(*(itr_team[i]))->IsInvitedToBGInstanceGUID)
            {
                m_SelectionPools[i].AddGroup(*(itr_team[i]), maxPlayers);
                if (m_SelectionPools[i].GetPlayerCount() >= minPlayers)
                    break;
            }
        }
    }
    //try to invite same number of players - this cycle may cause longer wait time even if there are enough players in queue, but we want ballanced bg
    uint32 j = BG_TEAM_ALLIANCE;
    if (m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount() < m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount())
        j = BG_TEAM_HORDE;
    if ( sWorld.getConfig(CONFIG_UINT32_BATTLEGROUND_INVITATION_TYPE) != 0
        && m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount() >= minPlayers && m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount() >= minPlayers )
    {
        //we will try to invite more groups to team with less players indexed by j
        ++(itr_team[j]);                                         //this will not cause a crash, because for cycle above reached break;
        for(; itr_team[j] != m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + j].end(); ++(itr_team[j]))
        {
            if (!(*(itr_team[j]))->IsInvitedToBGInstanceGUID)
                if (!m_SelectionPools[j].AddGroup(*(itr_team[j]), m_SelectionPools[(j + 1) % BG_TEAMS_COUNT].GetPlayerCount()))
                    break;
        }
        // do not allow to start bg with more than 2 players more on 1 faction
        if (abs((int32)(m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount() - m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount())) > 2)
            return false;
    }
    //allow 1v0 if debug bg
    if (sBattleGroundMgr.isTesting() && bg_template->isBattleGround() && (m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount() || m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount()))
        return true;
    //return true if there are enough players in selection pools - enable to work .debug bg command correctly
    return m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount() >= minPlayers && m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount() >= minPlayers;
}

// this method will check if we can invite players to same faction skirmish match
bool BattleGroundQueue::CheckSkirmishForSameFaction(BattleGroundBracketId bracket_id, uint32 minPlayersPerTeam)
{
    if (m_SelectionPools[BG_TEAM_ALLIANCE].GetPlayerCount() < minPlayersPerTeam && m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount() < minPlayersPerTeam)
        return false;
    uint32 teamIndex = BG_TEAM_ALLIANCE;
    uint32 otherTeam = BG_TEAM_HORDE;
    uint32 otherTeamId = HORDE;
    if (m_SelectionPools[BG_TEAM_HORDE].GetPlayerCount() == minPlayersPerTeam )
    {
        teamIndex = BG_TEAM_HORDE;
        otherTeam = BG_TEAM_ALLIANCE;
        otherTeamId = ALLIANCE;
    }
    //clear other team's selection
    m_SelectionPools[otherTeam].Init();
    //store last ginfo pointer
    GroupQueueInfo* ginfo = m_SelectionPools[teamIndex].SelectedGroups.back();
    //set itr_team to group that was added to selection pool latest
    GroupsQueueType::iterator itr_team = m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + teamIndex].begin();
    for(; itr_team != m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + teamIndex].end(); ++itr_team)
        if (ginfo == *itr_team)
            break;
    if (itr_team == m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + teamIndex].end())
        return false;
    GroupsQueueType::iterator itr_team2 = itr_team;
    ++itr_team2;
    //invite players to other selection pool
    for(; itr_team2 != m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + teamIndex].end(); ++itr_team2)
    {
        //if selection pool is full then break;
        if (!(*itr_team2)->IsInvitedToBGInstanceGUID && !m_SelectionPools[otherTeam].AddGroup(*itr_team2, minPlayersPerTeam))
            break;
    }
    if (m_SelectionPools[otherTeam].GetPlayerCount() != minPlayersPerTeam)
        return false;

    //here we have correct 2 selections and we need to change one teams team and move selection pool teams to other team's queue
    for(GroupsQueueType::iterator itr = m_SelectionPools[otherTeam].SelectedGroups.begin(); itr != m_SelectionPools[otherTeam].SelectedGroups.end(); ++itr)
    {
        //set correct team
        (*itr)->Team = otherTeamId;
        //add team to other queue
        m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + otherTeam].push_front(*itr);
        //remove team from old queue
        GroupsQueueType::iterator itr2 = itr_team;
        ++itr2;
        for(; itr2 != m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + teamIndex].end(); ++itr2)
        {
            if (*itr2 == *itr)
            {
                m_QueuedGroups[bracket_id][BG_QUEUE_NORMAL_ALLIANCE + teamIndex].erase(itr2);
                break;
            }
        }
    }
    return true;
}

/*
this method is called when group is inserted, or player / group is removed from BG Queue - there is only one player's status changed, so we don't use while(true) cycles to invite whole queue
it must be called after fully adding the members of a group to ensure group joining
should be called from BattleGround::RemovePlayer function in some cases
*/



void BGQueueNonRated::Update(BattleGroundBracketId bracket_id)
{
    //if no players in queue - do nothing
    if (m_QueuedGroups[bracket_id][BG_QUEUE_ALLIANCE].empty() &&
        m_QueuedGroups[bracket_id][BG_QUEUE_HORDE].empty())
        return;

    //battleground with free slot for player should be always in the beggining of the queue
    // maybe it would be better to create bgfreeslotqueue for each bracket_id
    BGFreeSlotQueueType::iterator itr, next;
    for (itr = sBattleGroundMgr.BGFreeSlotQueue[bgTypeId].begin(); itr != sBattleGroundMgr.BGFreeSlotQueue[bgTypeId].end(); itr = next)
    {
        next = itr;
        ++next;
        // DO NOT allow queue manager to invite new player to arena
        if ( (*itr)->isBattleGround() && (*itr)->GetTypeID() == bgTypeId && (*itr)->GetBracketId() == bracket_id &&
            (*itr)->GetStatus() > STATUS_WAIT_QUEUE && (*itr)->GetStatus() < STATUS_WAIT_LEAVE )
        {
            BattleGround* bg = *itr; //we have to store battleground pointer here, because when battleground is full, it is removed from free queue (not yet implemented!!)
            // and iterator is invalid

            // clear selection pools
            m_SelectionPools[BG_TEAM_ALLIANCE].Init();
            m_SelectionPools[BG_TEAM_HORDE].Init();

            // call a function that does the job for us
            FillPlayersToBG(bg, bracket_id);

            // now everything is set, invite players
            for(GroupsQueueType::const_iterator citr = m_SelectionPools[BG_TEAM_ALLIANCE].SelectedGroups.begin(); citr != m_SelectionPools[BG_TEAM_ALLIANCE].SelectedGroups.end(); ++citr)
                InviteGroupToBG((*citr), bg, (*citr)->Team);
            for(GroupsQueueType::const_iterator citr = m_SelectionPools[BG_TEAM_HORDE].SelectedGroups.begin(); citr != m_SelectionPools[BG_TEAM_HORDE].SelectedGroups.end(); ++citr)
                InviteGroupToBG((*citr), bg, (*citr)->Team);

            if (!bg->HasFreeSlots())
            {
                // remove BG from BGFreeSlotQueue
                bg->RemoveFromBGFreeSlotQueue();
            }
        }
    }

    // finished iterating through the bgs with free slots, maybe we need to create a new bg

    BattleGround * bg_template = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    if (!bg_template)
    {
        sLog.outError("Battleground: Update: bg template not found for %u", bgTypeId);
        return;
    }

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketById(bg_template->GetMapId(),bracket_id);
    if (!bracketEntry)
    {
        sLog.outError("Battleground: Update: bg bracket entry not found for map %u bracket id %u", bg_template->GetMapId(), bracket_id);
        return;
    }

    // get the min. players per team, properly for larger arenas as well. (must have full teams for arena matches!)
    uint32 MinPlayersPerTeam = bg_template->GetMinPlayersPerTeam();
    uint32 MaxPlayersPerTeam = bg_template->GetMaxPlayersPerTeam();
    if (sBattleGroundMgr.isTesting())
        MinPlayersPerTeam = 1;

    if (bg_template->isArena())
    {
        if (sBattleGroundMgr.isArenaTesting())
        {
            MaxPlayersPerTeam = 1;
            MinPlayersPerTeam = 1;
        }
        else
        {
            MaxPlayersPerTeam = arenaType;
            MinPlayersPerTeam = arenaType;
        }
    }

    m_SelectionPools[BG_TEAM_ALLIANCE].Init();
    m_SelectionPools[BG_TEAM_HORDE].Init();

    // PREMADE
    if (bg_template->isBattleGround())
    {
        //check if there is premade against premade match
        if (CheckPremadeMatch(bracket_id, MinPlayersPerTeam, MaxPlayersPerTeam))
        {
            //create new battleground
            BattleGround * bg2 = sBattleGroundMgr.CreateNewBattleGround(bgTypeId, bracketEntry, 0, false);
            if (!bg2)
            {
                sLog.outError("BattleGroundQueue::Update - Cannot create battleground: %u", bgTypeId);
                return;
            }
            //invite those selection pools
            for(uint32 i = 0; i < BG_TEAMS_COUNT; i++)
                for(GroupsQueueType::const_iterator citr = m_SelectionPools[BG_TEAM_ALLIANCE + i].SelectedGroups.begin(); citr != m_SelectionPools[BG_TEAM_ALLIANCE + i].SelectedGroups.end(); ++citr)
                    InviteGroupToBG((*citr), bg2, (*citr)->Team);

            //start bg
            bg2->StartBattleGround();

            //clear structures
            m_SelectionPools[BG_TEAM_ALLIANCE].Init();
            m_SelectionPools[BG_TEAM_HORDE].Init();
        }
    }

    // now check if there are in queues enough players to start new game of (normal battleground, or non-rated arena)
    // SKIRMISH AND NONPREMADES
    // if there are enough players in pools, start new battleground or non rated arena
    if (CheckNormalMatch(bg_template, bracket_id, MinPlayersPerTeam, MaxPlayersPerTeam) ||
       (bg_template->isArena() && CheckSkirmishForSameFaction(bracket_id, MinPlayersPerTeam)) )
    {
        // we successfully created a pool
        BattleGround * bg2 = sBattleGroundMgr.CreateNewBattleGround(bgTypeId, bracketEntry, arenaType, false);
        if (!bg2)
        {
            sLog.outError("BattleGroundQueue::Update - Cannot create battleground: %u", bgTypeId);
            return;
        }

        // invite those selection pools
        for(uint32 i = 0; i < BG_TEAMS_COUNT; i++)
            for(GroupsQueueType::const_iterator citr = m_SelectionPools[BG_TEAM_ALLIANCE + i].SelectedGroups.begin(); citr != m_SelectionPools[BG_TEAM_ALLIANCE + i].SelectedGroups.end(); ++citr)
                InviteGroupToBG((*citr), bg2, (*citr)->Team);

        // start bg
        bg2->StartBattleGround();
    }
}


void BGQueueRated::Update(BattleGroundBracketId bracket_id)
{
    //if no players in queue - do nothing
    if (m_QueuedRatedArenas[bracket_id].empty())
        return;

    BattleGround * bg_template = sBattleGroundMgr.GetBattleGroundTemplate(BATTLEGROUND_AA);
    if (!bg_template)
    {
        sLog.outError("Battleground: Update: bg template not found for %u", BATTLEGROUND_AA);
        return;
    }

    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketById(bg_template->GetMapId(),bracket_id);
    if (!bracketEntry)
    {
        sLog.outError("Battleground: Update: bg bracket entry not found for map %u bracket id %u", bg_template->GetMapId(), bracket_id);
        return;
    }

    // get the min. players per team, properly for larger arenas as well. (must have full teams for arena matches!)
    m_SelectionPools[BG_TEAM_ALLIANCE].Init();
    m_SelectionPools[BG_TEAM_HORDE].Init();


    /************************
    *** RATED ARENAS ONLY ***
    ************************/

    uint32 now = getMSTime();

    // update queues
    for(GroupsQueueType::iterator itr_team = m_QueuedRatedArenas[bracket_id].begin(); itr_team != m_QueuedRatedArenas[bracket_id].end(); ++itr_team)
    {
        GroupQueueInfo* ginfo = *itr_team;

        if (ginfo->DiscartedTime)   // if discarted, dont update
            continue;

        uint32 timeInQueue = getMSTimeDiff(ginfo->JoinTime, now);
        if (timeInQueue > sBattleGroundMgr.GetRatingDiscardTimer())
        {
            sLog.outCatLog("BGQ::Update:: Discarting arena team %u after %u of wait time", ginfo->ArenaTeamId, getMSTime()-ginfo->JoinTime);
            ginfo->DiscartedTime = now;
            continue;
        }
        else if (timeInQueue > sBattleGroundMgr.GetStepAddStartTimer())
        {
            if (!ginfo->LastUpdatedTime || getMSTimeDiff(ginfo->LastUpdatedTime, now) > sBattleGroundMgr.GetStepInterval())
            {
                ginfo->CurrentMaxChanceDiff += sBattleGroundMgr.GetChanceAddOnStep();
                ginfo->LastUpdatedTime = now;
            }
        }
    }

    // find groups to matchup
    for(GroupsQueueType::iterator itr_team1 = m_QueuedRatedArenas[bracket_id].begin(); itr_team1 != m_QueuedRatedArenas[bracket_id].end(); ++itr_team1)
    {
        GroupQueueInfo* ginfo = *itr_team1;
        GroupQueueInfo* minfo = NULL;

        // group already set
        if (ginfo->IsAlreadySet())
            continue;

        for(GroupsQueueType::iterator itr_team2 = m_QueuedRatedArenas[bracket_id].begin(); itr_team2 != m_QueuedRatedArenas[bracket_id].end(); ++itr_team2)
        {
            GroupQueueInfo* ginfo2 = *itr_team2;

            // skip if found team is same all is already set
            if (ginfo->ArenaTeamId == ginfo2->ArenaTeamId || ginfo2->IsAlreadySet())
                continue;

            // both teams have satisfactory chance or are both discarted
            if ((ginfo->ChanceOK(ginfo2->ArenaTeamMMR) && ginfo2->ChanceOK(ginfo->ArenaTeamMMR)) ||
                (ginfo->DiscartedTime && ginfo2->DiscartedTime))
            {
                minfo = ginfo2;
                break;
            }
        }

        // if no other group is discarted and ginfo1 group is discarted for too long, look for another group
        //(TODO: value to config)
        if (!minfo && ginfo->DiscartedTime > 90000)
        {
            // look up for first avaliable group in queue
            for(GroupsQueueType::iterator itr_disc = m_QueuedRatedArenas[bracket_id].begin(); itr_disc != m_QueuedRatedArenas[bracket_id].end(); ++itr_disc)
            {
                GroupQueueInfo* ginfo2 = *itr_disc;

                // skip if found team is same all is already set
                if (ginfo->ArenaTeamId != ginfo2->ArenaTeamId && !ginfo2->IsAlreadySet())
                {
                    minfo = ginfo2;
                    break;
                }
            }
        }

        if (minfo)
            StartRatedArena(ginfo, minfo, bracketEntry, arenaType);
    }
}

void BGQueueRated::StartRatedArena(GroupQueueInfo *ginfo1, GroupQueueInfo *ginfo2, const PvPDifficultyEntry *bracketEntry, uint8 arenaType)
{
    BattleGround* arena = sBattleGroundMgr.CreateNewBattleGround(BATTLEGROUND_AA, bracketEntry, arenaType, true);
    if (!arena)
    {
        sLog.outError("BattlegroundQueue::Update couldn't create arena instance for rated arena match!");
        return;
    }

    m_SelectionPools[BG_TEAM_ALLIANCE].AddGroup(ginfo1, arenaType);
    m_SelectionPools[BG_TEAM_HORDE].AddGroup(ginfo2, arenaType);

    ginfo1->OpponentTeamId = ginfo2->ArenaTeamId;
    ginfo1->OpponentsMMR = ginfo2->ArenaTeamMMR;

    ginfo2->OpponentTeamId = ginfo1->ArenaTeamId;
    ginfo2->OpponentsMMR = ginfo1->ArenaTeamMMR;

    DEBUG_LOG("BattleGroundQueue:: Setting opposite team rating for team %u to %u", ginfo1->ArenaTeamId, ginfo1->OpponentsMMR);
    DEBUG_LOG("BattleGroundQueue:: Setting opposite team rating for team %u to %u", ginfo2->ArenaTeamId, ginfo2->OpponentsMMR);

    arena->SetArenaMatchmakerRating(ALLIANCE, ginfo1->ArenaTeamMMR);
    arena->SetArenaMatchmakerRating(HORDE, ginfo2->ArenaTeamMMR);

    InviteGroupToBG(ginfo1, arena, ALLIANCE);
    InviteGroupToBG(ginfo2, arena, HORDE);

    DEBUG_LOG("Starting rated arena match!");

    arena->StartBattleGround();

    m_SelectionPools[BG_TEAM_ALLIANCE].Init();
    m_SelectionPools[BG_TEAM_HORDE].Init();
}








/*********************************************************/
/***            BATTLEGROUND QUEUE EVENTS              ***/
/*********************************************************/

bool BGQueueInviteEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    Player* plr = sObjectMgr.GetPlayer( m_PlayerGuid );
    // player logged off (we should do nothing, he is correctly removed from queue in another procedure)
    if (!plr)
        return true;

    BattleGround* bg = sBattleGroundMgr.GetBattleGround(m_BgInstanceGUID, m_BgTypeId);
    //if battleground ended and its instance deleted - do nothing
    if (!bg)
        return true;

    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(bg->GetTypeID(), bg->GetArenaType());
    uint32 queueSlot = plr->GetBattleGroundQueueIndex(bgQueueTypeId);
    if (queueSlot < PLAYER_MAX_BATTLEGROUND_QUEUES)         // player is in queue or in battleground
    {
        // check if player is invited to this bg
        BattleGroundQueue &bgQueue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];
        if (bgQueue.IsPlayerInvited(m_PlayerGuid, m_BgInstanceGUID, m_RemoveTime))
        {
            WorldPacket data;
            //we must send remaining time in queue
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_JOIN, INVITE_ACCEPT_WAIT_TIME - INVITATION_REMIND_TIME, 0, m_ArenaType);
            plr->GetSession()->SendPacket(&data);
        }
    }
    return true;                                            //event will be deleted
}

void BGQueueInviteEvent::Abort(uint64 /*e_time*/)
{
    //do nothing
}

bool BGQueueRemoveEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    Player* plr = sObjectMgr.GetPlayer( m_PlayerGuid );
    if (!plr)
        // player logged off (we should do nothing, he is correctly removed from queue in another procedure)
        return true;

    BattleGround* bg = sBattleGroundMgr.GetBattleGround(m_BgInstanceGUID, m_BgTypeId);
    //battleground can be deleted already when we are removing queue info
    //bg pointer can be NULL! so use it carefully!

    uint32 queueSlot = plr->GetBattleGroundQueueIndex(m_BgQueueTypeId);
    if (queueSlot < PLAYER_MAX_BATTLEGROUND_QUEUES)         // player is in queue, or in Battleground
    {
        // check if player is in queue for this BG and if we are removing his invite event
        BattleGroundQueue &bgQueue = sBattleGroundMgr.m_BattleGroundQueues[m_BgQueueTypeId];
        if (bgQueue.IsPlayerInvited(m_PlayerGuid, m_BgInstanceGUID, m_RemoveTime))
        {
            DEBUG_LOG("Battleground: removing player %u from bg queue for instance %u because of not pressing enter battle in time.",plr->GetGUIDLow(),m_BgInstanceGUID);

            plr->RemoveBattleGroundQueueId(m_BgQueueTypeId);
            bgQueue.RemovePlayer(m_PlayerGuid, true);
            //update queues if battleground isn't ended
            //if (bg && bg->isBattleGround() && bg->GetStatus() != STATUS_WAIT_LEAVE)
            //    sBattleGroundMgr.ScheduleQueueUpdate(0, 0, m_BgQueueTypeId, m_BgTypeId, bg->GetBracketId());

            WorldPacket data;
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_NONE, 0, 0, 0);
            plr->GetSession()->SendPacket(&data);
        }
    }

    //event will be deleted
    return true;
}

void BGQueueRemoveEvent::Abort(uint64 /*e_time*/)
{
    //do nothing
}
