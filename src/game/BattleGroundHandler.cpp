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
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "Player.h"
#include "ObjectMgr.h"
#include "WorldSession.h"
#include "Object.h"
#include "Chat.h"
#include "BattleGroundMgr.h"
#include "BattleGroundWS.h"
#include "BattleGround.h"
#include "ArenaTeam.h"
#include "Language.h"
#include "ScriptCalls.h"
#include "GameEventMgr.h"

void WorldSession::HandleBattlemasterHelloOpcode(WorldPacket & recv_data)
{
    uint64 guid;
    recv_data >> guid;

    DEBUG_LOG("WORLD: Recvd CMSG_BATTLEMASTER_HELLO Message from (GUID: %u TypeId:%u)", GUID_LOPART(guid),GuidHigh2TypeId(GUID_HIPART(guid)));

    Creature *pCreature = GetPlayer()->GetMap()->GetCreature(guid);

    if (!pCreature)
        return;

    if (!pCreature->isBattleMaster())                       // it's not battlemaster
        return;

    // Stop the npc if moving
    if (!pCreature->IsStopped())
        pCreature->StopMoving();

    BattleGroundTypeId bgTypeId = sBattleGroundMgr.GetBattleMasterBG(pCreature->GetEntry());

    if (bgTypeId == BATTLEGROUND_TYPE_NONE)
        return;

    if (!_player->GetBGAccessByLevel(bgTypeId))
    {
        // temp, must be gossip message...
        SendNotification(LANG_YOUR_BG_LEVEL_REQ_ERROR);
        return;
    }

    SendBattlegGroundList(guid, bgTypeId);
}

void WorldSession::SendBattlegGroundList( uint64 guid, BattleGroundTypeId bgTypeId )
{
    WorldPacket data;
    sBattleGroundMgr.BuildBattleGroundListPacket(&data, guid, _player, bgTypeId, 0);
    SendPacket( &data );
}

