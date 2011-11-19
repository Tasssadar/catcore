#ifndef BATTLEGROUNDQUEUE_H
#define BATTLEGROUNDQUEUE_H

#include "Common.h"
#include "DBCEnums.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"

struct GroupQueueInfo;                                      // type predefinition
struct PlayerQueueInfo                                      // stores information for players in queue
{
    uint32  LastOnlineTime;                                 // for tracking and removing offline players from queue after 5 minutes
    GroupQueueInfo * GroupInfo;                             // pointer to the associated groupqueueinfo
};

#define EQUAL_CHANCE 10
struct GroupQueueInfo                                       // stores information about the group in queue (also used when joined as solo!)
{
    std::map<uint64, PlayerQueueInfo*> Players;             // player queue info map
    uint32  Team;                                           // Player team (ALLIANCE/HORDE)
    BattleGroundTypeId BgTypeId;                            // battleground type id
    bool    IsRated;                                        // rated
    uint8   ArenaType;                                      // 2v2, 3v3, 5v5 or 0 when BG
    uint32  ArenaTeamId;                                    // team id if rated match
    uint32  OpponentTeamId;                                 // opponent team id if rated match
    uint32  JoinTime;                                       // time when group was added
    uint32  RemoveInviteTime;                               // time when we will remove invite for players in group
    uint32  IsInvitedToBGInstanceGUID;                      // was invited to certain BG
    uint32  ArenaTeamRating;                                // if rated match, inited to the rating of the team
    uint32  ArenaTeamMMR;                                   // if rated match, holds arena team mmr
    uint32  OpponentsMMR;                                   // for rated arena matches

    float   CurrentMaxChanceDiff;                           // current difference to max rating
    uint32  LastUpdatedTime;                                // time of last max rating update
    uint32  DiscartedTime;                                  // time of discart

    float   GetMinChance();
    float   GetMaxChance();
    bool    IsAlreadySet() const { return OpponentTeamId && IsInvitedToBGInstanceGUID; }
    bool    ChanceOK(uint32 mmr);       // compares rating in parameter with min and max allowed rating
    float   GetWinChanceValue(uint16 ratA, uint16 ratB);
    uint16  limRat(uint16 rat) const { return rat > 1500 ? 1500 : rat; }
};

enum BattleGroundQueueGroupTypes
{
    BG_QUEUE_ALLIANCE    = 0,
    BG_QUEUE_HORDE       = 1
};
#define BG_QUEUE_GROUP_TYPES_COUNT 2

class BattleGroundQueue
{
    public:
        BattleGroundQueue(uint32 queueTypeId);
        ~BattleGroundQueue();

        virtual void Update(BattleGroundBracketId bracket_id);
        //void UpdateNonrated(BattleGroundTypeId bgTypeId, BattleGroundBracketId bracket_id, uint8 arenaType = 0);
        //void UpdateRated(BattleGroundBracketId bracket_id, uint8 arenaType = 0);

        void FillPlayersToBG(BattleGround* bg, BattleGroundBracketId bracket_id);
        bool CheckPremadeMatch(BattleGroundBracketId bracket_id, uint32 MinPlayersPerTeam, uint32 MaxPlayersPerTeam);
        bool CheckNormalMatch(BattleGround* bg_template, BattleGroundBracketId bracket_id, uint32 minPlayers, uint32 maxPlayers);
        bool CheckSkirmishForSameFaction(BattleGroundBracketId bracket_id, uint32 minPlayersPerTeam);
        GroupQueueInfo * AddGroup(Player* leader, Group* group, BattleGroundTypeId bgTypeId, PvPDifficultyEntry const*  bracketEntry, uint8 ArenaType, bool isRated, bool isPremade, uint32 ArenaRating, uint32 ArenaTeamId = 0, uint32 ArenaMMR = 0);
        void RemovePlayer(const uint64& guid, bool decreaseInvitedCount);
        bool IsPlayerInvited(const uint64& pl_guid, const uint32 bgInstanceGuid, const uint32 removeTime);
        bool GetPlayerGroupInfoData(const uint64& guid, GroupQueueInfo* ginfo);
        void PlayerInvitedToBGUpdateAverageWaitTime(GroupQueueInfo* ginfo, BattleGroundBracketId bracket_id);
        uint32 GetAverageQueueWaitTime(GroupQueueInfo* ginfo, BattleGroundBracketId bracket_id);

