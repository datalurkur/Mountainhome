/*
 *  TerrainChunk.cpp
 *  Mountainhome
 *
 *  Created by loch on 1/24/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "Terrain.h"
#include "TerrainChunk.h"
#include "BlockTerrainChunkRenderable.h"
#include "VoxelGrid.h"
#include "MatrixVoxelGrid.h"

TerrainChunk::TerrainChunk(
    int xChunkIndex,
    int yChunkIndex,
    int zChunkIndex,
    Terrain *terrain
):
    Entity("TerrainChunk"),
    _xChunkIndex(xChunkIndex),
    _yChunkIndex(yChunkIndex),
    _zChunkIndex(zChunkIndex),
    _terrain(terrain)
{
    std::ostringstream stringStream;
    stringStream << "TerrainChunk [" <<
        _xChunkIndex << ", " <<
        _yChunkIndex << ", " <<
        _zChunkIndex << "]";

    _name = stringStream.str();

    _paletteRenderables.reserve(DefaultCapacity);

    _grid = new MatrixVoxelGrid(ChunkSize, ChunkSize, ChunkSize);

    // Setup for a correct model matrix.
    setPosition(
        _xChunkIndex * ChunkSize,
        _yChunkIndex * ChunkSize,
        _zChunkIndex * ChunkSize);

    // Setup for correct frustum culling.
    AABB3 localAABB;
    localAABB.setMinMax(Vector3(0, 0, 0), Vector3(ChunkSize, ChunkSize, ChunkSize));
    expandLocalAABB(localAABB);
}

void TerrainChunk::markDirty(PaletteIndex index) {
    // Don't need to handle empty voxels.
    if (index == VoxelPalette::EmptyVoxel) {
        return;
    }

    if (index >= _paletteRenderables.size()) {
        _paletteRenderables.resize(index + 1, NULL);
    }

    if (_paletteRenderables[index] == NULL) {
        _paletteRenderables[index] = new BlockTerrainChunkRenderable(
            _terrain, this, index, _terrain->getPalette()->getMaterialForIndex(index));
        addRenderable(_paletteRenderables[index]);
    }

    _paletteRenderables[index]->markDirty();
}

int TerrainChunk::populate() {
    // Mark each voxel individually, as there may be renderables that do not exist. This
    // will make sure that voxel types that don't have renderables yet are still considered.
    for (int x = 0; x < _grid->getWidth(); x++) {
        for (int y = 0; y < _grid->getHeight(); y++) {
            for (int z = 0; z < _grid->getDepth(); z++) {
                PaletteIndex index = _grid->getPaletteIndex(x, y, z);
                if (index != VoxelPalette::EmptyVoxel) {
                    markDirty(index);
                }
            }
        }
    }

    // Actually generate the geometry for the renderables and return the renderable count.
    int count = 0;
    for (int i = 0; i < _paletteRenderables.size(); i++) {
        if (_paletteRenderables[i]) {
            // Mark each as dirty, as there may be renderables for voxels that do not exist
            // anymore, and thus were not marked above. These need their geometry removed.
            _paletteRenderables[i]->markDirty();
            _paletteRenderables[i]->generateGeometry();
            count++;
        }
    }

    return count;
}
