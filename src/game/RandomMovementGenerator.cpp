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

#include "Creature.h"
#include "MapManager.h"
#include "RandomMovementGenerator.h"
#include "DestinationHolderImp.h"
#include "Map.h"
#include "Util.h"

template<>
void
RandomMovementGenerator<Creature>::_setRandomLocation(Creature &creature)
{
    float X,Y,Z,nx,ny,nz,wander_distance,ori,dist;

    creature.GetRespawnCoord(X, Y, Z, &ori, &wander_distance);

    Z = creature.GetPositionZ();
    TerrainInfo const* map = creature.GetTerrain();

    // For 2D/3D system selection
    //bool is_land_ok  = creature.canWalk();                // not used?
    //bool is_water_ok = creature.canSwim();                // not used?
    bool is_air_ok   = creature.canFly();

    const float angle = rand_norm_f()*(M_PI_F*2.0f);
    const float range = rand_norm_f()*wander_distance;
    const float distanceX = range * cos(angle);
    const float distanceY = range * sin(angle);

    nx = X + distanceX;
    ny = Y + distanceY;
    nz = Z;

    // prevent invalid coordinates generation
    MaNGOS::NormalizeMapCoord(nx);
    MaNGOS::NormalizeMapCoord(ny);

    dist = distanceX*distanceX + distanceY*distanceY;

    if (is_air_ok)                                          // 3D system above ground and above water (flying mode)
    {
        // Limit height change
        const float distanceZ = rand_norm_f() * sqrtf(dist)/2.0f;
        if(urand(0, 1))
            nz += distanceZ;
        else
            nz -= distanceZ;
        if(fabs(nz-Z) > wander_distance)
            nz = Z;
        float tz = map->GetHeight(nx, ny, nz, true);  // Map check only, vmap needed here but need to alter vmaps checks for height.
        float wz = map->GetWaterLevel(nx, ny, nz);

        // Problem here, we must fly above the ground and water, not under. Let's try on next tick
        if (tz >= nz || wz >= nz)
            return;
    }
    //else if (is_water_ok)                                 // 3D system under water and above ground (swimming mode)
    else                                                    // 2D only
    {
        nz = Z;
        if (!creature.GetMap()->IsNextZcoordOK(nx, ny, nz, dist))
            return;                                         // let's forget this bad coords where a z cannot be find and retry at next tick
        creature.UpdateGroundPositionZ(nx, ny, nz, dist);
    }

    Traveller<Creature> traveller(creature);

    creature.SetOrientation(creature.GetAngle(nx, ny));
    i_destinationHolder.SetDestination(traveller, nx, ny, nz);
    creature.addUnitState(UNIT_STAT_ROAMING_MOVE);

    if (is_air_ok && !(creature.canWalk() && creature.IsAtGroundLevel(Coords(nx, ny, nz))))
    {
        i_nextMoveTime.Reset(i_destinationHolder.GetTotalTravelTime());
        creature.AddSplineFlag(SPLINEFLAG_UNKNOWN7);
    }
    //else if (is_water_ok)                                 // Swimming mode to be done with more than this check
    else
    {
        i_nextMoveTime.Reset(urand(500+i_destinationHolder.GetTotalTravelTime(), 10000+i_destinationHolder.GetTotalTravelTime()));
        creature.AddSplineFlag(SPLINEFLAG_WALKMODE);
    }
}

template<>
void RandomMovementGenerator<Creature>::Initialize(Creature &creature)
{
    if (!creature.isAlive())
        return;

    if (creature.canFly() && !(creature.canWalk() &&
        creature.IsAtGroundLevel()))
        creature.AddSplineFlag(SPLINEFLAG_UNKNOWN7);
    else
        creature.AddSplineFlag(SPLINEFLAG_WALKMODE);

    creature.addUnitState(UNIT_STAT_ROAMING|UNIT_STAT_ROAMING_MOVE);
    _setRandomLocation(creature);
}

template<>
void RandomMovementGenerator<Creature>::Reset(Creature &creature)
{
    Initialize(creature);
}

template<>
void RandomMovementGenerator<Creature>::Interrupt(Creature &creature)
{
    creature.clearUnitState(UNIT_STAT_ROAMING|UNIT_STAT_ROAMING_MOVE);
}

template<>
void RandomMovementGenerator<Creature>::Finalize(Creature &creature)
{
    creature.clearUnitState(UNIT_STAT_ROAMING|UNIT_STAT_ROAMING_MOVE);
}