void WorldSession::HandleBattlemasterJoinOpcode( WorldPacket & recv_data )
{
    uint64 guid;
    uint32 bgTypeId_;
    uint32 instanceId;
    uint8 joinAsGroup;
    bool isPremade = false;
    Group * grp;

    recv_data >> guid;                                      // battlemaster guid
    recv_data >> bgTypeId_;                                 // battleground type id (DBC id)
    recv_data >> instanceId;                                // instance id, 0 if First Available selected
    recv_data >> joinAsGroup;                               // join as group

    if (!sBattlemasterListStore.LookupEntry(bgTypeId_))
    {
        sLog.outError("Battleground: invalid bgtype (%u) received. possible cheater? player guid %u",bgTypeId_,_player->GetGUIDLow());
        return;
    }

    BattleGroundTypeId bgTypeId = BattleGroundTypeId(bgTypeId_);

    DEBUG_LOG( "WORLD: Recvd CMSG_BATTLEMASTER_JOIN Message from (GUID: %u TypeId:%u)", GUID_LOPART(guid), GuidHigh2TypeId(GUID_HIPART(guid)));

    // can do this, since it's battleground, not arena
    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(bgTypeId, 0);
    BattleGroundQueueTypeId bgQueueTypeIdRandom = BattleGroundMgr::BGQueueTypeId(BATTLEGROUND_RB, 0);

    // ignore if player is already in BG
    if (_player->InBattleGround())
        return;

    // get bg instance or bg template if instance not found
    BattleGround *bg = NULL;
    if (instanceId)
        bg = sBattleGroundMgr.GetBattleGroundThroughClientInstance(instanceId, bgTypeId);

    if (!bg && !(bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId)))
    {
        sLog.outError("Battleground: no available bg / template found");
        return;
    }

    // expected bracket entry
    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(),_player->getLevel());
    if (!bracketEntry)
        return;

    GroupJoinBattlegroundResult err;

    // check queue conditions
    if (!joinAsGroup)
    {
        // Cant join while in LFG
        if (!_player->m_lookingForGroup.queuedDungeons.empty())
        {
            WorldPacket data;
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, ERR_LFG_CANT_USE_BATTLEGROUND);
            _player->GetSession()->SendPacket(&data);
            return;
        }

        // check Deserter debuff
        if (!_player->CanJoinToBattleground())
        {
            WorldPacket data;
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS);
            _player->GetSession()->SendPacket(&data);
            return;
        }

        if (_player->GetBattleGroundQueueIndex(bgQueueTypeIdRandom) < PLAYER_MAX_BATTLEGROUND_QUEUES)
        {
            //player is already in random queue
            WorldPacket data;
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, ERR_IN_RANDOM_BG);
            _player->GetSession()->SendPacket(&data);
            return;
        }
        
        if (_player->InBattleGroundQueue() && bgTypeId == BATTLEGROUND_RB)
        {
            //player is already in queue, can't start random queue
            WorldPacket data;
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, ERR_IN_NON_RANDOM_BG);
            _player->GetSession()->SendPacket(&data);
            return;
        }

        // check if already in queue
        if (_player->GetBattleGroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            //player is already in this queue
            return;

        // check if has free queue slots
        if (!_player->HasFreeBattleGroundQueueId())
        {
            WorldPacket data;
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, ERR_BATTLEGROUND_TOO_MANY_QUEUES);
            _player->GetSession()->SendPacket(&data);
            return;
        }
    }
    else
    {
        grp = _player->GetGroup();
        // no group found, error
        if (!grp)
            return;
        if (grp->GetLeaderGUID() != _player->GetGUID())
            return;
        err = grp->CanJoinBattleGroundQueue(bg, bgQueueTypeId, 0, bg->GetMaxPlayersPerTeam(), false, 0);
        isPremade = (grp->GetMembersCount() >= bg->GetMinPlayersPerTeam());
    }
    // if we're here, then the conditions to join a bg are met. We can proceed in joining.

    // _player->GetGroup() was already checked, grp is already initialized
    BattleGroundQueue& bgQueue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];
    if (joinAsGroup)
    {
        GroupQueueInfo * ginfo;
        uint32 avgTime;

        if (err > 0)
        {
            DEBUG_LOG("Battleground: the following players are joining as group:");
            ginfo = bgQueue.AddGroup(_player, grp, bgTypeId, bracketEntry, 0, false, isPremade, 0);
            avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
        }

        for(GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player *member = itr->getSource();
            if (!member)
                continue;                                   // this should never happen

            WorldPacket data;

            if (err <= 0)
            {
                sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, err);
                member->GetSession()->SendPacket(&data);
                continue;
            }

            // add to queue
            uint32 queueSlot = member->AddBattleGroundQueueId(bgQueueTypeId);

            // send status packet (in queue)
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0, ginfo->ArenaType);
            member->GetSession()->SendPacket(&data);
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, err);
            member->GetSession()->SendPacket(&data);
            DEBUG_LOG("Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s",bgQueueTypeId,bgTypeId,member->GetGUIDLow(), member->GetName());
        }
        DEBUG_LOG("Battleground: group end");
    }
    else
    {
        GroupQueueInfo * ginfo = bgQueue.AddGroup(_player, NULL, bgTypeId, bracketEntry, 0, false, isPremade, 0);
        uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
        // already checked if queueSlot is valid, now just get it
        uint32 queueSlot = _player->AddBattleGroundQueueId(bgQueueTypeId);

        WorldPacket data;
                                                            // send status packet (in queue)
        sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0, ginfo->ArenaType);
        SendPacket(&data);
        DEBUG_LOG("Battleground: player joined queue for bg queue type %u bg type %u: GUID %u, NAME %s",bgQueueTypeId,bgTypeId,_player->GetGUIDLow(), _player->GetName());
    }
    //sBattleGroundMgr.ScheduleQueueUpdate(0, 0, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());
}