    protected:
        BattleGroundQueueTypeId m_QueueTypeId;

        typedef std::map<uint64, PlayerQueueInfo> QueuedPlayersMap;
        QueuedPlayersMap m_QueuedPlayers;

        typedef std::list<GroupQueueInfo*> GroupsQueueType;

        GroupsQueueType m_QueuedGroups[MAX_BATTLEGROUND_BRACKETS][BG_QUEUE_GROUP_TYPES_COUNT];
        GroupsQueueType m_QueuedRatedArenas[MAX_BATTLEGROUND_BRACKETS];

        // class to select and invite groups to bg
        struct SelectionPool
        {
            void Init();
            bool AddGroup(GroupQueueInfo *ginfo, uint32 desiredCount);
            bool KickGroup(uint32 size);
            GroupsQueueType SelectedGroups;
            uint32 PlayerCount;
        };

        //one selection pool for horde, other one for alliance
        SelectionPool m_SelectionPools[BG_TEAMS_COUNT];

        bool InviteGroupToBG(GroupQueueInfo * ginfo, BattleGround * bg, uint32 side);
        uint32 m_WaitTimes[BG_TEAMS_COUNT][MAX_BATTLEGROUND_BRACKETS][COUNT_OF_PLAYERS_TO_AVERAGE_WAIT_TIME];
        uint32 m_WaitTimeLastPlayer[BG_TEAMS_COUNT][MAX_BATTLEGROUND_BRACKETS];
        uint32 m_SumOfWaitTimes[BG_TEAMS_COUNT][MAX_BATTLEGROUND_BRACKETS];
};

class BGQueueNonRated : public BattleGroundQueue
{
    public:
        BGQueueNonRated(uint32 queueTypeId) : BattleGroundQueue(queueTypeId) {}
        ~BGQueueNonRated();

        void Update(BattleGroundBracketId bracket_id);

};

class BGQueueRated : public BattleGroundQueue
{
    public:
        BGQueueRated(uint32 queueTypeId) : BattleGroundQueue(queueTypeId) {}
        ~BGQueueRated();

        void Update(BattleGroundBracketId bracket_id);
        void StartRatedArena(GroupQueueInfo* ginfo1, GroupQueueInfo* ginfo2, PvPDifficultyEntry const* bracketEntry, uint8 arenaType);

};

class BGQueueInviteEvent : public BasicEvent
{
    public:
        BGQueueInviteEvent(const uint64& pl_guid, uint32 BgInstanceGUID, BattleGroundTypeId BgTypeId, uint8 arenaType, uint32 removeTime) :
          m_PlayerGuid(pl_guid), m_BgInstanceGUID(BgInstanceGUID), m_BgTypeId(BgTypeId), m_ArenaType(arenaType), m_RemoveTime(removeTime) { }
        virtual ~BGQueueInviteEvent() {}

        virtual bool Execute(uint64 e_time, uint32 p_time);
        virtual void Abort(uint64 e_time);
    private:
        uint64 m_PlayerGuid;
        uint32 m_BgInstanceGUID;
        BattleGroundTypeId m_BgTypeId;
        uint8  m_ArenaType;
        uint32 m_RemoveTime;
};

class BGQueueRemoveEvent : public BasicEvent
{
    public:
        BGQueueRemoveEvent(const uint64& pl_guid, uint32 bgInstanceGUID, BattleGroundTypeId BgTypeId, BattleGroundQueueTypeId bgQueueTypeId, uint32 removeTime)
            : m_PlayerGuid(pl_guid), m_BgInstanceGUID(bgInstanceGUID), m_RemoveTime(removeTime), m_BgTypeId(BgTypeId), m_BgQueueTypeId(bgQueueTypeId) { }

        virtual ~BGQueueRemoveEvent() {}

        virtual bool Execute(uint64 e_time, uint32 p_time);
        virtual void Abort(uint64 e_time);
    private:
        uint64 m_PlayerGuid;
        uint32 m_BgInstanceGUID;
        uint32 m_RemoveTime;
        BattleGroundTypeId m_BgTypeId;
        BattleGroundQueueTypeId m_BgQueueTypeId;
};
#endif // BATTLEGROUNDQUEUE_H
