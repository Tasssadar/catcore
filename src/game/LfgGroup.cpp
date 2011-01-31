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
#include "InstanceSaveMgr.h"

bool RoleCheck::TryRoles(LfgGroup *group)
{
    ProposalAnswersMap more_roles; // two or more roles
    uint8 error = 0;
    bool isSingleRole = false;

    for(ProposalAnswersMap::iterator itr = m_rolesProposal.begin(); itr != m_rolesProposal.end() && !error; ++itr)
    {
        isSingleRole = false;
        uint8 role = (itr->second & LEADER) ? itr->second-1 : itr->second;
        if(role == 0)
        {
            error = LFG_ROLECHECK_WRONG_ROLES;
            break;
        }
        for(uint8 y = TANK; y <= DAMAGE && !isSingleRole; y *= 2)
        {
            if(role == y)
            {
                isSingleRole = true;
                if(HasFreeRole(y))
                    SetAsRole(y, itr->first);
                else
                    error = LFG_ROLECHECK_WRONG_ROLES;
            }   
        }
        if(!isSingleRole)
            more_roles.insert(std::make_pair<uint64, uint8>(itr->first, itr->second));
    }

    if(error)
    {
        group->SendRoleCheckFail(error);
        return false;
    }

    //Two or more roles
    ProposalAnswersMap::iterator itr, itr_next;
    for(ProposalAnswersMap::iterator itr = more_roles.begin(); itr != more_roles.end(); itr = itr_next)
    {
        itr_next = itr;
        ++itr_next;
        for(uint8 y = TANK; y <= DAMAGE; y *= 2)
        {
            if((itr->second & y) && HasFreeRole(y))
            {
                SetAsRole(y, itr->first);   
                more_roles.erase(itr);
                break;
            }                
        }
    }
    if(!more_roles.empty())
        return false;
    return true;
}

LfgGroup::LfgGroup(bool premade, bool mixed) : Group()
{
    m_lfgFlags = 0;
    dps.clear();
    premadePlayers.clear();
    m_answers.clear();
    m_tank = 0;
    m_heal = 0;
    m_readycheckTimer = 0;
    m_baseLevel = 0;
    //m_groupType = premade ? GROUPTYPE_LFD_2 : GROUPTYPE_LFD; <-- not sure...
    m_groupType = GROUPTYPE_LFD;
    m_instanceStatus = INSTANCE_NOT_SAVED;
    m_dungeonInfo = NULL;
    m_originalInfo = NULL;
    m_voteKickTimer = 0;
    if(premade)
        m_lfgFlags |= LFG_GRP_PREMADE;
    if(mixed)
        m_lfgFlags |= LFG_GRP_MIXED;
}

LfgGroup::~LfgGroup()
{
    sLfgMgr.LfgLog("group %u delete", GetId());

    Rolls::iterator itr;
    while(!RollId.empty())
    {
        itr = RollId.begin();
        Roll *r = *itr;
        RollId.erase(itr);
        delete(r);
    }

    // it is undefined whether objectmgr (which stores the groups) or instancesavemgr
    // will be unloaded first so we must be prepared for both cases
    // this may unload some instance saves
   // for(uint8 i = 0; i < MAX_DIFFICULTY; ++i)
     //   for(BoundInstancesMap::iterator itr2 = m_boundInstances[i].begin(); itr2 != m_boundInstances[i].end(); ++itr2)
       //     itr2->second.save->RemoveGroup(this);

    // Mangos structure expect that this is called separately
    //sObjectMgr.RemoveGroup(this);
}

void LfgGroup::ResetGroup()
{
    m_answers.clear();
    m_readycheckTimer = 0;
    m_voteKickTimer = 0;
    m_roleCheck.Reset();
}

bool LfgGroup::LoadGroupFromDB(Field *fields)
{
    if (!Group::LoadGroupFromDB(fields))
        return false;
    
    m_tank = MAKE_NEW_GUID(m_mainTank,0,HIGHGUID_PLAYER);
    m_heal = MAKE_NEW_GUID(fields[1].GetUInt64(),0,HIGHGUID_PLAYER);
    m_dungeonInfo = sLFGDungeonStore.LookupEntry(fields[19].GetUInt32());
    m_originalInfo = sLFGDungeonStore.LookupEntry(fields[20].GetUInt32());
    m_instanceStatus = fields[21].GetUInt8();
    m_lfgFlags = fields[22].GetUInt8();
    return true;
}
bool LfgGroup::AddMember(const uint64 &guid, const char* name)
{
    Player *player = sObjectMgr.GetPlayer(guid);
    if (!player)
        return false;
    
    if (GetMembersCount() == 0)
        m_baseLevel = player->getLevel();
    sLfgMgr.LfgLog("Add member %u , guid %u", GetId(), guid);
    member_witerator slot = _getMemberWSlot(guid);
    if (slot != m_memberSlots.end())
    {
        sLfgMgr.LfgLog("Player already in group, aborting!");
        return true;
    }
    MemberSlot member;
    member.guid      = guid;
    member.name      = name;
    member.group     = 0;
    member.assistant = false;
    m_memberSlots.push_back(member);
    player->m_lookingForGroup.groups.insert(std::pair<uint32, uint32>(GetDungeonInfo(true)->ID,GetId()));
    return true;
}

uint32 LfgGroup::RemoveMember(const uint64 &guid, const uint8 &method)
{
    member_witerator slot = _getMemberWSlot(guid);
    if (slot != m_memberSlots.end())
        m_memberSlots.erase(slot);

    sLfgMgr.LfgLog("Remove member %u , guid %u", GetId(), guid);
    if (Player *player = sObjectMgr.GetPlayer(guid))
    {
        player->m_lookingForGroup.groups.erase(GetDungeonInfo(true)->ID);
        if (method == 1)
        {
            WorldPacket data(SMSG_GROUP_UNINVITE, 0);
            player->GetSession()->SendPacket( &data );
            player->SetGroup(NULL);
            _removeRolls(guid);
            _homebindIfInstance(player);
        }
        if(IsInDungeon() && IsMixed())
            player->m_lookingForGroup.SetMixedDungeon(0, false);
    }
    //Remove from any role
    if (m_tank == guid)
        m_tank = 0;
    else if (m_heal == guid)
        m_heal = 0;
    else if (dps.find(guid) != dps.end())
        dps.erase(guid);
    CharacterDatabase.PExecute("DELETE FROM group_member WHERE memberGuid='%u'", GUID_LOPART(guid));
    return 0;
}

