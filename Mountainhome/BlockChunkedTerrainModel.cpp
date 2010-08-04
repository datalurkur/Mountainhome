/*
 *  BlockChunkedTerrainModel.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "BlockChunkedTerrainModel.h"

BlockChunkedTerrainModel::BlockChunkedTerrainModel(
    TileGrid *grid, TileType type,
    int xChunkIndex, int yChunkIndex, int zChunkIndex):
    ChunkedTerrainModel(grid, type, xChunkIndex, yChunkIndex, zChunkIndex)
{}

int BlockChunkedTerrainModel::update(bool doPolyReduction) {
    // Clean up the old memory.
    clear();

    // Create the dynamic model with enough space to handle normals at the edges.
    DynamicModel *model = new DynamicModel(ChunkSize + 3, ChunkSize + 3, _xLoc, _yLoc, _zLoc);

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

    // Convert the vert/texcoord vectors to flat arrays for rendering.
    if (_count = model->getVertexCount()) {
        // Setup all of the variables required for rendering.
        _norms = NULL;
        _verts = model->buildStaticVertexArray();
        _texCoords = model->buildStaticTexCoordArray();
        initialize(model->buildStaticIndexArray(), model->getIndexCount(), doPolyReduction);
        findBounds();
    }

    // Clean up the dynamic model.
    delete model;

    // Increment the count appropriately.
    return _count;
}

void BlockChunkedTerrainModel::addGeometry(int xPos, int yPos, int zPos, DynamicModel *model) {
    if (zPos > 0) {
        // Left
        if (_grid->getTile(xPos - 1, yPos, zPos) == 0) {
            model->addVertex(xPos, yPos    , zPos - 1);
            model->addVertex(xPos, yPos    , zPos    );
            model->addVertex(xPos, yPos + 1, zPos    );

            model->addVertex(xPos, yPos + 1, zPos    );
            model->addVertex(xPos, yPos + 1, zPos - 1);
            model->addVertex(xPos, yPos    , zPos - 1);
        }

        // Right
        if (_grid->getTile(xPos + 1, yPos, zPos) == 0) {
            model->addVertex(xPos + 1, yPos    , zPos - 1);
            model->addVertex(xPos + 1, yPos + 1, zPos - 1);
            model->addVertex(xPos + 1, yPos + 1, zPos    );

            model->addVertex(xPos + 1, yPos + 1, zPos    );
            model->addVertex(xPos + 1, yPos    , zPos    );
            model->addVertex(xPos + 1, yPos    , zPos - 1);
        }

        // Front
        if (_grid->getTile(xPos, yPos - 1, zPos) == 0) {
            model->addVertex(xPos    , yPos    , zPos - 1);
            model->addVertex(xPos + 1, yPos    , zPos - 1);
            model->addVertex(xPos + 1, yPos    , zPos    );

            model->addVertex(xPos + 1, yPos    , zPos    );
            model->addVertex(xPos    , yPos    , zPos    );
            model->addVertex(xPos    , yPos    , zPos - 1);
        }

        // Back
        if (_grid->getTile(xPos, yPos + 1, zPos) == 0) {
            model->addVertex(xPos + 1, yPos + 1, zPos - 1);
            model->addVertex(xPos    , yPos + 1, zPos - 1);
            model->addVertex(xPos    , yPos + 1, zPos    );

            model->addVertex(xPos    , yPos + 1, zPos    );
            model->addVertex(xPos + 1, yPos + 1, zPos    );
            model->addVertex(xPos + 1, yPos + 1, zPos - 1);
        }

        // Bottom
        if (_grid->getTile(xPos, yPos, zPos - 1) == 0) {
            model->addVertex(xPos    , yPos + 1, zPos - 1);
            model->addVertex(xPos + 1, yPos + 1, zPos - 1);
            model->addVertex(xPos + 1, yPos    , zPos - 1);

            model->addVertex(xPos + 1, yPos    , zPos - 1);
            model->addVertex(xPos    , yPos    , zPos - 1);
            model->addVertex(xPos    , yPos + 1, zPos - 1);
        }
    }

    // Top
    if (_grid->getTile(xPos, yPos, zPos + 1) == 0) {
        model->addVertex(xPos    , yPos    , zPos);
        model->addVertex(xPos + 1, yPos    , zPos);
        model->addVertex(xPos + 1, yPos + 1, zPos);

        model->addVertex(xPos + 1, yPos + 1, zPos);
        model->addVertex(xPos    , yPos + 1, zPos);
        model->addVertex(xPos    , yPos    , zPos);
    }
}
