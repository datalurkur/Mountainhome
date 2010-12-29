/*
 *  BlockChunkedTerrainModel.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "BlockChunkedTerrainModel.h"
#include "DynamicModel.h"

BlockChunkedTerrainModel::BlockChunkedTerrainModel(
    TileGrid *grid, PaletteIndex type,
    int xChunkIndex, int yChunkIndex, int zChunkIndex):
    ChunkedTerrainModel(grid, type, xChunkIndex, yChunkIndex, zChunkIndex) {
    // Manually set the chunk size for "special" chunks that occur at the edges and have odd sizes
    if(xChunkIndex * ChunkSize + ChunkSize > grid->getWidth()) {
        XChunkSize = grid->getWidth() - (xChunkIndex * ChunkSize);
    }
    else {
        XChunkSize = ChunkSize;
    }
    if(yChunkIndex * ChunkSize + ChunkSize > grid->getHeight()) {
        YChunkSize = grid->getHeight() - (yChunkIndex * ChunkSize);
    }
    else {
        YChunkSize = ChunkSize;
    }
    if(zChunkIndex * ChunkSize + ChunkSize > grid->getDepth()) {
        ZChunkSize = grid->getDepth() - (zChunkIndex * ChunkSize);
    }
    else {
        ZChunkSize = ChunkSize;
    }
}

int BlockChunkedTerrainModel::update(bool doPolyReduction) {
    // Clean up the old memory.
    clear();

    // Create the dynamic model with enough space to handle normals at the edges.
    DynamicModel *model = new DynamicModel(XChunkSize + 1, YChunkSize + 1, _xLoc, _yLoc, _zLoc);

    // Build the initial geometry for the chunk.
    for (int xPos = _xLoc; xPos < _xLoc + XChunkSize; xPos++) {
        for (int yPos = _yLoc; yPos < _yLoc + YChunkSize; yPos++) {
            for (int zPos = _zLoc; zPos < _zLoc + ZChunkSize; zPos++) {
                if (_grid->getPaletteIndex(xPos, yPos, zPos) == _type) {
                    addGeometry(xPos, yPos, zPos, model);
                }
            }
        }
    }

    if (doPolyReduction) {
        model->doPolyReduction();
    }

    // Convert the vert/texcoord vectors to flat arrays for rendering.
    if (_count = model->getVertexCount()) {

        // Setup all of the variables required for rendering.
        _texCoords = model->buildStaticTexCoordArray();
        _verts = model->buildStaticVertexArray();
        _indices = model->buildStaticIndexArray();
        _indexCount = model->getIndexCount();

        _norms = NULL;
        generateNormals();

        generateVBOs();
        findBounds();
    }

    // Clean up the dynamic model.
    delete model;

    // Increment the count appropriately.
    return _count;
}

void BlockChunkedTerrainModel::addGeometry(int xPos, int yPos, int zPos, DynamicModel *model) {
    // Only generate geometry for the sides/bottom if we're not on the lowest level.
    if (zPos > 0) {
        // Left
        if((xPos > 0) && (_grid->getPaletteIndex(xPos - 1, yPos, zPos) == TILE_EMPTY)) {
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
        if((xPos+1 < _grid->getWidth()) && (_grid->getPaletteIndex(xPos + 1, yPos, zPos) == TILE_EMPTY)) {
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
        if ((yPos > 0) && (_grid->getPaletteIndex(xPos, yPos - 1, zPos) == TILE_EMPTY)) {
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
        if ((yPos+1 < _grid->getHeight()) && (_grid->getPaletteIndex(xPos, yPos + 1, zPos) == TILE_EMPTY)) {
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
        if (_grid->getPaletteIndex(xPos, yPos, zPos - 1) == TILE_EMPTY) {
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
    if ((zPos+1 >= _grid->getDepth()) || (_grid->getPaletteIndex(xPos, yPos, zPos + 1) == TILE_EMPTY)) {
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
