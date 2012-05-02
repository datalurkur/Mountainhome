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
#include "VoxelGrid.h"

BlockTerrainChunkRenderable:: BlockTerrainChunkRenderable(
    int xChunkIndex, int yChunkIndex, int zChunkIndex,
    PaletteIndex index, VoxelGrid *grid, Material *mat
):
    TerrainChunkRenderable(xChunkIndex, yChunkIndex, zChunkIndex, index, grid, mat),
    _dynamicModel(NULL)
{
    // Create the dynamic model with enough space to handle normals at the edges.
    _dynamicModel = new DynamicModel(xChunkIndex, yChunkIndex, zChunkIndex);
    _renderOp = _dynamicModel->getRenderOp();
}

void BlockTerrainChunkRenderable::generateGeometry(bool doPolyReduction) {
    // Build the initial geometry for the chunk.
    for (int xPos = 0; xPos < ChunkSize; xPos++) {
        for (int yPos = 0; yPos < ChunkSize; yPos++) {
            for (int zPos = 0; zPos < ChunkSize; zPos++) {
                if (_grid->getPaletteIndex(xPos, yPos, zPos) == _index) {
                    addGeometry(xPos, yPos, zPos);
                }
            }
        }
    }

    // This updates the _renderOp and clears the _dynamicModel of geometry.
    _dynamicModel->updateRenderOp(doPolyReduction);
}

void BlockTerrainChunkRenderable::addGeometry(int xPos, int yPos, int zPos) {
    // Only generate geometry for the sides/bottom if we're not on the lowest level.
    if (zPos > 0) {
        // Left
        if((xPos == 0) || (_grid->getPaletteIndex(xPos - 1, yPos, zPos) == VoxelPalette::EmptyVoxel)) {
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
        if((xPos == ChunkSize - 1) || (_grid->getPaletteIndex(xPos + 1, yPos, zPos) == VoxelPalette::EmptyVoxel)) {
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
        if ((yPos == 0) || (_grid->getPaletteIndex(xPos, yPos - 1, zPos) == VoxelPalette::EmptyVoxel)) {
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
        if ((yPos == ChunkSize - 1) || (_grid->getPaletteIndex(xPos, yPos + 1, zPos) == VoxelPalette::EmptyVoxel)) {
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
        if (_grid->getPaletteIndex(xPos, yPos, zPos - 1) == VoxelPalette::EmptyVoxel) {
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
    }

    // Top
    if ((zPos == ChunkSize - 1) || (_grid->getPaletteIndex(xPos, yPos, zPos + 1) == VoxelPalette::EmptyVoxel)) {
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
