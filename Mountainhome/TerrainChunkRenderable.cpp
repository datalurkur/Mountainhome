/*
 *  TerrainChunkRenderable.cpp
 *  Mountainhome
 *
 *  Created by loch on 1/24/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "TerrainChunkRenderable.h"

TerrainChunkRenderable::TerrainChunkRenderable(
    TileGrid *grid,
    Material *mat
):
    Renderable(NULL, mat),
    _preRenderPolyReduction(true),
    _dirty(true),
    _grid(grid),
{}

void TerrainChunkRenderable::enablePreRenderPolyReduction(bool value) {
    _preRenderPolyReduction = value;
}

void TerrainChunkRenderable::preRenderNotice() {
    if (_dirty) {
        generateGeometry(_preRenderPolyReduction);
    }
}

void TerrainChunkRenderable::markDirty() {
    _dirty = true;
}
