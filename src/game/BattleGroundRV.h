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
#ifndef __BATTLEGROUNDRV_H
#define __BATTLEGROUNDRV_H

class BattleGround;
enum
{
    BATTLEGROUND_RV_EVENT_PILLARS               = 250,
    BATTLEGROUND_RV_EVENT_PILLARS_EQUIPMENT     = 251,
    BATTLEGROUND_RV_EVENT_BUFFS                 = 252,
    BATTLEGROUND_RV_EVENT_ELEVATORS             = 254,
    
    BATTLEGROUND_RV_SUBEVENT_PILLARS_FAR        = 0,
    BATTLEGROUND_RV_SUBEVENT_PILLARS_NEAR       = 1,

    BATTLEGROUND_RV_ELEVATING_TIME              = 18000,

    PILLAR_COUNT                                = 4
};

class BattleGroundRVScore : public BattleGroundScore
{
    public:
        BattleGroundRVScore() {};
        virtual ~BattleGroundRVScore() {};
        //TODO fix me
};

class BattleGroundRV : public BattleGround
{
    friend class BattleGroundMgr;

    public:
        BattleGroundRV();
        ~BattleGroundRV();
        void Update(uint32 diff);

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player *plr);
        virtual void StartingEventCloseDoors();
        virtual void StartingEventOpenDoors();

        bool ObjectInLOS(Unit* caster, Unit* target);
        void RemovePlayer(Player *plr, uint64 guid);
        void HandleAreaTrigger(Player *Source, uint32 Trigger);
        bool SetupBattleGround();
        virtual void Reset();
        virtual void FillInitialWorldStates(WorldPacket &d, uint32& count);
        void HandleKillPlayer(Player* player, Player *killer);
        bool HandlePlayerUnderMap(Player * plr);
        void ClickOnPillar(uint64 const& guid);
        bool IsXYZPositionOK(float x, float y, float z);

    private:
        void ChangeActivePillars();
        void ClickEvent(uint8 event1, uint8 event2 /*=0*/);
        //uint32 m_uiTeleport;
        uint32 m_uiPillarChanging;
        //uint32 m_uiTexturesCheck;
        uint32 m_bElevatingTimer;
        bool m_bElevatorsUp;

        std::list<GameObject*> m_lPillars;
};
#endif