uint8 LfgGroup::GetPlayerRole(uint64 guid, bool withLeader, bool joinedAs) const
{
    if (joinedAs)
    {
        if (Player *player = sObjectMgr.GetPlayer(guid))
        {
            if (withLeader)
                return player->m_lookingForGroup.roles;
            else
                return (player->m_lookingForGroup.roles & LEADER) ? player->m_lookingForGroup.roles-LEADER : player->m_lookingForGroup.roles;
        }
        return 0;
    }
    uint8 roles = (m_leaderGuid == guid && withLeader) ? LEADER : 0;
    if (m_tank == guid)
        roles |= TANK;
    else if (m_heal == guid)
        roles |= HEALER;
    else if (dps.find(guid) != dps.end())
        roles |= DAMAGE;
    return roles;        
}

bool LfgGroup::RemoveOfflinePlayers()  // Return true if group is empty after check
{
    sLfgMgr.LfgLog("Remove Offline %u, premade %u", GetId(), premadePlayers.empty() ? 0 : 1);
    if (m_memberSlots.empty())
    {
        sLfgMgr.LfgLog("Group %u add to delete", GetId());
        sLfgMgr.AddGroupToDelete(this);
        return true;
    }
    PlayerList toRemove;
    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        sLfgMgr.LfgLog("guid %u", citr->guid);
        Player *plr = sObjectMgr.GetPlayer(citr->guid);
        if (!plr || (!plr->GetSession() && !plr->IsBeingTeleported()))
        {
            sLfgMgr.LfgLog("Add to remove");
            toRemove.insert(citr->guid);
        }
    }
    for(PlayerList::iterator itr = toRemove.begin(); itr != toRemove.end(); ++itr)
    {
        sLfgMgr.LfgLog("Check for premade %u", *itr);
        PlayerList::iterator premade = premadePlayers.find(*itr);
        if(premade != premadePlayers.end())
        {
            sLfgMgr.LfgLog("premade yes");
            for(PlayerList::iterator prm = premadePlayers.begin(); prm != premadePlayers.end(); ++prm)
            {
                Player *plr = sObjectMgr.GetPlayer(*prm);
                if(!plr || !plr->GetSession())
                    continue;
                Group* group = plr->GetGroup();
                if(group)
                {
                    sLfgMgr.RemoveFromQueue(plr, false);
                    return true;
                }
            }
            for(PlayerList::iterator prm = premadePlayers.begin(); prm != premadePlayers.end(); ++prm)
                RemoveMember(*prm, 0);
        }
    }
    for(PlayerList::iterator itr = toRemove.begin(); itr != toRemove.end(); ++itr)
    {
        sLfgMgr.LfgLog("Remove %u", *itr);
        RemoveMember(*itr, 0);
    }
    toRemove.clear();
    //flush empty group
    if (GetMembersCount() == 0)
    {
        sLfgMgr.LfgLog("Group %u add to delete 2", GetId());
        sLfgMgr.AddGroupToDelete(this);
        return true;
    }
    return false;
}

void LfgGroup::KilledCreature(Creature *creature)
{
    if ((creature->GetCreatureInfo()->flags_extra & CREATURE_FLAG_EXTRA_INSTANCE_BIND) &&
        m_instanceStatus == INSTANCE_NOT_SAVED)
    {
            m_instanceStatus = INSTANCE_SAVED;
    }
    if (creature->GetEntry() == sLfgMgr.GetDungeonInfo(m_dungeonInfo->ID)->lastBossId)
    {
        //Last boss
        m_instanceStatus = INSTANCE_COMPLETED;
        //Reward here
        for (GroupReference *itr = GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player *plr = itr->getSource();
            if (!plr || !plr->IsInWorld())
                continue;
            WorldPacket data(SMSG_LFG_PLAYER_REWARD);
            data << uint32(GetDungeonInfo((IsRandom() || IsFromRnd(plr->GetGUID())))->Entry());
            data << uint32(m_dungeonInfo->Entry());

            uint32 ID = GetDungeonInfo((IsRandom() || IsFromRnd(plr->GetGUID())))->ID;
            sLfgMgr.BuildRewardBlock(data, ID, plr);
            plr->GetSession()->SendPacket(&data);
            LfgReward *reward = sLfgMgr.GetDungeonReward(ID, plr->m_lookingForGroup.DoneDungeon(ID, plr), plr->getLevel());
            if (!reward)
                continue;
            plr->CompleteQuest(reward->questInfo->GetQuestId());
            if (IsRandom())
                plr->RemoveAurasDueToSpell(LFG_RANDOM_COOLDOWN);
        }
    }  
    SendUpdate();
}

