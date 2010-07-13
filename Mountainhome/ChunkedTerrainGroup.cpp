/*
 *  ChunkedTerrainGroup.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ChunkedTerrainGroup.h"
#include "ChunkedTerrainModel.h"

#include "TileGrid.h"
#include "OctreeSceneManager.h"

#include <Render/MaterialManager.h>
#include <Render/Entity.h>

#define GET_CHUNK_INDEX(x, y, z) (((x) / ChunkSize) << (BitsPerDim * 2)) | (((y) / ChunkSize) << BitsPerDim) | ((z) / ChunkSize)

ChunkedTerrainGroup::ChunkedTerrainGroup(TileType type, TileGrid *grid,
OctreeSceneManager *scene, MaterialManager *manager): _type(type), _grid(grid),
_sceneManager(scene), _materialManager(manager)
{
    ///\todo XXXBMW: This restriction can almost certainly be removed. I just don't feel like thinking about it atm.
    if (_grid->getWidth () % ChunkSize != 0 ||
        _grid->getHeight() % ChunkSize != 0 ||
        _grid->getDepth () % ChunkSize != 0)
    {
        THROW(InternalError, "ChunkedTerrain is using a chunk size of " << ChunkSize <<
            ". Currently, all terrain dimensions must be a multiple of this.");
    }
}

ChunkedTerrainGroup::~ChunkedTerrainGroup() {}

// Assumptions: The counting system only works if this method is only called on a change!!
void ChunkedTerrainGroup::update(int x, int y, int z) {
    IndexType chunkIndex = GET_CHUNK_INDEX(x, y, z);
    TileType newType = _grid->getTile(x, y, z);

    // Instantiate a new ChunkedTerrainModel if it doesn't already exist.
    ChunkLookupMap::iterator chunkItr = _chunks.find(chunkIndex);
    if (chunkItr == _chunks.end()) {
        // Create a new terrain model and give it a name.
        _chunks[chunkIndex] = new ChunkedTerrainModel(_grid, _type, x / ChunkSize, y / ChunkSize, z / ChunkSize);
        chunkItr = _chunks.find(chunkIndex);

        // Create an entity in the scene manager for the model.
        Entity *entity = _sceneManager->createEntity(chunkItr->second, chunkItr->second->getName());
        entity->setMaterial(_materialManager->getCachedResource("grass"));
    }

    // Update the chunk model. If update returns true, the model is empty and we need to
    // delete it from the scene and from the chunk group.
    if (chunkItr->second->update(newType != _type ? -1 : 1)) {
        removeChunk(chunkItr);
    }

    // Lastly, update surrounding chunks as needed.
    if (x % ChunkSize == 0            ) { updateIfExists(x - 1, y, z); }
    if (x % ChunkSize == ChunkSize - 1) { updateIfExists(x + 1, y, z); }
    if (y % ChunkSize == 0            ) { updateIfExists(x, y - 1, z); }
    if (y % ChunkSize == ChunkSize - 1) { updateIfExists(x, y + 1, z); }
    if (z % ChunkSize == 0            ) { updateIfExists(x, y, z - 1); }
    if (z % ChunkSize == ChunkSize - 1) { updateIfExists(x, y, z + 1); }
}

void ChunkedTerrainGroup::updateAll() {
    THROW(NotImplementedError, "XXXBMW: Haven't gotten here, yet.");
}

void ChunkedTerrainGroup::clear() {
    ChunkLookupMap::iterator itr;
    for (itr = _chunks.begin(); itr != _chunks.end(); itr++) {
        removeChunk(itr);
    }
}

void ChunkedTerrainGroup::removeChunk(ChunkLookupMap::iterator itr) {
    _sceneManager->removeEntity(itr->second->getName());
    delete itr->second;
    _chunks.erase(itr);
}

void ChunkedTerrainGroup::updateIfExists(int x, int y, int z) {
    IndexType chunkIndex = GET_CHUNK_INDEX(x, y, z);
    ChunkLookupMap::iterator chunkItr = _chunks.find(chunkIndex);
    if (chunkItr == _chunks.end()) {
        chunkItr->second->update(0);
    }
}
