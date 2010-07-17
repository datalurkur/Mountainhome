/*
 *  ChunkedTerrainModel.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ChunkedTerrainModel.h"
#include "ChunkedTerrainGroup.h"

const float TileWidth     = 1.0;
const float TileHeight    = 1.0;
const float TileDepth     = 1.0;
const float TexCoordScale = 0.1;


ChunkedTerrainModel::ChunkedTerrainModel(TileGrid *grid, TileType type,
int xChunkIndex, int yChunkIndex, int zChunkIndex):
_grid(grid), _type(type),
_xLoc(xChunkIndex * ChunkSize),
_yLoc(yChunkIndex * ChunkSize),
_zLoc(zChunkIndex * ChunkSize)
{
    char buffer[32];
    snprintf(buffer, 32, "terrain_chunk_%i_%i_%i_%i", _type, xChunkIndex, yChunkIndex, zChunkIndex);
    _name = buffer;

    Vector3 loc  = Vector3(xChunkIndex, yChunkIndex, zChunkIndex);
    Vector3 size = Vector3(ChunkSize * TileWidth, ChunkSize * TileHeight, ChunkSize * TileDepth);
    _boundingBox.setMinMax(loc * size, (loc + 1) * size);
}
ChunkedTerrainModel::~ChunkedTerrainModel() {}

std::string ChunkedTerrainModel::getName() {
    return _name;
}

int ChunkedTerrainModel::update(bool doPolyReduction) {
    // Info("Updating chunk " << getName());

    // Clean up the old memory.
    clear();

    ///\todo XXXBMW: We could do something crazy like have a single translation matrix
    // shared by all chunked terrain models (wouldn't work with threading, but oh well).
    // This would save us the time it takes to new/delete the memory for every model.

    // Create the translation matrix we'll use to map coords to vertex array indices.
    int translationMatrixSize = (ChunkSize + 1) * (ChunkSize + 1) * (ChunkSize + 1);
    int *translationMatrix = new int[translationMatrixSize];

    // Here, -1 will mean "no associated vertex data.
    memset(translationMatrix, -1, sizeof(int) * translationMatrixSize);

    // Build the vertex array and the translation matrix
    std::vector<Vector3> vertsArray;
    std::vector<Vector2> texCoordsArray;
    std::vector<unsigned int> indexArray;
    for (int xPos = _xLoc; xPos < _xLoc + ChunkSize; xPos++) {
        for (int yPos = _yLoc; yPos < _yLoc + ChunkSize; yPos++) {
            TileType lastType = -1;
            for (int zPos = _zLoc; zPos < _zLoc + ChunkSize; zPos++) {
                // This should handle the zPos == 0 case correctly.
                if (lastType = -1) { lastType = _grid->getTile(xPos, yPos, zPos - 1); }

                TileType currentType = _grid->getTile(xPos, yPos, zPos);

                // Info("Examining " << xPos << ", " << yPos << ", " << zPos << " last: " << (int)lastType << " current: " << (int)currentType);

                // If we're transitioning into or out of nothing, we need a vertex.
                if ((lastType    == 0 && currentType == _type) ||
                    (currentType == 0 && lastType    == _type))
                {

//                if (currentType == _type) {

                    // Info("Found type change. Adding geometry for " << (int)_type << " at " << xPos << ", " << yPos << ", " << zPos);

                    #define LOOKUP(x, y, z) \
                        translationMatrix[ \
                        (((z) - _zLoc)*(ChunkSize+1)*(ChunkSize+1)) + \
                        (((y) - _yLoc)*(ChunkSize+1))   + \
                        (((x) - _xLoc))]

                    #define VERTEX(x, y, z) \
                    do { \
                        int index = LOOKUP(x, y, z); \
                        if (index == -1) { \
                            index = LOOKUP(x, y, z) = vertsArray.size(); \
                            vertsArray.push_back(Vector3((x) * TileWidth, (y) * TileHeight, (z) * TileDepth)); \
                            texCoordsArray.push_back(Vector2((x) * TexCoordScale, (y) * TexCoordScale)); \
                        } \
                        indexArray.push_back(index); \
                    } while(0)

                    int sw[3], se[3], nw[3], ne[3];
                    findVertexLocForTile(xPos,   yPos  , zPos, sw, currentType);
                    findVertexLocForTile(xPos+1, yPos  , zPos, se, currentType);
                    findVertexLocForTile(xPos,   yPos+1, zPos, nw, currentType);
                    findVertexLocForTile(xPos+1, yPos+1, zPos, ne, currentType);

                    // SW Triangle
                    VERTEX(sw[0], sw[1], sw[2]);
                    VERTEX(se[0], se[1], se[2]);
                    VERTEX(nw[0], nw[1], nw[2]);
                    // NE Triangle
                    VERTEX(nw[0], nw[1], nw[2]);
                    VERTEX(se[0], se[1], se[2]);
                    VERTEX(ne[0], ne[1], ne[2]);

                    #undef LOOKUP
                    #undef VERTEX
                }

                lastType = currentType;
            }
        }
    }

    // Convert the vert/texcoord vectors to flat arrays for rendering.
    _count = vertsArray.size();
    if (_count) {
        _verts = vector_to_array(vertsArray);
        _texCoords = vector_to_array(texCoordsArray);

        // Translate the index array using the translation matrix.
        int indexCount = indexArray.size();
        unsigned int *indices = vector_to_array(indexArray);

        // We no longer need the translation matrix.
        delete[] translationMatrix;

        // Calculate the normal for each vertex in the world by averaging the normal of all of
        // the faces a vertex is shared between. SPECIAL NOTE: In this particular case, we
        // know that all of the vertices will be attached to indices. In the generic case,
        // we don't, however, and should ensure we don't normalize bogus memory in the second
        // step of this process.
        _norms = new Vector3[_count];
        memset(_norms, 0, sizeof(Vector3) * _count);

        for (int i = 0; i < indexCount; i+=3) {
            Vector3 one = vertsArray[indexArray[i+1]] - vertsArray[indexArray[i+0]];
            Vector3 two = vertsArray[indexArray[i+2]] - vertsArray[indexArray[i+1]];
            Vector3 polyNormal = one.crossProduct(two);
            polyNormal.normalize();

            for (int j = 0; j < 3; j++) {
                _norms[indexArray[i+j]] += polyNormal;
            }
        }

        for (int i = 0; i < _count; i++) {
            _norms[i].normalize();
        }

        // And initialize the model (does the reduction).
        initialize(indices, indexCount, doPolyReduction);
    }

    // Increment the count appropriately.
    return _count;
}

void ChunkedTerrainModel::findVertexLocForTile(int x, int y, int z, int *result, TileType cardinalType) {
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

    if (_grid->getDepth() > 8) {
        THROW(InternalError, "This doesn't actually work yet! Sorry! It's because the "
            "below code is capable of generating positions outside of this chunk and the "
            "index translation matrix isn't capable of handling that. It shouldn't be "
            "hard to fix, but I've got to go camping, for now!!!");
    }

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
