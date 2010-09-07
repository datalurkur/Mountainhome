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
    TileGrid *grid, TileType type,
    int xChunkIndex, int yChunkIndex, int zChunkIndex):
    ChunkedTerrainModel(grid, type, xChunkIndex, yChunkIndex, zChunkIndex)
{}

int BlockChunkedTerrainModel::update(bool doPolyReduction) {
    // Clean up the old memory.
    clear();

    // Create the dynamic model with enough space to handle normals at the edges.
    DynamicModel *model = new DynamicModel(ChunkSize + 1, ChunkSize + 1, _xLoc, _yLoc, _zLoc);

    // Build the initial geometry for the chunk.
    for (int xPos = _xLoc; xPos < _xLoc + ChunkSize; xPos++) {
        for (int yPos = _yLoc; yPos < _yLoc + ChunkSize; yPos++) {
            for (int zPos = _zLoc; zPos < _zLoc + ChunkSize; zPos++) {
                if (_grid->getTile(xPos, yPos, zPos) == _type) {
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
        _norms = NULL;
        _texCoords = model->buildStaticTexCoordArray();
        _verts = model->buildStaticVertexArray();
        _indices = model->buildStaticIndexArray();
        _indexCount = model->getIndexCount();

        findBounds();
        generateVBOs();
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
        if (_grid->getTile(xPos - 1, yPos, zPos) == 0) {
            model->addFace(
                xPos, yPos    , zPos - 1,
                xPos, yPos    , zPos    ,
                xPos, yPos + 1, zPos    ,
                DynamicModel::YZ);

            model->addFace(
                xPos, yPos + 1, zPos    ,
                xPos, yPos + 1, zPos - 1,
                xPos, yPos    , zPos - 1,
                DynamicModel::YZ);
        }

        // Right
        if (_grid->getTile(xPos + 1, yPos, zPos) == 0) {
            model->addFace(
                xPos + 1, yPos    , zPos - 1,
                xPos + 1, yPos + 1, zPos - 1,
                xPos + 1, yPos + 1, zPos    ,
                DynamicModel::YZ);

            model->addFace(
                xPos + 1, yPos + 1, zPos    ,
                xPos + 1, yPos    , zPos    ,
                xPos + 1, yPos    , zPos - 1,
                DynamicModel::YZ);
        }

        // Front
        if (_grid->getTile(xPos, yPos - 1, zPos) == 0) {
            model->addFace(
                xPos    , yPos    , zPos - 1,
                xPos + 1, yPos    , zPos - 1,
                xPos + 1, yPos    , zPos    ,
                DynamicModel::XZ);

            model->addFace(
                xPos + 1, yPos    , zPos    ,
                xPos    , yPos    , zPos    ,
                xPos    , yPos    , zPos - 1,
                DynamicModel::XZ);
        }

        // Back
        if (_grid->getTile(xPos, yPos + 1, zPos) == 0) {
            model->addFace(
                xPos + 1, yPos + 1, zPos - 1,
                xPos    , yPos + 1, zPos - 1,
                xPos    , yPos + 1, zPos    ,
                DynamicModel::XZ);

            model->addFace(
                xPos    , yPos + 1, zPos    ,
                xPos + 1, yPos + 1, zPos    ,
                xPos + 1, yPos + 1, zPos - 1,
                DynamicModel::XZ);
        }

        // Bottom
        if (_grid->getTile(xPos, yPos, zPos - 1) == 0) {
            model->addFace(
                xPos    , yPos + 1, zPos - 1,
                xPos + 1, yPos + 1, zPos - 1,
                xPos + 1, yPos    , zPos - 1,
                DynamicModel::XY);

            model->addFace(
                xPos + 1, yPos    , zPos - 1,
                xPos    , yPos    , zPos - 1,
                xPos    , yPos + 1, zPos - 1,
                DynamicModel::XY);
        }
    }

    // Top
    int above = _grid->getTile(xPos, yPos, zPos + 1);
    if (above == 0 || above == TileGrid::OutOfBounds) {
        model->addFace(
            xPos    , yPos    , zPos,
            xPos + 1, yPos    , zPos,
            xPos + 1, yPos + 1, zPos,
            DynamicModel::XY);

        model->addFace(
            xPos + 1, yPos + 1, zPos,
            xPos    , yPos + 1, zPos,
            xPos    , yPos    , zPos,
            DynamicModel::XY);
    }
}