bool LfgGroup::UpdateCheckTimer(uint32 time)
{
    m_readycheckTimer += time;
    if (m_readycheckTimer >= LFG_TIMER_READY_CHECK || GetMembersCount() != LFG_GROUP)
        return false;
    return true;
}
void LfgGroup::TeleportToDungeon()
{
    if (IsInDungeon())
    {
        for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
        {
            Player *plr = sObjectMgr.GetPlayer(citr->guid);
            if (!plr || !plr->GetSession())
                continue;

            plr->m_lookingForGroup.queuedDungeons.clear();
            if (plr->GetMapId() == m_dungeonInfo->map)
            {
                sLfgMgr.SendLfgUpdatePlayer(plr, LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
                sLfgMgr.SendLfgUpdateParty(plr, LFG_UPDATETYPE_REMOVED_FROM_QUEUE);
                continue;
            }
            DungeonInfo* dungeonInfo = sLfgMgr.GetDungeonInfo(m_dungeonInfo->ID);
            plr->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS, GetMembersCount()-1);
            TeleportPlayer(plr, dungeonInfo, m_dungeonInfo->ID);
            /*SendUpdate();
            plr->SetGroupUpdateFlag(GROUP_UPDATE_FULL);
            UpdatePlayerOutOfRange(plr);*/
        }
        return;
    }
    //If random, then select here
    if (m_dungeonInfo->type == LFG_TYPE_RANDOM && !SelectRandomDungeon())
        return;

    DungeonInfo* dungeonInfo = sLfgMgr.GetDungeonInfo(m_dungeonInfo->ID);
    //Set Leader
    m_leaderGuid = 0;   
    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player *plr = sObjectMgr.GetPlayer(citr->guid);
        if (!plr || !plr->GetSession())
            continue;
        if (IsPremade() && plr->GetGroup())
        {
            plr->GetGroup()->UnbindInstance(dungeonInfo->start_map, m_dungeonInfo->isHeroic() ? DUNGEON_DIFFICULTY_HEROIC : DUNGEON_DIFFICULTY_NORMAL);
            m_leaderGuid = plr->GetGroup()->GetLeaderGUID();
            m_leaderName = plr->GetGroup()->GetLeaderName();
            break;
        }
        else if (plr->m_lookingForGroup.roles & LEADER)
        {
            m_leaderGuid = plr->GetGUID();
            m_leaderName = plr->GetName();
            break;
        }
    }
    if (m_leaderGuid == 0)
    {
        m_leaderGuid = m_memberSlots.begin()->guid;
        m_leaderName = m_memberSlots.begin()->name;
    }
    m_lootMethod = GROUP_LOOT;
    m_lootThreshold = ITEM_QUALITY_UNCOMMON;
    m_looterGuid = m_leaderGuid;
    m_dungeonDifficulty = m_dungeonInfo->isHeroic() ? DUNGEON_DIFFICULTY_HEROIC : DUNGEON_DIFFICULTY_NORMAL;
    m_raidDifficulty = RAID_DIFFICULTY_10MAN_NORMAL;

    //sort group members...
    UnbindInstance(dungeonInfo->start_map, m_dungeonInfo->isHeroic() ? DUNGEON_DIFFICULTY_HEROIC : DUNGEON_DIFFICULTY_NORMAL);
    CharacterDatabase.PExecute("DELETE FROM group_member WHERE groupId ='%u'", m_Id);
    ResetInstances(INSTANCE_RESET_ALL, true, NULL);
    ResetInstances(INSTANCE_RESET_GROUP_DISBAND, true, NULL);

    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player *plr = sObjectMgr.GetPlayer(citr->guid);
        if (!plr || !plr->GetSession())
            continue;

        plr->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_USE_LFD_TO_GROUP_WITH_PLAYERS, GetMembersCount()-1);   
        TeleportPlayer(plr, dungeonInfo, GetDungeonInfo(true)->ID);
    }
    m_lfgFlags |= LFG_GRP_IN_DUNGEON;
    
    //Save to DB
    CharacterDatabase.PExecute("DELETE FROM groups WHERE groupId ='%u' OR leaderGuid='%u'", m_Id, GUID_LOPART(m_leaderGuid));    
    CharacterDatabase.PExecute("INSERT INTO groups (groupId,leaderGuid,mainTank,mainAssistant,lootMethod,looterGuid,lootThreshold,icon1,icon2,icon3,icon4,icon5,icon6,icon7,icon8,groupType,difficulty,raiddifficulty,healGuid,LfgId,LfgRandomEntry,LfgInstanceStatus,LfgFlags) "
        "VALUES ('%u','%u','%u','%u','%u','%u','%u','" UI64FMTD "','" UI64FMTD "','" UI64FMTD "','" UI64FMTD "','" UI64FMTD "','" UI64FMTD "','" UI64FMTD "','" UI64FMTD "','%u','%u','%u','%u','%u','%u','%u','%u')",
        m_Id, GUID_LOPART(m_leaderGuid), GUID_LOPART(m_tank), GUID_LOPART(m_mainAssistant), uint32(m_lootMethod),
        GUID_LOPART(m_looterGuid), uint32(m_lootThreshold), m_targetIcons[0], m_targetIcons[1], m_targetIcons[2], m_targetIcons[3], m_targetIcons[4], m_targetIcons[5], m_targetIcons[6], m_targetIcons[7], uint8(m_groupType), uint32(m_dungeonDifficulty), uint32(m_raidDifficulty), GUID_LOPART(m_heal), m_dungeonInfo->ID, GetDungeonInfo(true)->ID, m_instanceStatus, uint8(m_lfgFlags));  
}