void WorldSession::HandleBattleGroundPlayerPositionsOpcode( WorldPacket & /*recv_data*/ )
{
                                                            // empty opcode
    DEBUG_LOG("WORLD: Recvd MSG_BATTLEGROUND_PLAYER_POSITIONS Message");

    BattleGround *bg = _player->GetBattleGround();
    if (!bg)                                                 // can't be received if player not in battleground
        return;

    switch( bg->GetTypeID(true) )
    {
        case BATTLEGROUND_WS:
            {
                uint32 count1 = 0;                          // always constant zero?
                uint32 count2 = 0;                          // count of next fields

                Player *ali_plr = sObjectMgr.GetPlayer(((BattleGroundWS*)bg)->GetAllianceFlagPickerGUID());
                if (ali_plr)
                    ++count2;

                Player *horde_plr = sObjectMgr.GetPlayer(((BattleGroundWS*)bg)->GetHordeFlagPickerGUID());
                if (horde_plr)
                    ++count2;

                WorldPacket data(MSG_BATTLEGROUND_PLAYER_POSITIONS, (4+4+16*count1+16*count2));
                data << count1;                                     // alliance flag holders count - obsolete, now always 0
                /*for(uint8 i = 0; i < count1; ++i)
                {
                    data << uint64(0);                              // guid
                    data << (float)0;                               // x
                    data << (float)0;                               // y
                }*/
                data << count2;                                     // horde flag holders count - obsolete, now count of next fields
                if (ali_plr)
                {
                    data << (uint64)ali_plr->GetGUID();
                    data << (float)ali_plr->GetPositionX();
                    data << (float)ali_plr->GetPositionY();
                }
                if (horde_plr)
                {
                    data << (uint64)horde_plr->GetGUID();
                    data << (float)horde_plr->GetPositionX();
                    data << (float)horde_plr->GetPositionY();
                }

                SendPacket(&data);
            }
            break;
        case BATTLEGROUND_EY:
            //TODO : fix me!
            break;
        case BATTLEGROUND_AB:
        case BATTLEGROUND_AV:
            {
                //for other BG types - send default
                WorldPacket data(MSG_BATTLEGROUND_PLAYER_POSITIONS, (4+4));
                data << uint32(0);
                data << uint32(0);
                SendPacket(&data);
            }
            break;
        default:
            //maybe it is sent also in arena - do nothing
            break;
    }
}

void WorldSession::HandlePVPLogDataOpcode( WorldPacket & /*recv_data*/ )
{
    DEBUG_LOG( "WORLD: Recvd MSG_PVP_LOG_DATA Message");

    BattleGround *bg = _player->GetBattleGround();
    if (!bg)
        return;

    WorldPacket data;
    sBattleGroundMgr.BuildPvpLogDataPacket(&data, bg);
    SendPacket(&data);

    DEBUG_LOG( "WORLD: Sent MSG_PVP_LOG_DATA Message");
}

void WorldSession::HandleBattlefieldListOpcode( WorldPacket &recv_data )
{
    DEBUG_LOG( "WORLD: Recvd CMSG_BATTLEFIELD_LIST Message");

    uint32 bgTypeId;
    recv_data >> bgTypeId;                                  // id from DBC

    uint8 fromWhere;
    recv_data >> fromWhere;                                 // 0 - battlemaster (lua: ShowBattlefieldList), 1 - UI (lua: RequestBattlegroundInstanceInfo)

    uint8 unk1;
    recv_data >> unk1;                                      // unknown 3.2.2

    BattlemasterListEntry const* bl = sBattlemasterListStore.LookupEntry(bgTypeId);
    if (!bl)
    {
        sLog.outError("Battleground: invalid bgtype received.");
        return;
    }

    WorldPacket data;
    sBattleGroundMgr.BuildBattleGroundListPacket(&data, 0, _player, BattleGroundTypeId(bgTypeId), fromWhere);
    SendPacket( &data );
}

