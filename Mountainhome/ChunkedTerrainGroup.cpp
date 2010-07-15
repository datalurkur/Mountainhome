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

    // Instantiate a new ChunkedTerrainModel if it doesn't already exist.
    if (_chunks.find(chunkIndex) == _chunks.end()) {
        // Create a new terrain model.
        ChunkedTerrainModel *model = new ChunkedTerrainModel(_grid, _type,
            x / ChunkSize, y / ChunkSize, z / ChunkSize);

        // Info("Adding model: " << model->getName());

        // Create an entity in the scene manager for the model and assign a texture.
        Entity *entity = _sceneManager->createEntity(model, model->getName());
        Material *mat = _materialManager->getCachedResource(_type == 1 ? "grass" : "gravel");
        entity->setMaterial(mat);

        // Save the model in the chunks map.
        _chunks[chunkIndex] = model;
    }

    // Lastly, update the chunk and the surrounding chunks as needed.
    updateIfExists(x, y, z);

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
    // Info("Removing model " << itr->second->getName());
    _sceneManager->removeEntity(itr->second->getName());
    delete itr->second;
    _chunks.erase(itr);
}

void ChunkedTerrainGroup::updateIfExists(int x, int y, int z) {
    // Verify the x/y/z set is within the bounds of the grid.
    if (!_grid->isOutOfBounds(x, y, z)) {
        IndexType chunkIndex = GET_CHUNK_INDEX(x, y, z);
        ChunkLookupMap::iterator chunkItr = _chunks.find(chunkIndex);
        if (chunkItr != _chunks.end()) {
            if (chunkItr->second->update() == 0) {
                removeChunk(chunkItr);
            }
        }
    }
}
