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

#define GET_CHUNK_INDEX(x, y, z) ((((x) / ChunkSize) << (BitsPerDim * 2)) | (((y) / ChunkSize) << BitsPerDim) | ((z) / ChunkSize))

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
void ChunkedTerrainGroup::update(int x, int y, int z, bool doPolyReduction) {
    // Create and update the chunk for loc x, y, z.
    createChunkIfNeeded(x, y, z);
    updateIfExists(x, y, z, doPolyReduction);

    // Lastly, update the surrounding chunks as needed.
    if (x % ChunkSize == 0            ) { updateIfExists(x - 1, y, z, doPolyReduction); }
    if (x % ChunkSize == ChunkSize - 1) { updateIfExists(x + 1, y, z, doPolyReduction); }
    if (y % ChunkSize == 0            ) { updateIfExists(x, y - 1, z, doPolyReduction); }
    if (y % ChunkSize == ChunkSize - 1) { updateIfExists(x, y + 1, z, doPolyReduction); }
    if (z % ChunkSize == 0            ) { updateIfExists(x, y, z - 1, doPolyReduction); }
    if (z % ChunkSize == ChunkSize - 1) { updateIfExists(x, y, z + 1, doPolyReduction); }
}

void ChunkedTerrainGroup::createChunkIfNeeded(int x, int y, int z) {
    IndexType chunkIndex = GET_CHUNK_INDEX(x, y, z);

    // Instantiate a new ChunkedTerrainModel if it doesn't already exist.
    if (_chunks.find(chunkIndex) == _chunks.end()) {
        // Create a new terrain model.
        ChunkedTerrainModel *model = new ChunkedTerrainModel(_grid, _type,
            x / ChunkSize, y / ChunkSize, z / ChunkSize);

        // Create an entity in the scene manager for the model and assign a texture.
        Entity *entity = _sceneManager->createEntity(model, model->getName());
        Material *mat = _materialManager->getCachedResource(_type == 1 ? "grass" : "gravel");
        entity->setMaterial(mat);

        // Save the model in the chunks map.
        _chunks[chunkIndex] = model;
    }
}

void ChunkedTerrainGroup::updateAll(bool doPolyReduction) {
    for (int x = 0; x < _grid->getWidth(); x+=ChunkSize) {
        for (int y = 0; y < _grid->getHeight(); y+=ChunkSize) {
            for (int z = 0; z < _grid->getDepth(); z+=ChunkSize) {
                createChunkIfNeeded(x, y, z);
                updateIfExists(x, y, z, doPolyReduction);
            }
        }
    }
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

void ChunkedTerrainGroup::updateIfExists(int x, int y, int z, bool doPolyReduction) {
    // Verify the x/y/z set is within the bounds of the grid.
    if (!_grid->isOutOfBounds(x, y, z)) {
        IndexType chunkIndex = GET_CHUNK_INDEX(x, y, z);
        ChunkLookupMap::iterator chunkItr = _chunks.find(chunkIndex);
        if (chunkItr != _chunks.end()) {
            if (chunkItr->second->update(doPolyReduction) == 0) {
                removeChunk(chunkItr);
            }
        }
    }
}