void WorldSession::HandleBattleFieldPortOpcode( WorldPacket &recv_data )
{
    DEBUG_LOG( "WORLD: Recvd CMSG_BATTLEFIELD_PORT Message");

    uint8 type;                                             // arenatype if arena
    uint8 unk2;                                             // unk, can be 0x0 (may be if was invited?) and 0x1
    uint32 bgTypeId_;                                       // type id from dbc
    uint16 unk;                                             // 0x1F90 constant?
    uint8 action;                                           // enter battle 0x1, leave queue 0x0

    recv_data >> type >> unk2 >> bgTypeId_ >> unk >> action;

    if (!sBattlemasterListStore.LookupEntry(bgTypeId_))
    {
        sLog.outError("BattlegroundHandler: invalid bgtype (%u) received.", bgTypeId_);
        return;
    }
    if (!_player->InBattleGroundQueue())
    {
        sLog.outError("BattlegroundHandler: Invalid CMSG_BATTLEFIELD_PORT received from player (%u), he is not in bg_queue.", _player->GetGUIDLow());
        return;
    }

    //get GroupQueueInfo from BattleGroundQueue
    BattleGroundTypeId bgTypeId = BattleGroundTypeId(bgTypeId_);
    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(bgTypeId, type);
    BattleGroundQueue& bgQueue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];
    //we must use temporary variable, because GroupQueueInfo pointer can be deleted in BattleGroundQueue::RemovePlayer() function
    GroupQueueInfo ginfo;
    if (!bgQueue.GetPlayerGroupInfoData(_player->GetGUID(), &ginfo))
    {
        sLog.outError("BattlegroundHandler: itrplayerstatus not found.");
        return;
    }
    // if action == 1, then instanceId is required
    if (!ginfo.IsInvitedToBGInstanceGUID && action == 1)
    {
        sLog.outError("BattlegroundHandler: instance not found.");
        return;
    }

    BattleGround *bg = sBattleGroundMgr.GetBattleGround(ginfo.IsInvitedToBGInstanceGUID, bgTypeId);

    // bg template might and must be used in case of leaving queue, when instance is not created yet
    if (!bg && action == 0)
        bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
    if (!bg)
    {
        sLog.outError("BattlegroundHandler: bg_template not found for type id %u.", bgTypeId);
        return;
    }

    // expected bracket entry
    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(),_player->getLevel());
    if (!bracketEntry)
        return;

    //some checks if player isn't cheating - it is not exactly cheating, but we cannot allow it
    if (action == 1 && ginfo.ArenaType == 0)
    {
        //if player is trying to enter battleground (not arena!) and he has deserter debuff, we must just remove him from queue
        if (!_player->CanJoinToBattleground())
        {
            //send bg command result to show nice message
            WorldPacket data2;
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data2, ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS);
            _player->GetSession()->SendPacket(&data2);
            action = 0;
            DEBUG_LOG("Battleground: player %s (%u) has a deserter debuff, do not port him to battleground!", _player->GetName(), _player->GetGUIDLow());
        }
        //if player don't match battleground max level, then do not allow him to enter! (this might happen when player leveled up during his waiting in queue
        if (_player->getLevel() > bg->GetMaxLevel())
        {
            sLog.outError("Battleground: Player %s (%u) has level (%u) higher than maxlevel (%u) of battleground (%u)! Do not port him to battleground!",
                _player->GetName(), _player->GetGUIDLow(), _player->getLevel(), bg->GetMaxLevel(), bg->GetTypeID());
            action = 0;
        }
    }
    uint32 queueSlot = _player->GetBattleGroundQueueIndex(bgQueueTypeId);
    WorldPacket data;
    switch( action )
    {
        case 1:                                         // port to battleground
            if (!_player->IsInvitedForBattleGroundQueueType(bgQueueTypeId))
                return;                                 // cheating?

            if (!_player->InBattleGround())
                _player->SetBattleGroundEntryPoint();

            // resurrect the player
            if (!_player->isAlive())
            {
                _player->ResurrectPlayer(1.0f);
                _player->SpawnCorpseBones();
            }
            // stop taxi flight at port
            if (_player->isInFlight())
            {
                _player->GetMotionMaster()->MovementExpired(false);
                _player->m_taxi.ClearTaxiDestinations();
            }

            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_IN_PROGRESS, 0, bg->GetStartTime(), bg->GetArenaType());
            _player->GetSession()->SendPacket(&data);
            // remove battleground queue status from BGmgr
            bgQueue.RemovePlayer(_player->GetGUID(), false);
            // this is still needed here if battleground "jumping" shouldn't add deserter debuff
            // also this is required to prevent stuck at old battleground after SetBattleGroundId set to new
            if (BattleGround *currentBg = _player->GetBattleGround())
                currentBg->RemovePlayerAtLeave(_player->GetGUID(), false, true);

            // set the destination instance id
            _player->SetBattleGroundId(bg->GetInstanceID(), bgTypeId);
            // set the destination team
            _player->SetBGTeam(ginfo.Team);
            // bg->HandleBeforeTeleportToBattleGround(_player);
            sBattleGroundMgr.SendToBattleGround(_player, ginfo.IsInvitedToBGInstanceGUID, bgTypeId);
            // add only in HandleMoveWorldPortAck()
            // bg->AddPlayer(_player,team);
            DEBUG_LOG("Battleground: player %s (%u) joined battle for bg %u, bgtype %u, queue type %u.", _player->GetName(), _player->GetGUIDLow(), bg->GetInstanceID(), bg->GetTypeID(), bgQueueTypeId);
            break;
        case 0:                                         // leave queue
            // if player leaves rated arena match before match start, it is counted as he played but he lost
            if (ginfo.IsRated)
            {
                ArenaTeam * at = sObjectMgr.GetArenaTeamById(ginfo.Team);
                if (at)
                {
                    DEBUG_LOG("UPDATING memberLost's personal arena rating for %u by opponents rating: %u, because he has left queue!", GUID_LOPART(_player->GetGUID()), ginfo.OpponentsMMR);
                    at->MemberPlayed(_player, ginfo.OpponentsMMR, false);
                    at->SaveToDB();
                }
            }
            _player->RemoveBattleGroundQueueId(bgQueueTypeId);  // must be called this way, because if you move this call to queue->removeplayer, it causes bugs
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_NONE, 0, 0, 0);
            bgQueue.RemovePlayer(_player->GetGUID(), true);
            // player left queue, we should update it - do not update Arena Queue
            //if (!ginfo.ArenaType)
            //    sBattleGroundMgr.ScheduleQueueUpdate(ginfo.ArenaTeamRating, ginfo.ArenaType, bgQueueTypeId, bgTypeId, bracketEntry->GetBracketId());
            SendPacket(&data);
            DEBUG_LOG("Battleground: player %s (%u) left queue for bgtype %u, queue type %u.", _player->GetName(), _player->GetGUIDLow(), bg->GetTypeID(), bgQueueTypeId);
            break;
        default:
            sLog.outError("Battleground port: unknown action %u", action);
            break;
    }
}

