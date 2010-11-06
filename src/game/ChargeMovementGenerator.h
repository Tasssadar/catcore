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

#ifndef MANGOS_CHARGEMOVEGEN_H
#define MANGOS_CHARGEMOVEGEN_H

#include "MovementGenerator.h"
#include "Path.h"

template<class T>
class MANGOS_DLL_SPEC ChargeMovementGenerator
: public MovementGeneratorMedium< T, ChargeMovementGenerator<T> >
{
    public:
        ChargeMovementGenerator(PointPath const& path, uint32 pointTime, uint32 start, uint32 end) :
          i_nextMoveTime(pointTime), m_pointTime(pointTime), m_path(path), m_start(start), m_end(end), curPoint(start) {}

        void Initialize(T &);
        void Finalize(T &);
        void Interrupt(T &);
        void Reset(T &);
        bool Update(T &, const uint32 &);
        void UpdateMapPosition(uint32 mapid, float &x ,float &y, float &z)
        {
            x = m_path[curPoint-1].x;
            y = m_path[curPoint-1].y;
            z = m_path[curPoint-1].z;
        }
        MovementGeneratorType GetMovementGeneratorType() const { return CHARGE_MOTION_TYPE; }

        bool GetDestination(float& x, float& y, float& z) const
        {
            x = m_path[m_end].x;
            y = m_path[m_end].y;
            z = m_path[m_end].z;
            return true;               
        }
    private:
        TimeTracker i_nextMoveTime;
        PointPath const m_path;
        uint32 curPoint;
        uint32 m_pointTime;
        uint32 m_start;
        uint32 m_end;
};

#endif