template<>
bool RandomMovementGenerator<Creature>::Update(Creature &creature, const uint32 &diff)
{
    if (creature.hasUnitState(UNIT_STAT_NOT_MOVE | UNIT_STAT_ON_VEHICLE))
    {
        i_nextMoveTime.Update(i_nextMoveTime.GetExpiry());  // Expire the timer
        creature.clearUnitState(UNIT_STAT_ROAMING_MOVE);
        return true;
    }

    i_nextMoveTime.Update(diff);

    if (i_destinationHolder.HasArrived() && !creature.IsStopped() && !creature.canFly())
        creature.clearUnitState(UNIT_STAT_ROAMING_MOVE);

    if (!i_destinationHolder.HasArrived() && creature.IsStopped())
        creature.addUnitState(UNIT_STAT_ROAMING_MOVE);

    CreatureTraveller traveller(creature);

    if (i_destinationHolder.UpdateTraveller(traveller, diff, false, true))
    {
        if (!IsActive(creature))                        // force stop processing (movement can move out active zone with cleanup movegens list)
            return true;                                // not expire now, but already lost

        if (i_nextMoveTime.Passed())
        {
            float x,y,z;
            if (i_destinationHolder.HasDestination())
                i_destinationHolder.GetLocationNowNoMicroMovement(x,y,z);
            else
                creature.GetPosition(x,y,z);

            if (creature.canFly() && !(creature.canWalk() && creature.IsAtGroundLevel(Coords(x,y,z))))
                creature.AddSplineFlag(SPLINEFLAG_UNKNOWN7);
            else
                creature.AddSplineFlag(SPLINEFLAG_WALKMODE);

            _setRandomLocation(creature);
        }
        else if (creature.isPet() && creature.GetOwner() && !creature.IsWithinDist(creature.GetOwner(), PET_FOLLOW_DIST+2.5f))
        {
            creature.AddSplineFlag(SPLINEFLAG_WALKMODE);
            _setRandomLocation(creature);
        }
    }
    return true;
}

template<>
bool RandomMovementGenerator<Creature>::GetResetPosition(Creature& c, float& x, float& y, float& z)
{
    float radius;
    c.GetRespawnCoord(x, y, z, NULL, &radius);

    // use current if in range
    if (c.IsWithinDist2d(x,y,radius))
        c.GetPosition(x,y,z);

    return true;
}


// random circles
template<>
void
RandomCircleMovementGenerator<Creature>::fillSplineWayPoints(Creature &creature)
{
    m_splineMap.clear();
    float spawnX, spawnY, spawnZ, spawnO, spawnDist, x, y, z;
    creature.GetRespawnCoord(spawnX, spawnY, spawnZ, &spawnO, &spawnDist);
    creature.GetPosition(x,y,z);
    //Add first waypoint
   /* SplineWayPointInfo *firstwp = new SplineWayPointInfo();
    firstwp->x = x;
    firstwp->y = y;
    firstwp->z = z;
    m_splineMap.insert(std::make_pair<uint32, SplineWayPointInfo*>(0, firstwp)); */

    //calculate other ones
    float m_fDistance = creature.GetDistance2d(spawnX, spawnY);
    float m_fLenght = 2*M_PI_F*m_fDistance;

    float m_fAngle = (M_PI_F - ((2*M_PI_F) / m_fLenght)) / 2;

    if (m_bClockWise)
        m_fAngle += ((2*M_PI_F) / m_fLenght) + creature.GetAngle(spawnX, spawnY);
    else
        m_fAngle = creature.GetAngle(spawnX, spawnY) - m_fAngle - ((2*M_PI_F) / m_fLenght);
    //because it cant be lower than 0 or bigger than 2*PI
    m_fAngle = (m_fAngle >= 0) ? m_fAngle : 2 * M_PI_F + m_fAngle;
    m_fAngle = (m_fAngle <= 2*M_PI_F) ? m_fAngle : m_fAngle - 2 * M_PI_F;
    float creature_speed = creature.GetSpeed(MOVE_FLIGHT) / 2;

    float lastx = x;
    float lasty = y;
    for(uint32 wpId = 0; wpId < 30; ++wpId)
    {
        bool canIncerase = true;
        bool canDecerase = true;
        if (m_fDistance > spawnDist)
            canIncerase = false;
        if (m_fDistance < 1)
            canDecerase = false;
        uint8 tmp = urand(0,2); // 0 nothing, 1 incerase, 2 decerase
        if (tmp == 1 && canIncerase)
            m_fDistance += 0.5f;
        else if (tmp == 2 && canDecerase)
            m_fDistance -= 0.5f;

        m_fLenght = 2*M_PI_F*m_fDistance;
        float m_fRotateAngle = ((2*M_PI_F) / m_fLenght); // Moving by half of speed every 500ms
        if (m_bClockWise)
            m_fAngle -= m_fRotateAngle;
        else
            m_fAngle += m_fRotateAngle;
        //because it cant be lower than 0 or bigger than 2*PI
        m_fAngle = (m_fAngle >= 0) ? m_fAngle : 2 * M_PI_F + m_fAngle;
        m_fAngle = (m_fAngle <= 2*M_PI_F) ? m_fAngle : m_fAngle - 2 * M_PI_F;
        lastx += cos(m_fAngle)*creature_speed;
        lasty += sin(m_fAngle)*creature_speed;
        MaNGOS::NormalizeMapCoord(lastx);
        MaNGOS::NormalizeMapCoord(lasty);
        Position *wp = new Position();
        wp->x = lastx;
        wp->y = lasty;
        wp->z = z;
        wp->o = m_fAngle;
        m_splineMap.insert(std::make_pair<uint32, Position*>(wpId, wp));
    }
    i_nextMoveTime.Reset(500);
}