void WorldSession::HandleLeaveBattlefieldOpcode( WorldPacket& recv_data )
{
    DEBUG_LOG( "WORLD: Recvd CMSG_LEAVE_BATTLEFIELD Message");

    recv_data.read_skip<uint8>();                           // unk1
    recv_data.read_skip<uint8>();                           // unk2
    recv_data.read_skip<uint32>();                          // BattleGroundTypeId
    recv_data.read_skip<uint16>();                          // unk3

    //if (bgTypeId >= MAX_BATTLEGROUND_TYPES)                  // cheating? but not important in this case
    //    return;

    // not allow leave battleground in combat
    if (_player->isInCombat())
        if (BattleGround* bg = _player->GetBattleGround())
            if (bg->GetStatus() != STATUS_WAIT_LEAVE)
                return;

    _player->LeaveBattleground();
}

void WorldSession::HandleBattlefieldStatusOpcode( WorldPacket & /*recv_data*/ )
{
    // empty opcode
    DEBUG_LOG( "WORLD: Battleground status" );

    WorldPacket data;
    // we must update all queues here
    BattleGround *bg = NULL;
    for (uint8 i = 0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
    {
        BattleGroundQueueTypeId bgQueueTypeId = _player->GetBattleGroundQueueTypeId(i);
        if (!bgQueueTypeId)
            continue;
        BattleGroundTypeId bgTypeId = BattleGroundMgr::BGTemplateId(bgQueueTypeId);
        uint8 arenaType = BattleGroundMgr::BGArenaType(bgQueueTypeId);
        if (bgTypeId == _player->GetBattleGroundTypeId())
        {
            bg = _player->GetBattleGround();
            //i cannot check any variable from player class because player class doesn't know if player is in 2v2 / 3v3 or 5v5 arena
            //so i must use bg pointer to get that information
            if (bg && bg->GetArenaType() == arenaType)
            {
                // this line is checked, i only don't know if GetStartTime is changing itself after bg end!
                // send status in BattleGround
                sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, i, STATUS_IN_PROGRESS, bg->GetEndTime(), bg->GetStartTime(), arenaType);
                SendPacket(&data);
                continue;
            }
        }
        //we are sending update to player about queue - he can be invited there!
        //get GroupQueueInfo for queue status
        BattleGroundQueue& bgQueue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];
        GroupQueueInfo ginfo;
        if (!bgQueue.GetPlayerGroupInfoData(_player->GetGUID(), &ginfo))
            continue;
        if (ginfo.IsInvitedToBGInstanceGUID)
        {
            bg = sBattleGroundMgr.GetBattleGround(ginfo.IsInvitedToBGInstanceGUID, bgTypeId);
            if (!bg)
                continue;
            uint32 remainingTime = getMSTimeDiff(getMSTime(), ginfo.RemoveInviteTime);
            // send status invited to BattleGround
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, i, STATUS_WAIT_JOIN, remainingTime, 0, arenaType);
            SendPacket(&data);
        }
        else
        {
            bg = sBattleGroundMgr.GetBattleGroundTemplate(bgTypeId);
            if (!bg)
                continue;

            // expected bracket entry
            PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(),_player->getLevel());
            if (!bracketEntry)
                continue;

            uint32 avgTime = bgQueue.GetAverageQueueWaitTime(&ginfo, bracketEntry->GetBracketId());
            // send status in BattleGround Queue
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, i, STATUS_WAIT_QUEUE, avgTime, getMSTimeDiff(ginfo.JoinTime, getMSTime()), arenaType);
            SendPacket(&data);
        }
    }
}

