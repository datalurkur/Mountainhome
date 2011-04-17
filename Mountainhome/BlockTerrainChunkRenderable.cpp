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
#include "TileGrid.h"

BlockTerrainChunkRenderable:: BlockTerrainChunkRenderable(
    int xChunkIndex, int yChunkIndex, int zChunkIndex,
    PaletteIndex index, TileGrid *grid, Material *mat
):
    TerrainChunkRenderable(xChunkIndex, yChunkIndex, zChunkIndex, index, grid, mat),
    // Manually set the chunk size for "special" chunks that occur at the edges and have odd sizes
    _xChunkSize(Math::Min(grid->getWidth()  - _xLoc, ChunkSize)),
    _yChunkSize(Math::Min(grid->getHeight() - _yLoc, ChunkSize)),
    _zChunkSize(Math::Min(grid->getDepth()  - _zLoc, ChunkSize))
{}

void BlockTerrainChunkRenderable::generateGeometry(bool doPolyReduction) {
    // Clean up the old memory.
    if (_renderOp) {
        delete _renderOp;
        _renderOp = NULL;
    }

    // Create the dynamic model with enough space to handle normals at the edges.
    DynamicModel *model = new DynamicModel(_xChunkSize + 1, _yChunkSize + 1, _xLoc, _yLoc, _zLoc);

    // Build the initial geometry for the chunk.
    for (int xPos = _xLoc; xPos < _xLoc + _xChunkSize; xPos++) {
        for (int yPos = _yLoc; yPos < _yLoc + _yChunkSize; yPos++) {
            for (int zPos = _zLoc; zPos < _zLoc + _zChunkSize; zPos++) {
                if (_grid->getPaletteIndex(xPos, yPos, zPos) == _index) {
                    addGeometry(xPos, yPos, zPos, model);
                }
            }
        }
    }

    _renderOp = model->generateRenderOp(doPolyReduction);

    // Clean up the dynamic model.
    delete model;
}

void BlockTerrainChunkRenderable::addGeometry(int xPos, int yPos, int zPos, DynamicModel *model) {
    // Only generate geometry for the sides/bottom if we're not on the lowest level.
    if (zPos > 0) {
        // Left
        if((xPos == 0) || (_grid->getPaletteIndex(xPos - 1, yPos, zPos) == TilePalette::EmptyTile)) {
            model->addFace(
                xPos, yPos    , zPos,
                xPos, yPos    , zPos + 1,
                xPos, yPos + 1, zPos + 1,
                DynamicModel::YZ_NEG);

            model->addFace(
                xPos, yPos + 1, zPos + 1,
                xPos, yPos + 1, zPos,
                xPos, yPos    , zPos,
                DynamicModel::YZ_NEG);
        }

        // Right
        if((xPos == _grid->getWidth() - 1) || (_grid->getPaletteIndex(xPos + 1, yPos, zPos) == TilePalette::EmptyTile)) {
            model->addFace(
                xPos + 1, yPos    , zPos,
                xPos + 1, yPos + 1, zPos,
                xPos + 1, yPos + 1, zPos + 1,
                DynamicModel::YZ_POS);

            model->addFace(
                xPos + 1, yPos + 1, zPos + 1,
                xPos + 1, yPos    , zPos + 1,
                xPos + 1, yPos    , zPos,
                DynamicModel::YZ_POS);
        }

        // Front
        if ((yPos == 0) || (_grid->getPaletteIndex(xPos, yPos - 1, zPos) == TilePalette::EmptyTile)) {
            model->addFace(
                xPos    , yPos    , zPos,
                xPos + 1, yPos    , zPos,
                xPos + 1, yPos    , zPos + 1,
                DynamicModel::XZ_NEG);

            model->addFace(
                xPos + 1, yPos    , zPos + 1,
                xPos    , yPos    , zPos + 1,
                xPos    , yPos    , zPos,
                DynamicModel::XZ_NEG);
        }

        // Back
        if ((yPos == _grid->getHeight() - 1) || (_grid->getPaletteIndex(xPos, yPos + 1, zPos) == TilePalette::EmptyTile)) {
            model->addFace(
                xPos + 1, yPos + 1, zPos,
                xPos    , yPos + 1, zPos,
                xPos    , yPos + 1, zPos + 1,
                DynamicModel::XZ_POS);

            model->addFace(
                xPos    , yPos + 1, zPos + 1,
                xPos + 1, yPos + 1, zPos + 1,
                xPos + 1, yPos + 1, zPos,
                DynamicModel::XZ_POS);
        }

        // Bottom
        if (_grid->getPaletteIndex(xPos, yPos, zPos - 1) == TilePalette::EmptyTile) {
            model->addFace(
                xPos    , yPos + 1, zPos,
                xPos + 1, yPos + 1, zPos,
                xPos + 1, yPos    , zPos,
                DynamicModel::XY_NEG);

            model->addFace(
                xPos + 1, yPos    , zPos,
                xPos    , yPos    , zPos,
                xPos    , yPos + 1, zPos,
                DynamicModel::XY_NEG);
        }
    }

    // Top
    if ((zPos == _grid->getDepth() - 1) || (_grid->getPaletteIndex(xPos, yPos, zPos + 1) == TilePalette::EmptyTile)) {
        model->addFace(
            xPos    , yPos    , zPos + 1,
            xPos + 1, yPos    , zPos + 1,
            xPos + 1, yPos + 1, zPos + 1,
            DynamicModel::XY_POS);

        model->addFace(
            xPos + 1, yPos + 1, zPos + 1,
            xPos    , yPos + 1, zPos + 1,
            xPos    , yPos    , zPos + 1,
            DynamicModel::XY_POS);
    }
}
