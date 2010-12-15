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
#ifndef __BATTLEGROUNDDS_H
#define __BATTLEGROUNDDS_H

class BattleGround;

class BattleGroundDSScore : public BattleGroundScore
{
    public:
        BattleGroundDSScore() {};
        virtual ~BattleGroundDSScore() {};
        //TODO fix me
};

#define DOORS_EVENT 254
#define WATERFALL_EVENT 250

class BattleGroundDS : public BattleGround
{
    friend class BattleGroundMgr;

    public:
        BattleGroundDS();
        ~BattleGroundDS();
        void Update(uint32 diff);

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player *plr);
        virtual void StartingEventCloseDoors();
        virtual void StartingEventOpenDoors();

        void RemovePlayer(Player *plr, uint64 guid);
        bool ObjectInLOS(Unit* caster, Unit* target);
        void HandleAreaTrigger(Player *Source, uint32 Trigger);
        bool SetupBattleGround();
        bool IsXYZPositionOK(float x, float y, float z);

    private:
        virtual void Reset();
        virtual void FillInitialWorldStates(WorldPacket &d, uint32& count);
        void HandleWatterfall();
        void KnockOutOfTubes();
        void HandleKillPlayer(Player* player, Player *killer);
        void KnockbackFromWaterfall();
        bool HandlePlayerUnderMap(Player * plr);
        void SpawnWaterfall(bool effect);
        Player* GetRandomPlayer();


        bool m_bKnocked;
        bool m_bTubeIsEmpty;
        uint8  m_uiWaterfallStage;
        uint32 m_uiKnockTimer;
        uint32 m_uiWaterfall;
        GameObject* m_WaterfallEffect;
        GameObject* m_WaterfallCollision;
};
#endif