void WorldSession::HandleAreaSpiritHealerQueryOpcode( WorldPacket & recv_data )
{
    DEBUG_LOG("WORLD: CMSG_AREA_SPIRIT_HEALER_QUERY");

    BattleGround *bg = _player->GetBattleGround();
    if (!bg)
        return;

    uint64 guid;
    recv_data >> guid;

    Creature *unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isSpiritService())                            // it's not spirit service
        return;

    unit->SendAreaSpiritHealerQueryOpcode(GetPlayer());
}

void WorldSession::HandleAreaSpiritHealerQueueOpcode( WorldPacket & recv_data )
{
    DEBUG_LOG("WORLD: CMSG_AREA_SPIRIT_HEALER_QUEUE");

    BattleGround *bg = _player->GetBattleGround();
    if (!bg)
        return;

    uint64 guid;
    recv_data >> guid;

    Creature *unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isSpiritService())                            // it's not spirit service
        return;

    Script->GossipHello(GetPlayer(), unit);
}

void WorldSession::HandleBattlemasterJoinArena( WorldPacket & recv_data )
{
    DEBUG_LOG("WORLD: CMSG_BATTLEMASTER_JOIN_ARENA");
    //recv_data.hexlike();

    uint64 guid;                                            // arena Battlemaster guid
    uint8 arenaslot;                                        // 2v2, 3v3 or 5v5
    uint8 asGroup;                                          // asGroup
    uint8 isRated;                                          // isRated
    Group * grp;

    recv_data >> guid >> arenaslot >> asGroup >> isRated;

    // ignore if we already in BG or BG queue
    if (_player->InBattleGround())
        return;

    Creature *unit = GetPlayer()->GetMap()->GetCreature(guid);
    if (!unit)
        return;

    if (!unit->isBattleMaster())                             // it's not battle master
        return;

    uint8 arenatype = 0;
    uint32 arenaRating = 0;
    uint32 arenaMMR = 0;

    switch(arenaslot)
    {
        case 0:
            arenatype = ARENA_TYPE_2v2;
            break;
        case 1:
            arenatype = ARENA_TYPE_3v3;
            break;
        case 2:
            arenatype = ARENA_TYPE_5v5;
            break;
        default:
            sLog.outError("Unknown arena slot %u at HandleBattlemasterJoinArena()", arenaslot);
            return;
    }

    // is current arena type active
    if (isRated && arenatype != ARENA_TYPE_5v5)
    {
        if (!sGameEventMgr.IsActiveEventForArena(arenatype))
        {
            SendNotification("This arena bracket is not allowed right now");
            ChatHandler(GetPlayer()).SendSysMessage("Check avalible arena types by typing .arenainfo");
            return;
        }
    }

    // check existence
    BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(BATTLEGROUND_AA);
    if (!bg)
    {
        sLog.outError("Battleground: template bg (all arenas) not found");
        return;
    }

    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(BATTLEGROUND_AA, arenatype);
    PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(),_player->getLevel());
    if (!bracketEntry)
        return;

    GroupJoinBattlegroundResult err;

    // check queue conditions
    if (!asGroup)
    {
        // check if already in queue
        if (_player->GetBattleGroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            //player is already in this queue
            return;
        // check if has free queue slots
        if (!_player->HasFreeBattleGroundQueueId())
            return;
    }
    else
    {
        grp = _player->GetGroup();
        // no group found, error
        if (!grp)
            return;
        if (grp->GetLeaderGUID() != _player->GetGUID())
            return;
        // may be Group::CanJoinBattleGroundQueue should be moved to player class...
        err = grp->CanJoinBattleGroundQueue(bg, bgQueueTypeId, arenatype, arenatype, (bool)isRated, arenaslot);
    }

    uint32 ateamId = 0;

    if (isRated)
    {
        ateamId = _player->GetArenaTeamId(arenaslot);
        // check real arena team existence only here (if it was moved to group->CanJoin .. () then we would have to get it twice)
        ArenaTeam * at = sObjectMgr.GetArenaTeamById(ateamId);
        if (!at)
        {
            _player->GetSession()->SendNotInArenaTeamPacket(arenatype);
            return;
        }
        arenaRating = at->GetRating();
        arenaMMR = grp->GetAverageMMR(arenaslot);

    }

    BattleGroundQueue &bgQueue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];
    if (asGroup)
    {
        uint32 avgTime;

        if (err > 0)
        {
            DEBUG_LOG("Battleground: arena join as group start");
            if (isRated)
                DEBUG_LOG("Battleground: arena team id %u, leader %s queued with rating %u for type %u",_player->GetArenaTeamId(arenaslot),_player->GetName(),arenaRating,arenatype);

            GroupQueueInfo * ginfo = bgQueue.AddGroup(_player, grp, BATTLEGROUND_AA, bracketEntry, arenatype, isRated, false, arenaRating, ateamId, arenaMMR);
            avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
        }

        for(GroupReference *itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player *member = itr->getSource();
            if (!member)
                continue;

            WorldPacket data;

            if (err <= 0)
            {
                sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, err);
                member->GetSession()->SendPacket(&data);
                continue;
            }

            // add to queue
            uint32 queueSlot = member->AddBattleGroundQueueId(bgQueueTypeId);

            // send status packet (in queue)
            sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0, arenatype);
            member->GetSession()->SendPacket(&data);
            sBattleGroundMgr.BuildGroupJoinedBattlegroundPacket(&data, err);
            member->GetSession()->SendPacket(&data);
            DEBUG_LOG("Battleground: player joined queue for arena as group bg queue type %u bg type %u: GUID %u, NAME %s", bgQueueTypeId, BATTLEGROUND_AA, member->GetGUIDLow(), member->GetName());
        }
        DEBUG_LOG("Battleground: arena join as group end");
    }
    else
    {
        GroupQueueInfo * ginfo = bgQueue.AddGroup(_player, NULL, BATTLEGROUND_AA, bracketEntry, arenatype, isRated, false, arenaRating, ateamId);
        uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo, bracketEntry->GetBracketId());
        uint32 queueSlot = _player->AddBattleGroundQueueId(bgQueueTypeId);

        WorldPacket data;
        // send status packet (in queue)
        sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0, arenatype);
        SendPacket(&data);
        DEBUG_LOG("Battleground: player joined queue for arena, skirmish, bg queue type %u bg type %u: GUID %u, NAME %s",bgQueueTypeId,BATTLEGROUND_AA,_player->GetGUIDLow(), _player->GetName());
    }
    //sBattleGroundMgr.ScheduleQueueUpdate(arenaRating, arenatype, bgQueueTypeId, BATTLEGROUND_AA, bracketEntry->GetBracketId());
}

void WorldSession::HandleReportPvPAFK( WorldPacket & recv_data )
{
    uint64 playerGuid;
    recv_data >> playerGuid;
    Player *reportedPlayer = sObjectMgr.GetPlayer(playerGuid);

    if (!reportedPlayer)
    {
        DEBUG_LOG("WorldSession::HandleReportPvPAFK: player not found");
        return;
    }

    DEBUG_LOG("WorldSession::HandleReportPvPAFK: %s reported %s", _player->GetName(), reportedPlayer->GetName());

    reportedPlayer->ReportedAfkBy(_player);
}
