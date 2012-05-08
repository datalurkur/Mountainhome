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
#include "VoxelGrid.h"

const int TerrainChunkRenderable::ChunkSize = Terrain::ChunkSize;

TerrainChunkRenderable::TerrainChunkRenderable(
    Terrain *terrain,
    TerrainChunk *parent,
    PaletteIndex index,
    Material *mat
):
    Renderable(NULL, mat),
    _terrain(terrain),
    _parent(parent),
    _preRenderPolyReduction(true),
    _dirty(true),
    _index(index)
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
        generateGeometry(_preRenderPolyReduction);
        _dirty = false;
    }
}

bool TerrainChunkRenderable::isIndexEmpty(int localX, int localY, int localZ, bool padBounries) {
    if (localX >= 0 && localX < ChunkSize &&
        localY >= 0 && localY < ChunkSize &&
        localZ >= 0 && localZ < ChunkSize)
    {
        return _parent->getLocalGrid()->getPaletteIndex(localX, localY, localZ) == VoxelPalette::EmptyVoxel;
    }
    else
    {
        int x = _parent->getXChunkIndex() * ChunkSize + localX;
        int y = _parent->getYChunkIndex() * ChunkSize + localY;
        int z = _parent->getZChunkIndex() * ChunkSize + localZ;
        if (x >= 0 && x < _terrain->getWidth() &&
            y >= 0 && y < _terrain->getHeight() &&
            z >= 0 && z < _terrain->getDepth())
        {
            return _terrain->getPaletteIndex(x, y, z) == VoxelPalette::EmptyVoxel;
        }
        else
        {
            return padBounries;
        }
    }
}
