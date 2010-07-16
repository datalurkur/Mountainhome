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

                    // SW Triangle
                    VERTEX(xPos,   yPos  , zPos);
                    VERTEX(xPos+1, yPos  , zPos);
                    VERTEX(xPos,   yPos+1, zPos);
                    // NE Triangle
                    VERTEX(xPos,   yPos+1, zPos);
                    VERTEX(xPos+1, yPos  , zPos);
                    VERTEX(xPos+1, yPos+1, zPos);

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
