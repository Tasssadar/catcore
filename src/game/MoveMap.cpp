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

#include "Map.h"
#include "Log.h"
#include "Utilities/UnorderedMap.h"
#include "World.h"

inline uint32 packTileID(uint32 tileX, uint32 tileY) { return tileX<<16 | tileY; }
inline void unpackTileID(uint32 ID, uint32 &tileX, uint32 &tileY) { tileX = ID>>16; tileY = ID&0xFF; }

void Map::LoadNavMesh(int gx, int gy)
{
    if(!sWorld.MMapsEnabled())
        return;

<<<<<<< HEAD
=======
    char fileName[512];
    FILE* file;
>>>>>>> parent of c82de25... + delete/free missmatch.

    if(!m_navMesh)
    {
        sprintf(fileName, "%smmaps/%03i.mmap", sWorld.GetDataPath().c_str(), i_id);
        file = fopen(fileName, "rb");

        if(!file)
        {
            sLog.outDebug("Error: Could not open mmap file '%s'", fileName);
            return;
        }

        dtNavMeshParams params;
        uint32 offset;
        fread(&params, sizeof(dtNavMeshParams), 1, file);
        fread(&offset, sizeof(uint32), 1, file);
        fclose(file);

        m_navMesh = dtAllocNavMesh();
        if(!m_navMesh->init(&params))
        {
<<<<<<< HEAD
            dtFreeNavMesh(m_navMesh);
            m_navMesh = NULL;
<<<<<<< HEAD
            sLog.outError("Error: Failed to initialize mmap %03u from file %s", m_mapId, fileName);
=======
=======
            delete m_navMesh;
            m_navMesh = 0;
>>>>>>> parent of c82de25... + delete/free missmatch.
            sLog.outError("Error: Failed to initialize mmap %03u from file %s", i_id, fileName);
>>>>>>> parent of 6c22936... + Changes to fit new Recast API.
            return;
        }
    }

    uint32 packedGridPos = packTileID(uint32(gx), uint32(gy));
    if(m_mmapTileMap.find(packedGridPos) != m_mmapTileMap.end())
        return;

    // mmaps/0000000.mmtile
    sprintf(fileName, "%smmaps/%03i%02i%02i.mmtile", sWorld.GetDataPath().c_str(), i_id, gx, gy);
    file = fopen(fileName, "rb");

    if(!file)
    {
        sLog.outDebug("Error: Could not open mmtile file '%s'", fileName);
        return;
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* data =  (unsigned char*)dtAlloc(length, DT_ALLOC_PERM);
<<<<<<< HEAD
    ASSERT(data);

=======
>>>>>>> parent of c82de25... + delete/free missmatch.
    fread(data, length, 1, file);
    fclose(file);

    dtMeshHeader* header = (dtMeshHeader*)data;
    if (header->magic != DT_NAVMESH_MAGIC)
    {
<<<<<<< HEAD
<<<<<<< HEAD
        sLog.outError("Error: %03u%02i%02i.mmtile has an invalid header", m_mapId, gx, gy);
=======
        sLog.outError("%03u%02i%02i.mmtile has an invalid header", i_id, gx, gy);
>>>>>>> parent of 303c8a7... + Store actualt dtTileRef inside m_mmapLoadedTiles.
=======
        sLog.outError("Error: %03u%02i%02i.mmtile has an invalid header", i_id, gx, gy);
>>>>>>> parent of 6c22936... + Changes to fit new Recast API.
        dtFree(data);
        return;
    }
    if (header->version != DT_NAVMESH_VERSION)
    {
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
        sLog.outError("Error: %03u%02i%02i.mmtile was built with Detour v%i, expected v%i",m_mapId, gx, gy, header->version, DT_NAVMESH_VERSION);
=======
        sLog.outError("%03u%02i%02i.mmtile was built with Detour v%i, expected v%i",i_id, gx, gy, header->version, DT_NAVMESH_VERSION);
>>>>>>> parent of 303c8a7... + Store actualt dtTileRef inside m_mmapLoadedTiles.
=======
        sLog.outError("Error: %03u%02i%02i.mmtile was built with Detour v%i, expected v%i",i_id, gx, gy, header->version, DT_NAVMESH_VERSION);
>>>>>>> parent of 6c22936... + Changes to fit new Recast API.
=======
        sLog.outError("Error: %03u%02i%02i.mmtile was built with Detour v%i, expected v%i",
                              i_id, gx, gy,                 header->version, DT_NAVMESH_VERSION);
>>>>>>> parent of c82de25... + delete/free missmatch.
        dtFree(data);
        return;
    }

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    if (!m_navMesh->addTile(data, length, DT_TILE_FREE_DATA))
    {
        sLog.outError("Error: could not load %03u%02i%02i.mmtile into navmesh", m_mapId, gx, gy);
=======
    if (DT_SUCCESS != m_navMesh->addTile(data, length, DT_TILE_FREE_DATA, 0, NULL))
    {
        sLog.outError("Could not load %03u%02i%02i.mmtile into navmesh", i_id, gx, gy);
>>>>>>> parent of 303c8a7... + Store actualt dtTileRef inside m_mmapLoadedTiles.
=======
    if (!m_navMesh->addTile(data, length, DT_TILE_FREE_DATA))
=======
    if(!m_navMesh->addTile(data, length, DT_TILE_FREE_DATA))
>>>>>>> parent of 03305e6... lots of code style fixes
    {
        sLog.outError("Error: could not load %03u%02i%02i.mmtile into navmesh", i_id, gx, gy);
>>>>>>> parent of 6c22936... + Changes to fit new Recast API.
        dtFree(data);
        return;
    }

    // memory allocated for data is now managed by detour, and will be deallocated when the tile is removed

    uint32 packedTilePos = packTileID(uint32(header->x), uint32(header->y));
<<<<<<< HEAD
    m_mmapLoadedTiles.insert(std::pair<uint32, uint32>(packedGridPos, packedTilePos));
<<<<<<< HEAD
<<<<<<< HEAD
    sLog.outDetail("Loaded mmtile %03i[%02i,%02i] into %03i[%02i,%02i]", m_mapId, gx, gy, m_mapId, header->x, header->y);
=======
    sLog.outDetail("Loaded mmtile %03i[%02i,%02i] into %03i[%02i,%02i]", i_id, gx, gy, i_id, header->x, header->y);
>>>>>>> parent of 303c8a7... + Store actualt dtTileRef inside m_mmapLoadedTiles.
=======
=======
    m_mmapTileMap.insert(std::pair<uint32, uint32>(packedGridPos, packedTilePos));
>>>>>>> parent of 03305e6... lots of code style fixes
    sLog.outDetail("Loaded mmtile %03i[%02i,%02i] into %03i(%u)[%02i,%02i]", i_id, gx, gy, i_id, GetInstanceId(), header->x, header->y);
>>>>>>> parent of 5029e5d... + Memory leak in NavMesh loading code.
}

void Map::UnloadNavMesh(int gx, int gy)
{
    uint32 packedGridPos = packTileID(uint32(gx), uint32(gy));
    if(m_mmapTileMap.find(packedGridPos) == m_mmapTileMap.end())
        return;
<<<<<<< HEAD
    }
=======

<<<<<<< HEAD
>>>>>>> parent of 303c8a7... + Store actualt dtTileRef inside m_mmapLoadedTiles.
    uint32 packedTilePos = m_mmapLoadedTiles[packedGridPos];
=======
    uint32 packedTilePos = m_mmapTileMap[packedGridPos];
>>>>>>> parent of 03305e6... lots of code style fixes
    uint32 tileX, tileY;
    unpackTileID(packedTilePos, tileX, tileY);

    // unload, and mark as non loaded
<<<<<<< HEAD
<<<<<<< HEAD
    if(m_navMesh->removeTile(m_navMesh->getTileRefAt(int(tileX), int(tileY)), 0, 0))
=======
    if(DT_SUCCESS != m_navMesh->removeTile(m_navMesh->getTileRefAt(int(tileX), int(tileY)), NULL, NULL))
    {
        sLog.outError("Could not unload %03u%02i%02i.mmtile from navmesh", i_id, gx, gy);
    }
    else
    {
>>>>>>> parent of 303c8a7... + Store actualt dtTileRef inside m_mmapLoadedTiles.
        m_mmapLoadedTiles.erase(packedGridPos);
<<<<<<< HEAD

    sLog.outDetail("Unloaded mmtile %03i[%02i,%02i] from %03i", m_mapId, gx, gy, m_mapId);
=======
        sLog.outDetail("Unloaded mmtile %03i[%02i,%02i] from %03i(%u)", i_id, gx, gy, i_id, GetInstanceId());
    }
>>>>>>> parent of 5029e5d... + Memory leak in NavMesh loading code.
=======
    if(m_navMesh->removeTile(m_navMesh->getTileRefAt(int(tileX), int(tileY)), 0, 0))
        m_mmapTileMap.erase(packedGridPos);

    sLog.outDetail("Unloaded mmtile %03i[%02i,%02i] from %03i(%u)", i_id, gx, gy, i_id, GetInstanceId());
>>>>>>> parent of 6c22936... + Changes to fit new Recast API.
}

dtNavMesh* Map::GetNavMesh()
{
    return m_navMesh;
}

