/*
 *  ChunkedTerrainGroup.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ChunkedTerrainGroup.h"
#include "TileGrid.h"
#include "MaterialManager.h"
#include "OctreeSceneManager.h"

#define PACK(x, y, z) ((x) << (BitsPerDim * 2)) | ((y) << BitsPerDim) | (z)

ChunkedTerrainGroup::ChunkedTerrainGroup(TileType type, TileGrid *grid,
OctreeSceneManager *scene, MaterialManager *manager): _type(type), _grid(grid),
_sceneManager(scene), _materialManager(manager)
{
    if (_grid->getWidth () % ChunkSize != 0 ||
        _grid->getHeight() % ChunkSize != 0 ||
        _grid->getDepth () % ChunkSize != 0)
    {
        THROW(InternalError, "ChunkedTerrain is using a chunk size of " << ChunkSize <<
            ". Currently, all terrain dimensions must be a multiple of this.");
    }
}

ChunkedTerrainGroup::~ChunkedTerrainGroup() {}

void ChunkedTerrainGroup::update(int x, int y, int z) {
    _grid->getTile(x, y, z);
    
}

void ChunkedTerrainGroup::updateAll() {
    clear();
}

void ChunkedTerrainGroup::clear() {

}