void LfgGroup::TeleportPlayer(Player *plr, DungeonInfo *dungeonInfo, uint32 originalDungeonId, bool newPlr)
{
    if(newPlr)
    {
        plr->m_lookingForGroup.queuedDungeons.clear(); // must be before remove grom group
        if (Group *group = plr->GetGroup())
        {
            if (!group->isLfgGroup())
            {
                group->RemoveMember(plr->GetGUID(), 0);
                if (group->GetMembersCount() == 0)
                    group->Disband(true);
            }
            plr->SetGroup(NULL);
        }

        plr->UnbindInstance(dungeonInfo->start_map, m_dungeonInfo->isHeroic() ? DUNGEON_DIFFICULTY_HEROIC : DUNGEON_DIFFICULTY_NORMAL);
        plr->ResetInstances(INSTANCE_RESET_GROUP_JOIN,false);
        plr->ResetInstances(INSTANCE_RESET_GROUP_JOIN,true);
        plr->ResetInstances(INSTANCE_RESET_ALL, true);
        plr->ResetInstances(INSTANCE_RESET_ALL, false);

        if (plr->getLevel() >= LEVELREQUIREMENT_HEROIC)
        {
            if (plr->GetDungeonDifficulty() != GetDungeonDifficulty())
                plr->SetDungeonDifficulty(GetDungeonDifficulty());
            if (plr->GetRaidDifficulty() != GetRaidDifficulty())
                plr->SetRaidDifficulty(GetRaidDifficulty());
        }
        plr->SetGroup(this, 0);
        plr->SetGroupInvite(NULL);

        uint32 taxi_start = 0;
        uint32 taxi_end = 0;
        uint32 mount_spell = 0;
        WorldLocation joinLoc;
        if (!plr->m_taxi.empty())
        {
           taxi_start = plr->m_taxi.GetTaxiSource();
           taxi_end = plr->m_taxi.GetTaxiDestination();
           joinLoc = WorldLocation(plr->GetMapId(), plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), plr->GetOrientation());
        }
        else
        {
            // Mount spell id storing
            if (plr->IsMounted())
            {
                Unit::AuraList const& auras = plr->GetAurasByType(SPELL_AURA_MOUNTED);
                if (!auras.empty())
                    mount_spell = (*auras.begin())->GetId();
            }
            if(!mount_spell && plr->getClass() == CLASS_DRUID)
            {
                Unit::AuraList const& auras = plr->GetAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
                if (!auras.empty())
                    mount_spell = (*auras.begin())->GetId();
            }
            //Nearest graveyard if in dungeon
            if (plr->GetMap()->IsDungeon())
            {
                if (const WorldSafeLocsEntry* entry = sObjectMgr.GetClosestGraveYard(plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), plr->GetMapId(), plr->GetTeam()))
                    joinLoc = WorldLocation(entry->map_id, entry->x, entry->y, entry->z, 0.0f);
                else
                    joinLoc = WorldLocation(plr->GetMapId(), plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), plr->GetOrientation());
            }
            else
                joinLoc = WorldLocation(plr->GetMapId(), plr->GetPositionX(), plr->GetPositionY(), plr->GetPositionZ(), plr->GetOrientation());
        }
        CharacterDatabase.PExecute("DELETE FROM group_member WHERE memberGuid='%u'", GUID_LOPART(plr->GetGUID()));
        CharacterDatabase.PExecute("INSERT INTO group_member(groupId,memberGuid,assistant,subgroup,lfg_join_x,lfg_join_y,lfg_join_z,lfg_join_o,lfg_join_map,taxi_start,taxi_end,mount_spell) "
            "VALUES('%u','%u','%u','%u','%f','%f','%f','%f','%u','%u','%u','%u')",
            m_Id, GUID_LOPART(plr->GetGUID()), 0, 1, joinLoc.coord_x, joinLoc.coord_y, joinLoc.coord_z, joinLoc.orientation, joinLoc.mapid, taxi_start, taxi_end, mount_spell);
        
        //Set info to player
        plr->m_lookingForGroup.joinLoc = joinLoc;
        plr->m_lookingForGroup.taxi_start = taxi_start;
        plr->m_lookingForGroup.taxi_end = taxi_end;
        plr->m_lookingForGroup.mount_spell = mount_spell;
        plr->m_lookingForGroup.roles = GetPlayerRole(plr->GetGUID());

        if (IsRandom())
            plr->CastSpell(plr, LFG_RANDOM_COOLDOWN, true);
    }

    // resurrect the player
    if (!plr->isAlive())
    {
        plr->ResurrectPlayer(1.0f);
        plr->SpawnCorpseBones();
    }
    // stop taxi flight at port
    if (plr->isInFlight())
    {
        plr->GetMotionMaster()->MovementExpired(false);
        plr->GetMotionMaster()->Clear(false, true);
        plr->GetMotionMaster()->MoveIdle();
        plr->m_taxi.ClearTaxiDestinations();
    }
    
    plr->ScheduleDelayedOperation(DELAYED_LFG_ENTER_DUNGEON);
    plr->ScheduleDelayedOperation(DELAYED_SAVE_PLAYER);
    plr->ScheduleDelayedOperation(DELAYED_LFG_CLEAR_LOCKS);

    if(IsMixed())
    {
        plr->m_lookingForGroup.SetMixedDungeon(dungeonInfo->start_map);
        if(newPlr)
            plr->setFaction(sWorld.getConfig(CONFIG_UINT32_TEAM_BG_FACTION_BLUE));
    }

    if (IsInDungeon())
    {
        if(newPlr)
        {
            for (GroupReference *itr = GetFirstMember(); itr != NULL; itr = itr->next())
            {
                Player *player = itr->getSource();
                if (!player  || player->GetMapId() != GetDungeonInfo()->map)
                    continue;

				plr->TeleportTo(player->GetLocation());
				return;
            }
        }
        plr->TeleportTo(dungeonInfo->start_map, dungeonInfo->start_x, dungeonInfo->start_y, dungeonInfo->start_z, dungeonInfo->start_o);
        return;
    }

    //Must re-add player to reset id...
    Map *map = plr->GetMap();
    if(map->GetId() == dungeonInfo->start_map)
    {
        map->Remove(plr, false);
        map->Add(plr);
    }
    
    plr->TeleportTo(dungeonInfo->start_map, dungeonInfo->start_x,
        dungeonInfo->start_y, dungeonInfo->start_z, dungeonInfo->start_o);
}

