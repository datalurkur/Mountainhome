/*
 *  TerrainChunkRenderable.cpp
 *  Mountainhome
 *
 *  Created by loch on 1/24/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "TerrainChunkRenderable.h"
#include "ChunkedTerrain.h"

const int TerrainChunkRenderable::ChunkSize = ChunkedTerrain::ChunkSize;

TerrainChunkRenderable::TerrainChunkRenderable(
    int xChunkIndex, int yChunkIndex, int zChunkIndex,
    PaletteIndex index, TileGrid *grid, Material *mat
):
    Renderable(NULL, mat),
    _xLoc(xChunkIndex * ChunkSize),
    _yLoc(yChunkIndex * ChunkSize),
    _zLoc(zChunkIndex * ChunkSize),
    _preRenderPolyReduction(true),
    _dirty(true),
    _index(index),
    _grid(grid)
{}

void TerrainChunkRenderable::enablePreRenderPolyReduction(bool value) {
    _preRenderPolyReduction = value;
}

void TerrainChunkRenderable::preRenderNotice() {
    if (_dirty) {
        generateGeometry();
    }
}

void TerrainChunkRenderable::markDirty() {
    _dirty = true;
}

void TerrainChunkRenderable::generateGeometry() {
    generateGeometry(_preRenderPolyReduction);
}