template<>
void RandomCircleMovementGenerator<Creature>::Initialize(Creature &creature)
{
    if (!creature.isAlive())
        return;

    if (creature.canFly() && !(creature.canWalk() &&
        creature.IsAtGroundLevel()))
        creature.AddSplineFlag(SPLINEFLAG_UNKNOWN7);
    else
        creature.AddSplineFlag(SPLINEFLAG_WALKMODE);

    creature.addUnitState(UNIT_STAT_ROAMING|UNIT_STAT_ROAMING_MOVE);
    m_bClockWise = urand(0, 1) ? true : false;
    i_wpId = 0;
    fillSplineWayPoints(creature);
    SplineFlags flags = SplineFlags(SPLINEFLAG_FORWARD | SPLINEFLAG_UNKNOWN7 | SPLINEFLAG_FLYING | creature.GetSplineFlags());
    creature.SendSplineMove(&m_splineMap, SPLINETYPE_NORMAL, flags, 500, NULL);
}

template<>
void RandomCircleMovementGenerator<Creature>::Reset(Creature &creature)
{
    m_splineMap.clear();
    creature.SendMonsterMove(0,0,0, SPLINETYPE_STOP, SPLINEFLAG_NONE, 500, NULL);
    Initialize(creature);
}

template<>
void RandomCircleMovementGenerator<Creature>::Interrupt(Creature &creature)
{
    creature.clearUnitState(UNIT_STAT_ROAMING|UNIT_STAT_ROAMING_MOVE);
    m_splineMap.clear();
    creature.SendMonsterMove(0,0,0, SPLINETYPE_STOP, SPLINEFLAG_NONE, 500, NULL);
}

template<>
void RandomCircleMovementGenerator<Creature>::Finalize(Creature &creature)
{
    creature.clearUnitState(UNIT_STAT_ROAMING|UNIT_STAT_ROAMING_MOVE);
    m_splineMap.clear();
    creature.SendMonsterMove(0,0,0, SPLINETYPE_STOP, SPLINEFLAG_NONE, 500, NULL);
}

template<>
bool RandomCircleMovementGenerator<Creature>::Update(Creature &creature, const uint32 &diff)
{   
    if (creature.hasUnitState(UNIT_STAT_NOT_MOVE | UNIT_STAT_ON_VEHICLE))
    {
        i_nextMoveTime.Update(i_nextMoveTime.GetExpiry());  // Expire the timer
        creature.clearUnitState(UNIT_STAT_ROAMING_MOVE);
        return true;
    }

    i_nextMoveTime.Update(diff);

    if (!creature.IsStopped() && !creature.canFly())
        creature.clearUnitState(UNIT_STAT_ROAMING_MOVE);

    if (creature.IsStopped())
        creature.addUnitState(UNIT_STAT_ROAMING_MOVE);

    if (!IsActive(creature))                        // force stop processing (movement can move out active zone with cleanup movegens list)
    {
        Interrupt(creature);
        return true;
    }

    if (m_splineMap.empty())
    {
        Reset(creature);
        return true;
    }

    if (i_nextMoveTime.Passed())
    {
        if (m_splineMap.find(i_wpId) == m_splineMap.end())
        {
            Reset(creature);
            return true;
        }
        SplineWayPointMap::iterator wp = m_splineMap.find(i_wpId);
        //rellocate but not send
        creature.Relocate(wp->second->x, wp->second->y, wp->second->z, wp->second->o); 
        if (i_wpId == 29) // not last wp
        {
            Initialize(creature);
            return true;
        }
        i_wpId++;
        i_nextMoveTime.Reset(500);
    }
    return true;
}

template<>
bool RandomCircleMovementGenerator<Creature>::GetResetPosition(Creature& c, float& x, float& y, float& z)
{
    float radius;
    c.GetRespawnCoord(x, y, z, NULL, &radius);

    // use current if in range
    if (c.IsWithinDist2d(x,y,radius))
        c.GetPosition(x,y,z);

    return true;
}

