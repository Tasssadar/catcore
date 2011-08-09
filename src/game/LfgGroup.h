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

#ifndef __LFGGROUP_H
#define __LFGGROUP_H

#include "Common.h"
#include "Policies/Singleton.h"
#include "Utilities/EventProcessor.h"
#include "DBCEnums.h"
#include "Group.h"
#include "LfgMgr.h"
#include "ace/Recursive_Thread_Mutex.h"

#define LFG_VOTES_NEEDED 3

typedef std::map<uint64, uint8> ProposalAnswersMap; // Guid and accept

// Custom flags
enum LfgGrpFlags
{
    LFG_GRP_RANDOM          = 0x01,   // party has randomly chosen dungeon
    LFG_GRP_MIXED           = 0x02,   // party has both alliance and horde players
    LFG_GRP_IN_DUNGEON      = 0x04,   // party is in dungeon
    LFG_GRP_ROLECHECK       = 0x08,   // rolecheck is in progress
    LFG_GRP_BONUS           = 0x10,   // party has +1 player bonus in queue, added when rolecheck fails
    LFG_GRP_PREMADE         = 0x20    // party joined as group
};

struct VoteToKick
{
    VoteToKick() { Reset(); }

    void Reset()
    {
        isInProggres = false;
        votes.clear();
        victim = 0;
        beginTime = 0;
        reason = "";
    }

    bool PlayerVoted(uint64 guid) { return votes.find(guid) != votes.end(); }
    uint8 GetVote(uint64 guid) 
    {
        ProposalAnswersMap::iterator itr = votes.find(guid);
        if (itr != votes.end())
            return itr->second;
        else
            return 0;
    }
    uint32 GetVotesNum(bool agreeOnly)
    {
        if (!agreeOnly)
            return votes.size();
        else
        {
            uint32 votesCount = 0;
            for(ProposalAnswersMap::iterator itr = votes.begin(); itr != votes.end(); ++itr)
                if (itr->second)
                    ++votesCount;
            return votesCount;
        }
    }
    int32 GetTimeLeft() { return 60-(getMSTimeDiff(beginTime, getMSTime())/1000); }

    bool isInProggres;
    ProposalAnswersMap votes;
    uint64 victim;
    uint32 beginTime;
    std::string reason;
};

struct RoleCheck
{
    RoleCheck() { Reset(); }

    void Reset()
    {
        tank = 0;
        heal = 0;
        dps.clear();
        m_rolesProposal.clear();
        m_beforeCheck = 0;
    }

    bool HasFreeRole(uint8 role)
    {
        return ( (role == TANK && !tank) || (role == HEALER && !heal) || (role == DAMAGE && dps.size() < LFG_DPS_COUNT) );
    }

    void SetAsRole(uint8 role, uint64 guid)
    {
        switch(role)
        {
            case TANK:   tank = guid; break;
            case HEALER: heal = guid; break;
            case DAMAGE: dps.insert(guid); break;
        }
    }
    uint8 GetPlayerRole(uint64 guid) const
    {
        if(guid == tank)
            return TANK;
        else if(guid == heal)
            return HEALER;
        else if(dps.find(guid) != dps.end())
            return DAMAGE;
        return 0;
    }

    bool TryRoles(LfgGroup *group);

    uint8 GetSize() const 
    {
        uint8 size = 0;
        if(tank) ++size;
        if(heal) ++size;
        size += dps.size();
        return size;
    }

    uint64 tank;
    uint64 heal;
    PlayerList dps;
    uint8 m_beforeCheck;
    ProposalAnswersMap m_rolesProposal;
};

class MANGOS_DLL_SPEC LfgGroup : public Group
{
    public:
        LfgGroup(bool premade = false, bool mixed = false);
        ~LfgGroup();

        void SetGroupId(uint32 newid) { m_Id = newid; }
        uint32 GetKilledBosses();
        bool LoadGroupFromDB(Field *fields);
        bool IsInDungeon() const { return (m_lfgFlags & LFG_GRP_IN_DUNGEON); }
        bool IsRandom() const { return (m_lfgFlags & LFG_GRP_RANDOM); }
        bool IsActiveRoleCheck() const { return (m_lfgFlags & LFG_GRP_ROLECHECK); }
        bool IsMixed() const { return (m_lfgFlags & LFG_GRP_MIXED); }
        bool IsPremade() const { return (m_lfgFlags & LFG_GRP_PREMADE); }
        uint8 HasBonus() const { return ((m_lfgFlags & LFG_GRP_BONUS) ? 1 : 0); }
        uint8 GetLfgFlags() const { return m_lfgFlags; }
        void AddLfgFlag(uint8 flag) { m_lfgFlags |= flag; }

