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

#ifndef MANGOS_MOTIONMASTER_H
#define MANGOS_MOTIONMASTER_H

#include "Common.h"
#include <stack>
#include <vector>
#include "Path.h"
#include "Object.h"

class MovementGenerator;
class Unit;

// Creature Entry ID used for waypoints show, visible only for GMs
#define VISUAL_WAYPOINT 1

// values 0 ... MAX_DB_MOTION_TYPE-1 used in DB
enum MovementGeneratorType
{
    IDLE_MOTION_TYPE                = 0,                    // IdleMovementGenerator.h
    RANDOM_MOTION_TYPE              = 1,                    // RandomMovementGenerator.h
    WAYPOINT_MOTION_TYPE            = 2,                    // WaypointMovementGenerator.h
    RANDOM_CIRCLE_MOTION_TYPE       = 3,                    // RandomMovementGenerator.h
    MAX_DB_MOTION_TYPE              = 4,                    // *** this and below motion types can't be set in DB.

    CONFUSED_MOTION_TYPE            = 5,                    // ConfusedMovementGenerator.h
    CHASE_MOTION_TYPE               = 6,                    // TargetedMovementGenerator.h
    HOME_MOTION_TYPE                = 7,                    // HomeMovementGenerator.h
    FLIGHT_MOTION_TYPE              = 8,                    // WaypointMovementGenerator.h
    POINT_MOTION_TYPE               = 9,                    // PointMovementGenerator.h
    FLEEING_MOTION_TYPE             = 10,                   // FleeingMovementGenerator.h
    DISTRACT_MOTION_TYPE            = 11,                   // IdleMovementGenerator.h
    ASSISTANCE_MOTION_TYPE          = 12,                   // PointMovementGenerator.h (first part of flee for assistance)
    ASSISTANCE_DISTRACT_MOTION_TYPE = 13,                   // IdleMovementGenerator.h (second part of flee for assistance)
    TIMED_FLEEING_MOTION_TYPE       = 14,                   // FleeingMovementGenerator.h (alt.second part of flee for assistance)
    FOLLOW_MOTION_TYPE              = 15,                   // TargetedMovementGenerator.h
    CHARGE_MOTION_TYPE              = 16,                   // ChargeMovementGenerator.h
};

enum MMCleanFlag
{
    MMCF_NONE   = 0,
    MMCF_UPDATE = 1, // Clear or Expire called from update
    MMCF_RESET  = 2  // Flag if need top()->Reset()
};

class MANGOS_DLL_SPEC MotionMaster : private std::stack<MovementGenerator *>
{
    private:
        typedef std::stack<MovementGenerator *> Impl;
        typedef std::vector<MovementGenerator *> ExpireList;
    public:

        explicit MotionMaster(Unit *unit) : i_owner(unit), m_expList(NULL), m_cleanFlag(MMCF_NONE), m_pauseTimer(0) {}
        ~MotionMaster();

        void Initialize();

        MovementGenerator* operator->(void) { return top(); }

        using Impl::top;
        using Impl::empty;

        typedef Impl::container_type::const_iterator const_iterator;
        const_iterator begin() const { return Impl::c.begin(); }
        const_iterator end() const { return Impl::c.end(); }

        void UpdateMotion(uint32 diff);
        void Clear(bool reset = true, bool all = false)
        {
            if (m_cleanFlag & MMCF_UPDATE)
                DelayedClean(reset, all);
            else
                DirectClean(reset, all);
        }
        void MovementExpired(bool reset = true)
        {
            if (m_cleanFlag & MMCF_UPDATE)
                DelayedExpire(reset);
            else
                DirectExpire(reset);
        }

        void MoveIdle();
        void MoveRandom();
        void MoveTargetedHome();
        void MoveFollow(Unit* target, float dist, float angle);
        void MoveChase(Unit* target, float dist = 0.0f, float angle = 0.0f);
        void MoveConfused();
        void MoveFleeing(Unit* enemy, uint32 timeLimit = 0);
        void MovePoint(uint32 id, float x, float y, float z, bool usePathfinding = true);
        void MovePoint(uint32 id, const Coords coord, bool usePathfinding = true)
        {
            MovePoint(id, coord.x, coord.y, coord.z, usePathfinding);
        }

        void MoveSeekAssistance(float x,float y,float z);
        void MoveSeekAssistanceDistract(uint32 timer);
        void MoveWaypoint();
        void MoveTaxiFlight(uint32 path, uint32 pathnode);
        void MoveDistract(uint32 timeLimit);
        void MoveCharge(PointPath const& path, uint32 pointTime, uint32 start, uint32 end);

        MovementGeneratorType GetCurrentMovementGeneratorType() const;

        void propagateSpeedChange();

        // will only work in MMgens where we have a target (TARGETED_MOTION_TYPE)
        void UpdateFinalDistanceToTarget(float fDistance);

        bool GetDestination(float &x, float &y, float &z);

        void PauseMoveGens(uint32 Timer) { m_pauseTimer = Timer; }
    private:
        void Mutate(MovementGenerator *m);                  // use Move* functions instead

        void DirectClean(bool reset, bool all);
        void DelayedClean(bool reset, bool all);

        void DirectExpire(bool reset);
        void DelayedExpire(bool reset);

        Unit       *i_owner;
        ExpireList *m_expList;
        uint8       m_cleanFlag;
        uint32      m_pauseTimer;
};
#endif
