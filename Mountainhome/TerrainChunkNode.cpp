/*
 *  TerrainChunkNode.cpp
 *  Mountainhome
 *
 *  Created by loch on 1/24/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "TerrainChunkNode.h"
#include "BlockTerrainChunkRenderable.h"
#include "TileGrid.h"

TerrainChunkNode::TerrainChunkNode(
    int xChunkIndex, int yChunkIndex, int zChunkIndex,
    TilePalette *palette, TileGrid *grid
):
    Entity("TerrainChunkNode"),
    _xChunkIndex(xChunkIndex),
    _yChunkIndex(yChunkIndex),
    _zChunkIndex(zChunkIndex),
    _palette(palette),
    _grid(grid)
{
    std::ostringstream stringStream;
    stringStream << "TerrainChunkNode [" <<
        _xChunkIndex << ", " <<
        _yChunkIndex << ", " <<
        _zChunkIndex << "]";

    _name = stringStream.str();

    AABB3 localAABB;
    localAABB.setMinMax(
        Vector3((_xChunkIndex+0) * ChunkSize, (_yChunkIndex+0) * ChunkSize, (_zChunkIndex+0) * ChunkSize),
        Vector3((_xChunkIndex+1) * ChunkSize, (_yChunkIndex+1) * ChunkSize, (_zChunkIndex+1) * ChunkSize));
    expandLocalAABB(localAABB);

    _paletteRenderables.reserve(DefaultCapacity);
}

void TerrainChunkNode::markDirty(PaletteIndex index) {
    if (index >= _paletteRenderables.size()) {
        _paletteRenderables.resize(index + 1, NULL);
    }

    if (_paletteRenderables[index] == NULL) {
        _paletteRenderables[index] = new BlockTerrainChunkRenderable(
            _xChunkIndex, _yChunkIndex, _zChunkIndex, index,
            _grid, _palette->getMaterialForPalette(index));
        addRenderable(_paletteRenderables[index]);
    }

    _paletteRenderables[index]->markDirty();
}

int TerrainChunkNode::populate() {
    // Mark each tile individually, as there may be renderables that do not exist. This
    // will make sure that tile types that don't have renderables yet are still considered.
    for (int x = _xChunkIndex * ChunkSize; x <= (_xChunkIndex+1) * ChunkSize && x < _grid->getWidth(); x++) {
        for (int y = _yChunkIndex * ChunkSize; y <= (_yChunkIndex+1) * ChunkSize && y < _grid->getHeight(); y++) {
            for (int z = _zChunkIndex * ChunkSize; z <= (_zChunkIndex+1) * ChunkSize && z < _grid->getDepth(); z++) {
                PaletteIndex index = _grid->getPaletteIndex(x, y, z);
                if (index != TILE_EMPTY) { markDirty(index); }
            }
        }
    }

    // Actually generate the geometry for the renderables and return the renderable count.
    int count = 0;
    for (int i = 0; i < _paletteRenderables.size(); i++) {
        if (_paletteRenderables[i]) {
            _paletteRenderables[i]->generateGeometry();
            count++;
        }
    }

    return count;
}
