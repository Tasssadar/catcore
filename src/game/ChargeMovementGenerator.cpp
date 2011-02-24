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
#include "ChargeMovementGenerator.h"
#include "DestinationHolderImp.h"
#include "Map.h"
#include "Util.h"
#include "Path.h"

template<>
void RandomMovementGenerator<Creature>::Initialize(Creature &creature)
{
    if (!creature.isAlive())
        return;

    creature.addUnitState(UNIT_STAT_ROAMING|UNIT_STAT_ROAMING_MOVE);

    if((m_start - m_end) == 0)
    {
        float dist = creature.GetDistance(m_path[m_end].x, m_path[m_end].y, m_path[m_end].z);
        float angle = creature.GetAngle(m_path[m_end].x, m_path[m_end].y);
        bool outdoor = creature.GetTerrain()->IsOutdoors(m_path[m_end].x, m_path[m_end].y, m_path[m_end].z);
        float x, y, z;
        creature.GetPosition(x, y, z);
        float itr = 1;
        if(m_pointTime > 200)
        {
            while(float(m_pointTime)/float(dist+itr/itr) <= 200 && itr < dist)
                itr+=0.3f;
        }
        else itr = dist;

        for(float i = 1; i < dist; i+=itr)
        {
            m_path.resize(m_path.size()+1);
            m_path.set(m_end+1, m_path[m_end]);
             
            x += cos(angle);
            y += sin(angle);
            creature.UpdateGroundPositionZ(x, y, z, outdoor ? 10.0f : 3.0f);
             m_path.set(m_end, PathNode(x,y,z));
            ++m_end;
        }
        m_pointTime = float(m_pointTime)/float((m_end - m_start) + 1);
        i_nextMoveTime.Reset(m_pointTime);

    }
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
    i_nextMoveTime.Update(diff);

    if (i_destinationHolder.HasArrived() && !creature.IsStopped() && !creature.canFly())
        creature.clearUnitState(UNIT_STAT_ROAMING_MOVE);

    if (!i_destinationHolder.HasArrived() && creature.IsStopped())
        creature.addUnitState(UNIT_STAT_ROAMING_MOVE);

    if(i_nextMoveTime.Passed())
    {
        i_nextMoveTime.Reset(m_pointTime);
        float angle = creature.GetAngle(m_path[curPoint].x,m_path[curPoint].y);
        creature.GetMap()->CreatureRelocation(&creature, m_path[curPoint].x, m_path[curPoint].y, m_path[curPoint].z, angle);
        
        if(curPoint >= end)
        {
            creature.clearUnitState(UNIT_STAT_ROAMING|UNIT_STAT_ROAMING_MOVE);
            return false;
        }
        ++curPoint;
    }
    return true;
}
