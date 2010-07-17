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
}
ChunkedTerrainModel::~ChunkedTerrainModel() {}

std::string ChunkedTerrainModel::getName() {
    return _name;
}

class TranslationMatrix {
public:
    TranslationMatrix(int width, int height): _width(width) {
        _matrix = new ZMap[width * height];
    }

    ~TranslationMatrix() {
        delete[] _matrix;
    }

    void setIndex(int x, int y, int z, int index) {
        _matrix[y * _width + x][z] = index;
    }

    int getIndex(int x, int y, int z) {
        ZMap::iterator itr = _matrix[y * _width + x].find(z);
        return itr == _matrix[y * _width + x].end() ? -1 : itr->second;
    }

private:
    typedef std::map<int, int> ZMap;
    ZMap *_matrix;
    int _width;
};

int ChunkedTerrainModel::update(bool doPolyReduction) {
    // Info("Updating chunk " << getName());

    // Clean up the old memory.
    clear();

    // Create the translation matrix we'll use to map coords to vertex array indices.
    TranslationMatrix *matrix = new TranslationMatrix(ChunkSize + 1, ChunkSize + 1);

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

                    #define VERTEX(x, y, z) \
                    do { \
                        int index = matrix->getIndex((x) - _xLoc, (y) - _yLoc, (z) - _zLoc); \
                        if (index == -1) { \
                            index = vertsArray.size(); \
                            matrix->setIndex((x) - _xLoc, (y) - _yLoc, (z) - _zLoc, index); \
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

        int indexCount = indexArray.size();
        unsigned int *indices = vector_to_array(indexArray);

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
        findBounds();
    }

    // Clean up the matrix.
    delete matrix;

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
