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

uint32 packTileID(int x, int y) { return uint32(x << 16 | y); }

void Map::LoadNavMesh(int gx, int gy)
{
    if(!sWorld.MMapsEnabled())
        return;
   

    if(!m_navMesh)
    {
        uint32 pathLen = sWorld.GetDataPath().length() + strlen("mmaps/%03i.mmap")+1;
        char *fileName = new char[pathLen];
        snprintf(fileName, pathLen, (sWorld.GetDataPath()+"mmaps/%03i.mmap").c_str(), i_id);

        FILE* file = fopen(fileName, "rb");
        if (!file)
        {
            sLog.outDebug("Error: Could not open mmap file '%s'", fileName);
            delete [] fileName;
            return;
        }

        dtNavMeshParams params;
        uint32 offset;
        fread(&params, sizeof(dtNavMeshParams), 1, file);
        fread(&offset, sizeof(uint32), 1, file);
        fclose(file);
        delete [] fileName;

        m_navMesh = dtAllocNavMesh();
        if (!m_navMesh->init(&params))
        {
            dtFreeNavMesh(m_navMesh);
            m_navMesh = NULL;
            sLog.outError("Failed to initialize mmap %03u from file %s", i_id, fileName);
            return;
        }
    }

    // check if we already have this tile loaded
    uint32 packedGridPos = packTileID(gx, gy);
    if (m_mmapLoadedTiles.find(packedGridPos) != m_mmapLoadedTiles.end())
    {
        sLog.outError("Asked to load already loaded navmesh tile. %03u%02i%02i.mmtile", i_id, gx, gy);
        return;
    }

    // mmaps/0000000.mmtile
    uint32 pathLen = sWorld.GetDataPath().length() + strlen("mmaps/%03i%02i%02i.mmtile")+1;
    char *fileName = new char[pathLen];
    snprintf(fileName, pathLen, (sWorld.GetDataPath()+"mmaps/%03i%02i%02i.mmtile").c_str(), i_id, gx, gy);

    FILE *file = fopen(fileName, "rb");
    if (!file)
    {
        sLog.outDebug("Could not open mmtile file '%s'", fileName);
        delete [] fileName;
        return;
    }

    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* data =  (unsigned char*)dtAlloc(length, DT_ALLOC_PERM);
    ASSERT(data);

    fread(data, length, 1, file);
    fclose(file);
    delete [] fileName;

    dtMeshHeader* header = (dtMeshHeader*)data;
    dtTileRef tileRef = 0;

    // memory allocated for data is now managed by detour, and will be deallocated when the tile is removed
    dtStatus dtResult = m_navMesh->addTile(data, length, DT_TILE_FREE_DATA, 0, &tileRef);
    switch(dtResult)
    {
        case DT_SUCCESS:
        {
            m_mmapLoadedTiles.insert(std::pair<uint32, dtTileRef>(packedGridPos, tileRef));
            sLog.outDetail("Loaded mmtile %03i[%02i,%02i] into %03i[%02i,%02i]", i_id, gx, gy, i_id, header->x, header->y);
        }
        break;
        case DT_FAILURE_DATA_MAGIC:
        {
            sLog.outError("%03u%02i%02i.mmtile has an invalid header", i_id, gx, gy);
            dtFree(data);
        }
        break;
        case DT_FAILURE_DATA_VERSION:
        {
            sLog.outError("%03u%02i%02i.mmtile was built with Detour v%i, expected v%i",i_id, gx, gy, header->version, DT_NAVMESH_VERSION);
            dtFree(data);
        }
        break;
        case DT_FAILURE_OUT_OF_MEMORY:
        case DT_FAILURE:
        default:
        {
            sLog.outError("Could not load %03u%02i%02i.mmtile into navmesh", i_id, gx, gy);
            dtFree(data);
        }
        break;
    }
}

void Map::UnloadNavMesh(int gx, int gy)
{
    // navMesh was not loaded for this map
    if (!m_navMesh)
        return;

    uint32 packedGridPos = packTileID(gx, gy);
    if (m_mmapLoadedTiles.find(packedGridPos) == m_mmapLoadedTiles.end())
    {
        sLog.outError("Asked to unload not loaded navmesh tile. %03u%02i%02i.mmtile", i_id, gx, gy);
        return;
    }

    dtTileRef tileRef = m_mmapLoadedTiles[packedGridPos];

    // unload, and mark as non loaded
    if(DT_SUCCESS != m_navMesh->removeTile(tileRef, NULL, NULL))
    {
        sLog.outError("Could not unload %03u%02i%02i.mmtile from navmesh", i_id, gx, gy);
    }
    else
    {
        m_mmapLoadedTiles.erase(packedGridPos);
        sLog.outDetail("Unloaded mmtile %03i[%02i,%02i] from %03i", i_id, gx, gy, i_id);
    }
}

dtNavMesh const* Map::GetNavMesh() const
{
    return m_navMesh;
}