        void SendLfgPartyInfo(Player *plr);
        void SendLfgQueueStatus();
        void SendGroupFormed();
        void SendProposalUpdate(uint8 state);
        void SendRoleCheckUpdate(uint8 state);
        LfgLocksMap *GetLocksList() const;
        
        //Override these methods
        bool AddMember(const uint64 &guid, const char* name);
        uint32 RemoveMember(const uint64 &guid, const uint8 &method);
        void SendUpdate();

        uint64 GetTank() const { return m_tank; };
        uint64 GetHeal() const { return m_heal; };
        PlayerList *GetDps() { return &dps; };
        bool HasFreeRole(uint8 role)
        {
            return ( (role == TANK && !m_tank) || (role == HEALER && !m_heal) || (role == DAMAGE && dps.size() < LFG_DPS_COUNT) );
        }
        ProposalAnswersMap *GetProposalAnswers() { return &m_answers; }
        ProposalAnswersMap *GetRoleAnswers() { return &m_roleCheck.m_rolesProposal; }
        void UpdateRoleCheck(uint32 diff = 0);
        PlayerList *GetPremadePlayers() { return &premadePlayers; }
        PlayerList *GetRandomPlayers() { return &randomPlayers; }

        void SetTank(uint64 tank) { m_tank = tank; }
        void SetHeal(uint64 heal) { m_heal = heal; }
        void SetLeader(uint64 guid) { _setLeader(guid); }
        void SetAsRole(uint8 role, uint64 guid)
        {
            switch(role)
            {
                case TANK:   m_tank = guid; break;
                case HEALER: m_heal = guid; break;
                case DAMAGE: dps.insert(guid); break;
            }
        }

        void SetDungeonInfo(LFGDungeonEntry const *dungeonInfo) { m_dungeonInfo = dungeonInfo; }
        void SetOriginalDungeonInfo(LFGDungeonEntry const *dungeonInfo) { m_originalInfo = dungeonInfo; }
        LFGDungeonEntry const *GetDungeonInfo(bool original = false) { return (original && m_originalInfo) ? m_originalInfo : m_dungeonInfo; }

        bool RemoveOfflinePlayers();
        bool UpdateCheckTimer(uint32 time);
        void TeleportToDungeon();
        void TeleportPlayer(Player *plr, DungeonInfo *dungeonInfo, uint32 originalDungeonId = 0, bool newPlr = true);
        bool SelectRandomDungeon();
        bool HasCorrectLevel(uint8 level);
        
        void SetInstanceStatus(uint8 status) { m_instanceStatus = status; }
        uint8 GetInstanceStatus() const { return m_instanceStatus; }
        
        uint8 GetPlayerRole(uint64 guid, bool withLeader = true, bool joinedAs = false) const;
        void KilledCreature(Creature *creature);
        void ResetGroup();
        void InitVoteKick(uint64 who, Player *initiator, std::string reason);
        void SendBootPlayer(Player *plr);
        VoteToKick *GetVoteToKick() { return &m_voteToKick; }
        bool UpdateVoteToKick(uint32 diff = 0);
        bool IsFromRnd(uint64 guid) { return (randomPlayers.find(guid) != randomPlayers.end()); }
        void SendRoleCheckFail(uint8 error);
        bool canMove(uint64 guid, uint8 level, LfgGroup *target) const
        {
            if(premadePlayers.find(guid) != premadePlayers.end() ||
               !target->HasCorrectLevel(level))
               return false;
            return true;
        };

    private:
        uint64 m_tank;
        uint64 m_heal;
        PlayerList dps;
        LFGDungeonEntry const *m_dungeonInfo;
        LFGDungeonEntry const *m_originalInfo;
        PlayerList premadePlayers;
        PlayerList randomPlayers;
        ProposalAnswersMap m_answers;

        int32 m_readycheckTimer;
        int32 m_voteKickTimer;
        uint8 m_baseLevel;
        uint8 m_instanceStatus;
        uint8 m_lfgFlags;
        VoteToKick m_voteToKick;
        RoleCheck m_roleCheck;
        bool m_awarded;
};

#endif
