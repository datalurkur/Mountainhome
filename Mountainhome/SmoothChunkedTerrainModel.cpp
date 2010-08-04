/*
 *  SmoothChunkedTerrainModel.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "SmoothChunkedTerrainModel.h"

SmoothChunkedTerrainModel::SmoothChunkedTerrainModel(
    TileGrid *grid, TileType type,
    int xChunkIndex, int yChunkIndex, int zChunkIndex):
    ChunkedTerrainModel(grid, type, xChunkIndex, yChunkIndex, zChunkIndex)
{}

int SmoothChunkedTerrainModel::update(bool doPolyReduction) {
    // Clean up the old memory.
    clear();

    // Create the dynamic model with enough space to handle normals at the edges.
    DynamicModel *model = new DynamicModel(ChunkSize + 3, ChunkSize + 3, _xLoc, _yLoc, _zLoc);

    // Build the initial geometry for the chunk.
    for (int xPos = _xLoc; xPos < _xLoc + ChunkSize; xPos++) {
        for (int yPos = _yLoc; yPos < _yLoc + ChunkSize; yPos++) {
            buildGeometry(xPos, yPos, model);
        }
    }

    // Convert the vert/texcoord vectors to flat arrays for rendering.
    if (_count = model->getVertexCount()) {
        // Do this now to work with both smooth shading on and off.
        _indexCount = model->getIndexCount();

        // Now we need to generate some extra geometry to get the normals correct. This
        // is technically wasted space and extra work, but it's the only way I can think
        // of to get smooth shading, correct normals, AND chunked terrain.

        // Use these when iterating across the X axis. This way we cover the corners, too!
        int startX = _xLoc;
        int endX   = _xLoc + ChunkSize - 1;

        if (_xLoc - 1 >= 0) {
            startX = _xLoc - 1;
            for (int yPos = _yLoc; yPos < _yLoc + ChunkSize; yPos++) {
                buildGeometry(startX, yPos, model);
            }
        }

        if (_xLoc + ChunkSize < _grid->getWidth() - 1) {
            endX = _xLoc + ChunkSize;
            for (int yPos = _yLoc; yPos < _yLoc + ChunkSize; yPos++) {
                buildGeometry(_xLoc + ChunkSize, yPos, model);
            }
        }

        if (_yLoc - 1 >= 0) {
            for (int xPos = startX; xPos <= endX; xPos++) {
                buildGeometry(xPos, _yLoc - 1, model);
            }
        }

        if (_yLoc + ChunkSize < _grid->getHeight() - 1) {
            for (int xPos = startX; xPos <= endX; xPos++) {
                buildGeometry(xPos, _yLoc + ChunkSize, model);
            }
        }

        // Setup all of the variables required for rendering.
        _verts = model->buildStaticVertexArray();
        _texCoords = model->buildStaticTexCoordArray();
        _norms = model->buildStaticNormalArray();
        _indices = model->buildStaticIndexArray();

        findBounds();
        if (doPolyReduction) {
            this->doPolyReduction();
        }
    }

    // Clean up the dynamic model.
    delete model;

    // Increment the count appropriately.
    return _count;
}

void SmoothChunkedTerrainModel::buildGeometry(int xPos, int yPos, DynamicModel *model)
{
    TileType lastType = -1;
    for (int zPos = _zLoc; zPos < _zLoc + ChunkSize; zPos++) {
        // This should handle the zPos == 0 case correctly.
        if (lastType = -1) { lastType = _grid->getTile(xPos, yPos, zPos - 1); }

        TileType currentType = _grid->getTile(xPos, yPos, zPos);

        // If we're transitioning into or out of nothing, we need a vertex.
        if ((lastType    == 0 && currentType == _type) ||
            (currentType == 0 && lastType    == _type))
        {
            int sw[3], se[3], nw[3], ne[3];
            findVertexLocForTile(xPos,   yPos  , zPos, sw, currentType);
            findVertexLocForTile(xPos+1, yPos  , zPos, se, currentType);
            findVertexLocForTile(xPos,   yPos+1, zPos, nw, currentType);
            findVertexLocForTile(xPos+1, yPos+1, zPos, ne, currentType);

            // SW Triangle
            model->addVertex(sw[0], sw[1], sw[2]);
            model->addVertex(se[0], se[1], se[2]);
            model->addVertex(nw[0], nw[1], nw[2]);
            // NE Triangle
            model->addVertex(nw[0], nw[1], nw[2]);
            model->addVertex(se[0], se[1], se[2]);
            model->addVertex(ne[0], ne[1], ne[2]);
        }

        lastType = currentType;
    }
}

void SmoothChunkedTerrainModel::findVertexLocForTile(int x, int y, int z, int *result, TileType cardinalType) {
    Math::Clamp(0, _grid->getWidth()  - 1, x);
    Math::Clamp(0, _grid->getHeight() - 1, y);
    Math::Clamp(0, _grid->getDepth()  - 1, z);

#if 0
    result[0] = x;
    result[1] = y;
    result[2] = z;
#else
    result[0] = x;
    result[1] = y;

    TileType lastType = _grid->getTile(x, y, z);
    int direction = lastType == cardinalType ? -1 : 1;
    for (int i = z + direction;; i += direction) {
        TileType currentType = _grid->getTile(x, y, i);
        if (((lastType == 0 && currentType != 0) || (lastType != 0 && currentType == 0)) ||
            (i + direction < 0 || i + direction >= _grid->getDepth()))
        {
            result[2] = direction == 1 ? i - 1 : i;
            break;
        }

        lastType = currentType;
    }
#endif
}
