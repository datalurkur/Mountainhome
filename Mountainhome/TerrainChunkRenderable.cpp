/*
 *  TerrainChunkRenderable.cpp
 *  Mountainhome
 *
 *  Created by loch on 1/24/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/Matrix.h>
#include "TerrainChunkRenderable.h"
#include "TerrainChunk.h"
#include "Terrain.h"

const int TerrainChunkRenderable::ChunkSize = Terrain::ChunkSize;

TerrainChunkRenderable::TerrainChunkRenderable(
    int xChunkIndex, int yChunkIndex, int zChunkIndex,
    PaletteIndex index, VoxelGrid *grid, Material *mat
):
    Renderable(NULL, mat),
    _xChunkIndex(xChunkIndex),
    _yChunkIndex(yChunkIndex),
    _zChunkIndex(zChunkIndex),
    _preRenderPolyReduction(true),
    _dirty(true),
    _index(index),
    _grid(grid)
{}

void TerrainChunkRenderable::enablePreRenderPolyReduction(bool value) {
    _preRenderPolyReduction = value;
}

void TerrainChunkRenderable::preRenderNotice() {
    // Call this to avoid mismatched call exist in the pre/post call safety code.
    Renderable::preRenderNotice();
    generateGeometry();
}

void TerrainChunkRenderable::markDirty() {
    _dirty = true;
}

void TerrainChunkRenderable::generateGeometry() {
    if (_dirty) {
        Info("Chunk [" << _xChunkIndex << ", " << _yChunkIndex << ", " << _zChunkIndex <<
             "] [" << this << "] regenerating renderable index " << (int)_index);
        generateGeometry(_preRenderPolyReduction);
        _dirty = false;
    }
}