bool LfgGroup::SelectRandomDungeon()
{
    m_originalInfo = m_dungeonInfo;
    m_lfgFlags |= LFG_GRP_RANDOM;
    LfgLocksMap *groupLocks = GetLocksList();
    std::vector<LFGDungeonEntry const*> options;
    LFGDungeonEntry const *currentRow = NULL;
    //Possible dungeons
    LfgDungeonList* list = sLfgMgr.GetRandomOptions(m_dungeonInfo->ID);
    for(LfgDungeonList::iterator itr = list->begin(); itr != list->end(); ++itr)
        options.push_back(*itr);

    //And now get only without locks
    for(LfgLocksMap::iterator itr = groupLocks->begin(); itr != groupLocks->end(); ++itr)
    {
        for(LfgLocksList::iterator itr2 = itr->second->begin(); itr2 != itr->second->end(); ++itr2)
        {
            for(std::vector<LFGDungeonEntry const*>::iterator itrDung = options.begin(); itrDung != options.end(); ++itrDung)
            {
                if ((*itrDung)->ID != (*itr2)->dungeonInfo->ID)
                    continue;
                DungeonInfo* dungeonInfo = sLfgMgr.GetDungeonInfo((*itr2)->dungeonInfo->ID);
                if (dungeonInfo->locked || (*itr2)->lockType != LFG_LOCKSTATUS_RAID_LOCKED)
                {
                    options.erase(itrDung);
                    break;
                }                   
            }
        }
    }
    //This should not happen
    if (options.empty())
    {
        PlayerList toRemove;
        for(member_witerator itr = m_memberSlots.begin(); itr != m_memberSlots.end(); ++itr)
        {
            Player *plr = sObjectMgr.GetPlayer(itr->guid);
            if (!plr)
                continue;
            sLfgMgr.SendLfgUpdatePlayer(plr, LFG_UPDATETYPE_GROUP_DISBAND);
            sLog.outError("LfgMgr: Cannot find any random dungeons for player %s", plr->GetName());
            plr->GetSession()->SendNotification("Cannot find any random dungeons for this group, you have to find new group. We are sorry");
            toRemove.insert(plr->GetGUID());     
        }
        for(PlayerList::iterator itr = toRemove.begin(); itr != toRemove.end(); ++itr)
            RemoveMember(*itr, 0);
        toRemove.clear();
        sLfgMgr.AddGroupToDelete(this);
        return false;
    }
    //Select dungeon, there should be also bind check
    uint32 tmp = time(NULL)%options.size();
    m_dungeonInfo = options[tmp];
    return true;
}

bool LfgGroup::HasCorrectLevel(uint8 level)
{
    //Non random
    if (!m_dungeonInfo->isRandom())
        return (level >= m_dungeonInfo->minlevel && level <= m_dungeonInfo->maxlevel);

    //And random
    switch(m_dungeonInfo->grouptype)
    {
        case LFG_GROUPTYPE_CLASSIC: 
        case LFG_GROUPTYPE_BC_NORMAL:
            if (m_baseLevel > level)
                return (m_baseLevel - level <= 5);
            else
                return (level - m_baseLevel <= 5);
        case LFG_GROUPTYPE_BC_HEROIC:
            return (level >= 70 && level <= 73);
        case LFG_GROUPTYPE_WTLK_NORMAL:
            return (level > 68);
        case LFG_GROUPTYPE_WTLK_HEROIC:
            return (level == 80);
    }
	return true;
}
void LfgGroup::SendUpdate()
{
    Player *player;

    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        player = sObjectMgr.GetPlayer(citr->guid);
        if (!player || !player->GetSession() || player->GetGroup() != this )
            continue;
                                                            // guess size
        WorldPacket data(SMSG_GROUP_LIST, (1+1+1+1+8+4+GetMembersCount()*20));
        data << uint8(m_groupType);                         // group type (flags in 3.3)
        data << uint8(citr->group);                         // groupid
        data << uint8(GetFlags(*citr));                     // group flags
        data << uint8(GetPlayerRole(citr->guid));           // 2.0.x, isBattleGroundGroup? <--- Its flags or maybe more likely roles....?
        data << uint8(m_instanceStatus);                    // Instance status 0= not saved, 1= saved, 2 = completed
        data << uint32(GetDungeonInfo(IsFromRnd(citr->guid))->Entry());// dungeon entry
        data << uint64(0x1F54000004D3B000);                 // related to voice chat?
        data << uint32(0);                                  // 3.3, this value increments every time SMSG_GROUP_LIST is sent
        data << uint32(GetMembersCount()-1);
        for(member_citerator citr2 = m_memberSlots.begin(); citr2 != m_memberSlots.end(); ++citr2)
        {
            if (citr->guid == citr2->guid)
                continue;
            Player* member = sObjectMgr.GetPlayer(citr2->guid);
            uint8 onlineState = (member) ? MEMBER_STATUS_ONLINE : MEMBER_STATUS_OFFLINE;
            onlineState = onlineState | ((isBGGroup()) ? MEMBER_STATUS_PVP : 0);
            if(member)
            {
                if(member->isAFK())
                    onlineState |= MEMBER_STATUS_AFK;
                if(member->isDND())
                    onlineState |= MEMBER_STATUS_DND;
            }

            data << citr2->name;
            data << uint64(citr2->guid);
            data << uint8(onlineState);                     // online-state
            data << uint8(citr2->group);                    // groupid
            data << uint8(GetFlags(*citr2));                // group flags
            data << uint8(GetPlayerRole(citr2->guid));      // 3.3, role?
        }

        data << uint64(m_leaderGuid);                       // leader guid
        if (GetMembersCount()-1)
        {
            data << uint8(m_lootMethod);                    // loot method
            data << uint64(m_looterGuid);                   // looter guid
            data << uint8(m_lootThreshold);                 // loot threshold
            data << uint8(m_dungeonDifficulty);             // Dungeon Difficulty
            data << uint8(m_raidDifficulty);                // Raid Difficulty
            data << uint8(0);                               // 3.3, dynamic difficulty?
        }
        player->GetSession()->SendPacket( &data );
    }
}

LfgLocksMap* LfgGroup::GetLocksList() const
{
    LfgLocksMap *groupLocks = new LfgLocksMap();
    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player *plr = sObjectMgr.GetPlayer(citr->guid);
        if (!plr || !plr->GetSession())
            continue;

        LfgLocksList *playerLocks = sLfgMgr.GetDungeonsLock(plr);
        if (!playerLocks->empty())
            groupLocks->insert(std::pair<uint64, LfgLocksList*>(plr->GetGUID(),playerLocks));
    }
    return groupLocks;
}

