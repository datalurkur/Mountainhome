/*
 *  BlockTerrainChunkRenderable.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */


#include "BlockTerrainChunkRenderable.h"
#include "DynamicModel.h"
#include "TerrainChunk.h"
#include "VoxelGrid.h"

BlockTerrainChunkRenderable:: BlockTerrainChunkRenderable(
    Terrain *terrain,
    TerrainChunk *parent,
    PaletteIndex index,
    Material *mat
):
    TerrainChunkRenderable(terrain, parent, index, mat),
    _dynamicModel(NULL)
{
    // Create the dynamic model with enough space to handle normals at the edges.
    _dynamicModel = new DynamicModel();

    _renderOp = _dynamicModel->getRenderOp();
}

void BlockTerrainChunkRenderable::generateGeometry(bool doPolyReduction) {
    // Build the initial geometry for the chunk.
    for (int xPos = 0; xPos < ChunkSize; xPos++) {
        for (int yPos = 0; yPos < ChunkSize; yPos++) {
            for (int zPos = 0; zPos < ChunkSize; zPos++) {
                if (_parent->getLocalGrid()->getPaletteIndex(xPos, yPos, zPos) == _index) {
                    addGeometry(xPos, yPos, zPos);
                }
            }
        }
    }

    // This updates the _renderOp and clears the _dynamicModel of geometry.
    _dynamicModel->updateRenderOp(doPolyReduction);
}

void BlockTerrainChunkRenderable::addGeometry(int xPos, int yPos, int zPos) {
    // Left
    if (isIndexEmpty(xPos - 1, yPos, zPos)) {
        _dynamicModel->addFace(
            xPos, yPos    , zPos,
            xPos, yPos    , zPos + 1,
            xPos, yPos + 1, zPos + 1,
            DynamicModel::YZ_NEG);

        _dynamicModel->addFace(
            xPos, yPos + 1, zPos + 1,
            xPos, yPos + 1, zPos,
            xPos, yPos    , zPos,
            DynamicModel::YZ_NEG);
    }

    // Right
    if (isIndexEmpty(xPos + 1, yPos, zPos)) {
        _dynamicModel->addFace(
            xPos + 1, yPos    , zPos,
            xPos + 1, yPos + 1, zPos,
            xPos + 1, yPos + 1, zPos + 1,
            DynamicModel::YZ_POS);

        _dynamicModel->addFace(
            xPos + 1, yPos + 1, zPos + 1,
            xPos + 1, yPos    , zPos + 1,
            xPos + 1, yPos    , zPos,
            DynamicModel::YZ_POS);
    }

    // Front
    if (isIndexEmpty(xPos, yPos - 1, zPos)) {
        _dynamicModel->addFace(
            xPos    , yPos    , zPos,
            xPos + 1, yPos    , zPos,
            xPos + 1, yPos    , zPos + 1,
            DynamicModel::XZ_NEG);

        _dynamicModel->addFace(
            xPos + 1, yPos    , zPos + 1,
            xPos    , yPos    , zPos + 1,
            xPos    , yPos    , zPos,
            DynamicModel::XZ_NEG);
    }

    // Back
    if (isIndexEmpty(xPos, yPos + 1, zPos)) {
        _dynamicModel->addFace(
            xPos + 1, yPos + 1, zPos,
            xPos    , yPos + 1, zPos,
            xPos    , yPos + 1, zPos + 1,
            DynamicModel::XZ_POS);

        _dynamicModel->addFace(
            xPos    , yPos + 1, zPos + 1,
            xPos + 1, yPos + 1, zPos + 1,
            xPos + 1, yPos + 1, zPos,
            DynamicModel::XZ_POS);
    }

    // Bottom
    // XXXBMW: Don't put a bottom on the world. It's only visible from underneath the
    // block, so this is kind of silly.
    if (isIndexEmpty(xPos, yPos, zPos - 1, false)) {
        _dynamicModel->addFace(
            xPos    , yPos + 1, zPos,
            xPos + 1, yPos + 1, zPos,
            xPos + 1, yPos    , zPos,
            DynamicModel::XY_NEG);

        _dynamicModel->addFace(
            xPos + 1, yPos    , zPos,
            xPos    , yPos    , zPos,
            xPos    , yPos + 1, zPos,
            DynamicModel::XY_NEG);
    }

    // Top
    if (isIndexEmpty(xPos, yPos, zPos + 1)) {
        _dynamicModel->addFace(
            xPos    , yPos    , zPos + 1,
            xPos + 1, yPos    , zPos + 1,
            xPos + 1, yPos + 1, zPos + 1,
            DynamicModel::XY_POS);

        _dynamicModel->addFace(
            xPos + 1, yPos + 1, zPos + 1,
            xPos    , yPos + 1, zPos + 1,
            xPos    , yPos    , zPos + 1,
            DynamicModel::XY_POS);
    }
}