void LfgGroup::SendLfgPartyInfo(Player *plr)
{   
    LfgLocksMap *groupLocks = GetLocksList();

    uint32 size = 0;
    for (LfgLocksMap::const_iterator itr = groupLocks->begin(); itr != groupLocks->end(); ++itr)
        size += 8 + 4 + itr->second->size() * (4 + 4);

    WorldPacket data(SMSG_LFG_PARTY_INFO, 1 + size);
    data << uint8(groupLocks->size());                   // number of locks...
    for (LfgLocksMap::const_iterator itr = groupLocks->begin(); itr != groupLocks->end(); ++itr)
    {
        data << uint64(itr->first);                      // guid of player which has lock
        data << uint32(itr->second->size());             // Size of lock dungeons for that player
        for (LfgLocksList::iterator it = itr->second->begin(); it != itr->second->end(); ++it)
        {
            data << uint32((*it)->dungeonInfo->Entry()); // Dungeon entry + type
            data << uint32((*it)->lockType);             // Lock status
        }
    }
    plr->GetSession()->SendPacket(&data);
    for (LfgLocksMap::const_iterator itr = groupLocks->begin(); itr != groupLocks->end(); ++itr)
    {
        for (LfgLocksList::iterator it = itr->second->begin(); it != itr->second->end(); ++it)
            delete *it;
        delete itr->second;
    }
    delete groupLocks;
}

void LfgGroup::SendLfgQueueStatus()
{
    Player *plr;
    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        plr = sObjectMgr.GetPlayer(citr->guid);
        if (!plr || !plr->GetSession())
            continue;

        uint8 role = plr->m_lookingForGroup.roles;
        WorldPacket data(SMSG_LFG_QUEUE_STATUS, 31);
        data << uint32(GetDungeonInfo(IsFromRnd(citr->guid))->ID);     // Dungeon
        data << uint32(sLfgMgr.GetAvgWaitTime(GetDungeonInfo(true)->ID, LFG_WAIT_TIME_AVG, role)); // Average Wait time
        data << uint32(sLfgMgr.GetAvgWaitTime(GetDungeonInfo(true)->ID, LFG_WAIT_TIME, role));     // Wait Time
        data << uint32(sLfgMgr.GetAvgWaitTime(GetDungeonInfo(true)->ID, LFG_WAIT_TIME_TANK, role));// Wait Tanks
        data << uint32(sLfgMgr.GetAvgWaitTime(GetDungeonInfo(true)->ID, LFG_WAIT_TIME_HEAL, role));// Wait Healers
        data << uint32(sLfgMgr.GetAvgWaitTime(GetDungeonInfo(true)->ID, LFG_WAIT_TIME_DPS, role)); // Wait Dps
        data << uint8(m_tank ? 0 : 1);                                  // Tanks needed
        data << uint8(m_heal ? 0 : 1);                                  // Healers needed
        data << uint8(LFG_DPS_COUNT - dps.size());                      // Dps needed
        data << uint32(getMSTimeDiff(plr->m_lookingForGroup.joinTime, getMSTime())/1000);   // Player wait time in queue
        plr->GetSession()->SendPacket(&data);
    }
}

void LfgGroup::SendGroupFormed()
{
    if (!premadePlayers.empty())
    {
        ResetGroup();
        for(PlayerList::iterator itr = premadePlayers.begin(); itr != premadePlayers.end(); ++itr)
            m_answers.insert(std::pair<uint64, uint8>(*itr, 1));
    }   

    //SMSG_LFG_UPDATE_PLAYER -> SMSG_LFG_PROPOSAL_UPDATE
    WorldPacket data(SMSG_LFG_UPDATE_PLAYER, 10);
    Player *plr = NULL;
    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        plr = sObjectMgr.GetPlayer(citr->guid);
        if (!plr || !plr->GetSession())
            continue;
        data.clear();
        data << uint8(LFG_UPDATETYPE_PROPOSAL_FOUND);
        data << uint8(true); //extra info
        data << uint8(false); //queued
        data << uint8(0); //unk
        data << uint8(0); //unk
        data << uint8(1); //count
        data << uint32(GetDungeonInfo(IsFromRnd(citr->guid))->Entry());
        data << uint8(0);
    
        plr->GetSession()->SendPacket(&data);
    }
    SendProposalUpdate(LFG_PROPOSAL_WAITING);
}

void LfgGroup::SendProposalUpdate(uint8 state)
{
    Player *plr;
    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        plr = sObjectMgr.GetPlayer(citr->guid);
        if (!plr || !plr->GetSession() || (IsInDungeon() && plr->GetMapId() == GetDungeonInfo()->map))
            continue;
        //Correct - 3.3.3a
        WorldPacket data(SMSG_LFG_PROPOSAL_UPDATE);
        data << uint32(GetDungeonInfo(IsFromRnd(citr->guid))->Entry());
        data << uint8(state); 
        data << uint32(GetId()); 
        data << uint32(GetKilledBosses());
        data << uint8(0); //silent
        uint8 membersCount = 0;
        for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
        {
            Player *plr2 = sObjectMgr.GetPlayer(citr->guid);
            if (plr2 && plr2->GetSession())
                ++membersCount;
        }
        data << uint8(membersCount);
        for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
        {
            if (Player *plr2 = sObjectMgr.GetPlayer(citr->guid))
            {
                uint8 roles = GetPlayerRole(plr2->GetGUID());
                //Something got wrong
                if (roles < 2)
                {
                    sLog.outError("LfgMgr: Wrong role for player %s in SMSG_LFG_PROPOSAL_UPDATE", plr2->GetName());
                    m_answers.insert(std::pair<uint64, uint8>(plr2->GetGUID(), 0));
                }

                data << uint32(roles);
                data << uint8(plr == plr2);  // if its you, this is true
                data << uint8(IsInDungeon()); // InDungeon
                data << uint8(premadePlayers.find(plr2->GetGUID()) != premadePlayers.end()); // Same group
                //If player agrees with dungeon, these two are 1
                if (m_answers.find(plr2->GetGUID()) != m_answers.end())
                {
                    data << uint8(1);
                    data << uint8(m_answers.find(plr2->GetGUID())->second);
                }
                else
                {
                    data << uint8(0);  //Answer
                    data << uint8(0);  //Accept
                }
            }
        }
        plr->GetSession()->SendPacket(&data);
    }
}

void LfgGroup::UpdateRoleCheck(uint32 diff)
{
    sLfgMgr.LfgLog("Updaterolecheck %u, diff %u", GetId(), diff);
    if (diff != 0)
    {
        m_readycheckTimer += diff;
        if (m_readycheckTimer >= LFG_TIMER_READY_CHECK && m_roleCheck.m_beforeCheck != m_roleCheck.m_rolesProposal.size())
        {
            SendRoleCheckFail(LFG_ROLECHECK_MISSING_ROLE);
            return;
        }
    }

    //offline check
    bool offline = false;
    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player *player = sObjectMgr.GetPlayer(citr->guid);
        if (!player)
        {
            offline = true;
            break;
        }
    }
    if (GetMembersCount() != m_roleCheck.m_beforeCheck || offline)
    {
        SendRoleCheckFail(LFG_ROLECHECK_ABORTED);
        return;
    }

    // add answers
    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player *player = sObjectMgr.GetPlayer(citr->guid);
        if (m_roleCheck.m_rolesProposal.find(citr->guid) != m_roleCheck.m_rolesProposal.end() || !player || !player->GetSession() ||
            player->m_lookingForGroup.roles == 0)
            continue;
        m_roleCheck.m_rolesProposal.insert(std::make_pair<uint64, uint8>(player->GetGUID(), player->m_lookingForGroup.roles));
        WorldPacket data(SMSG_LFG_ROLE_CHOSEN, 13);
        data << uint64(player->GetGUID());
        data << uint8(1);
        data << uint32(player->m_lookingForGroup.roles);
        BroadcastPacket(&data, false);
    }

    //Offline members checked at join
    //Check roles
    if (m_roleCheck.m_beforeCheck > m_roleCheck.m_rolesProposal.size())
        return;
    
    Player *leader = sObjectMgr.GetPlayer(GetLeaderGUID());
    if(!leader || !leader->IsInWorld() || !m_roleCheck.TryRoles(this))
    {
        SendRoleCheckFail(LFG_ROLECHECK_WRONG_ROLES);
        return;
    }

    SetAsRole(TANK, m_roleCheck.tank);
    SetAsRole(HEALER, m_roleCheck.heal);
    dps.clear();
    dps = m_roleCheck.dps;

    //Move group to queue
    SendRoleCheckUpdate(LFG_ROLECHECK_FINISHED);
    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player *player = sObjectMgr.GetPlayer(citr->guid);
        if (!player || !player->GetSession())
            continue;
        
        if (player->GetGUID() == GetLeaderGUID())
            sLfgMgr.SendJoinResult(player, LFG_JOIN_OK);
        else
        {
            //player->m_lookingForGroup.roles = GetPlayerRole(player->GetGUID());
            player->m_lookingForGroup.comment = "";
            player->m_lookingForGroup.joinTime = getMSTime();
            player->m_lookingForGroup.queuedDungeons = leader->m_lookingForGroup.queuedDungeons;
        }
        if (IsInDungeon())
            premadePlayers.insert(player->GetGUID());
    }
    m_lfgFlags &= ~LFG_GRP_ROLECHECK;
    SendUpdate();
    sLfgMgr.AddCheckedGroup(this, true);
}

void LfgGroup::SendRoleCheckFail(uint8 error)
{
    m_lfgFlags &= ~LFG_GRP_ROLECHECK;
    SendRoleCheckUpdate(error);
    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player *player = sObjectMgr.GetPlayer(citr->guid);
        if (!player || !player->GetSession())
            continue;
  
        sLfgMgr.SendLfgUpdateParty(player, LFG_UPDATETYPE_ROLECHECK_FAILED);
        if (player->GetGUID() == GetLeaderGUID())
            sLfgMgr.SendJoinResult(player, LFG_JOIN_FAILED, error);
    }
    sLfgMgr.AddCheckedGroup(this, false);
}

void LfgGroup::SendRoleCheckUpdate(uint8 state)
{
    if (state == LFG_ROLECHECK_INITIALITING)
    {
        m_lfgFlags |= LFG_GRP_ROLECHECK;
        ResetGroup();
        if (IsInDungeon())
            premadePlayers.clear();
        m_roleCheck.Reset();
        m_roleCheck.m_beforeCheck = GetMembersCount();
    }
    
    Player *leader = sObjectMgr.GetPlayer(GetLeaderGUID());
    if (!leader)
        return;

    WorldPacket data(SMSG_LFG_ROLE_CHECK_UPDATE, 6 + leader->m_lookingForGroup.queuedDungeons.size() * 4 + 1 + GetMembersCount() * 14);
    data << uint32(state);
    data << uint8(state == LFG_ROLECHECK_INITIALITING); // begining
    data << uint8(leader->m_lookingForGroup.queuedDungeons.size());
    for (LfgDungeonList::const_iterator it = leader->m_lookingForGroup.queuedDungeons.begin(); it != leader->m_lookingForGroup.queuedDungeons.end(); ++it)
        data << uint32((*it)->Entry());
    data << uint8(GetMembersCount());
    //leader first
    data << uint64(GetLeaderGUID());
    ProposalAnswersMap::iterator itr = m_roleCheck.m_rolesProposal.find(GetLeaderGUID());
    if (itr != m_roleCheck.m_rolesProposal.end())
    {
        data << uint8(1); //ready
        data << uint32(itr->second); //roles 
    }
    else
    {
        data << uint8(0); //ready
        data << uint32(0); //roles
    }
    data << uint8(leader->getLevel());
    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player *member = sObjectMgr.GetPlayer(citr->guid);
        if (!member || !member->GetSession() || member->GetGUID() == GetLeaderGUID())
            continue;
        itr = m_roleCheck.m_rolesProposal.find(citr->guid);

        data << uint64(member->GetGUID());
        if(itr != m_roleCheck.m_rolesProposal.end())
        {
            data << uint8(1);
            data << uint32(itr->second);
        }else
            data << uint8(0) << uint32(0);
        data << uint8(member->getLevel());
    }

    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player *plr = sObjectMgr.GetPlayer(citr->guid);
        if (!plr || !plr->GetSession())
            continue;
        if(state == LFG_ROLECHECK_INITIALITING)
            plr->m_lookingForGroup.roles = 0;
        plr->GetSession()->SendPacket(&data);
    }
}

void LfgGroup::InitVoteKick(uint64 who, Player *initiator, std::string reason)
{
    //Checks first
    PartyResult error = ERR_PARTY_RESULT_OK;
    if (GetMembersCount() <= 3)
        error = ERR_PARTY_LFG_BOOT_TOO_FEW_PLAYERS;
    else if (m_instanceStatus == INSTANCE_COMPLETED)
        error = ERR_PARTY_LFG_BOOT_DUNGEON_COMPLETE;
    else if (m_voteToKick.isInProggres)
        error = ERR_PARTY_LFG_BOOT_IN_PROGRESS;
    
    if (error != ERR_PARTY_RESULT_OK)
    {
        initiator->GetSession()->SendPartyResult(PARTY_OP_LEAVE, "", error);
        return;
    }

    m_voteToKick.Reset();
    m_voteToKick.isInProggres = true;
    m_voteToKick.victim = who;
    m_voteToKick.beginTime = getMSTime();
    m_voteToKick.reason = reason;
    m_voteToKick.votes.insert(std::make_pair<uint64, uint8>(initiator->GetGUID(), 1));  // initiator agrees automatically
    m_voteKickTimer = 0;

    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player *member = sObjectMgr.GetPlayer(citr->guid);
        if (!member || !member->GetSession())
            continue;
        SendBootPlayer(member);
    }

    sLfgMgr.AddVoteKickGroup(this);   
}

// return true = remove from update list, false = continue
bool LfgGroup::UpdateVoteToKick(uint32 diff)
{
    if (!m_voteToKick.isInProggres)
        return true;

    if (diff)
    {
        if (m_voteToKick.GetTimeLeft() <= 0)
        {
            m_voteToKick.isInProggres = false;
            for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
            {
                Player *member = sObjectMgr.GetPlayer(citr->guid);
                if (!member || !member->GetSession())
                    continue;
                SendBootPlayer(member);
            }
            m_voteToKick.Reset();
            return true;
        }
        return false;
    }

    //Send Update
    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player *member = sObjectMgr.GetPlayer(citr->guid);
        if (!member || !member->GetSession())
            continue;
        SendBootPlayer(member);
    }

    if (m_voteToKick.GetVotesNum(false) < 3)
        return false;
    else if (m_voteToKick.GetVotesNum(true) >= 3)
    {
        
        Player *victim = sObjectMgr.GetPlayer(m_voteToKick.victim);
         sLfgMgr.LfgLog("Remove member - afk rolecheck");
        RemoveMember(m_voteToKick.victim, 1);
        if (victim && victim->GetSession())
        {
            victim->ScheduleDelayedOperation(DELAYED_LFG_MOUNT_RESTORE);
            victim->ScheduleDelayedOperation(DELAYED_LFG_TAXI_RESTORE);
            victim->ScheduleDelayedOperation(DELAYED_LFG_CLEAR_LOCKS);
            victim->RemoveAurasDueToSpell(LFG_BOOST);
            WorldLocation teleLoc = victim->m_lookingForGroup.joinLoc;
            if (teleLoc.coord_x != 0 && teleLoc.coord_y != 0 && teleLoc.coord_z != 0)
                victim->TeleportTo(teleLoc);
            else
                victim->TeleportToHomebind();
        }
        else
        {
            AreaTrigger const *trigger = sObjectMgr.GetGoBackTrigger(GetDungeonInfo()->map);
            if(trigger)
                Player::SavePositionInDB(trigger->target_mapId, trigger->target_X, trigger->target_Y, trigger->target_Z, trigger->target_Orientation,
                    sTerrainMgr.GetZoneId(trigger->target_mapId, trigger->target_X, trigger->target_Y, trigger->target_Z), m_voteToKick.victim);
        }
        //Change leader
        if (m_voteToKick.victim == m_leaderGuid)
        {
            for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
            {
                Player *plr = sObjectMgr.GetPlayer(citr->guid);
                if (!plr || !plr->GetSession())
                    continue;
                if (plr->m_lookingForGroup.roles & LEADER)
                {
                    ChangeLeader(plr->GetGUID());
                    break;
                }
            }
            if (m_voteToKick.victim == m_leaderGuid)
                ChangeLeader(GetFirstMember()->getSource()->GetGUID());            
        }

        m_voteToKick.Reset();
        SendUpdate();
        return true;
    }
    return false;
}

void LfgGroup::SendBootPlayer(Player *plr)
{
    if (plr->GetGUID() == m_voteToKick.victim)
        return;

    WorldPacket data(SMSG_LFG_BOOT_PLAYER, 27 + m_voteToKick.reason.length());
    data << uint8(m_voteToKick.isInProggres);               // Vote in progress
    data << uint8(m_voteToKick.PlayerVoted(plr->GetGUID()));// Did Vote
    data << uint8(m_voteToKick.GetVote(plr->GetGUID()));    // Agree
    data << uint64(m_voteToKick.victim);                    // Victim GUID
    data << uint32(m_voteToKick.GetVotesNum(false));        // Total Votes
    data << uint32(m_voteToKick.GetVotesNum(true));         // Agree Count
    data << uint32(m_voteToKick.GetTimeLeft());             // Time Left
    data << uint32(LFG_VOTES_NEEDED);                       // Needed Votes
    data << m_voteToKick.reason.c_str();                    // Kick reason

    plr->GetSession()->SendPacket(&data);
}

uint32 LfgGroup::GetKilledBosses()
{
    if(!IsInDungeon())
        return 0;

    for(member_citerator citr = m_memberSlots.begin(); citr != m_memberSlots.end(); ++citr)
    {
        Player *member = sObjectMgr.GetPlayer(citr->guid);
        if (!member || !member->IsInWorld() || member->GetMapId() != GetDungeonInfo()->map)
            continue;
        InstanceSave *save = member->GetMap()->GetInstanceSave();
        if(save)
            return save->GetEncounterMask();
    }
    return 0;
}
