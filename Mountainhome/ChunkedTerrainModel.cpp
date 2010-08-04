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
#include "MHTerrain.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark TranslationMatrix definitions
//////////////////////////////////////////////////////////////////////////////////////////
TranslationMatrix::TranslationMatrix(int width, int height): _width(width), _height(height) {
    _matrix = new ZMap[width * height];
}

TranslationMatrix::~TranslationMatrix() {
    delete[] _matrix;
}

void TranslationMatrix::setIndex(int x, int y, int z, int index) {
    if (x < 0) { x = _width  - 1 ; }
    if (y < 0) { y = _height - 1 ; }
    ASSERT_LE(x, _width  - 1);
    ASSERT_LE(y, _height - 1);

    _matrix[y * _width + x][z] = index;
}

int TranslationMatrix::getIndex(int x, int y, int z) {
    if (x < 0) { x = _width  - 1 ; }
    if (y < 0) { y = _height - 1 ; }
    ASSERT_LE(x, _width  - 1);
    ASSERT_LE(y, _height - 1);

    ZMap::iterator itr = _matrix[y * _width + x].find(z);
    return itr == _matrix[y * _width + x].end() ? -1 : itr->second;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark DynamicModel definitions
//////////////////////////////////////////////////////////////////////////////////////////
DynamicModel::DynamicModel(int width, int height, int xOffset, int yOffset, int zOffset):
_xOffset(xOffset), _yOffset(yOffset), _zOffset(zOffset)
{
    _matrix = new TranslationMatrix(width, height);
}

DynamicModel::~DynamicModel() {
    delete _matrix;
}

void DynamicModel::addVertex(Real x, Real y, Real z) {
    int index = _matrix->getIndex(x - _xOffset, y - _yOffset, z - _zOffset);

    if (index == -1) {
        index = _vertsArray.size();
        _matrix->setIndex(x - _xOffset, y - _yOffset, z - _zOffset, index);

        // Offset x and y by half a tile, so the center of the top of the tile is at the
        // specified position.
        _vertsArray.push_back(Vector3(x - 0.5, y - 0.5, z));

        _texCoordsArray.push_back(Vector2(x, y));
    }

    _indexArray.push_back(index);
}

int DynamicModel::getVertexCount() {
    return _vertsArray.size();
}

int DynamicModel::getIndexCount() {
    return _indexArray.size();
}

Vector3 *DynamicModel::buildStaticVertexArray()   { return vector_to_array(_vertsArray); }
Vector2 *DynamicModel::buildStaticTexCoordArray() { return vector_to_array(_texCoordsArray); }
unsigned int *DynamicModel::buildStaticIndexArray()   { return vector_to_array(_indexArray); }
Vector3 *DynamicModel::buildStaticNormalArray() {
    Vector3 *norms = new Vector3[_vertsArray.size()];
    memset(norms, 0, sizeof(Vector3) * _vertsArray.size());
    for (int i = 0; i < _indexArray.size(); i+=3) {
        Vector3 one = _vertsArray[_indexArray[i+1]] - _vertsArray[_indexArray[i+0]];
        Vector3 two = _vertsArray[_indexArray[i+2]] - _vertsArray[_indexArray[i+1]];
        Vector3 polyNormal = one.crossProduct(two);
        polyNormal.normalize();

        for (int j = 0; j < 3; j++) {
            norms[_indexArray[i+j]] += polyNormal;
        }
    }

    for (int i = 0; i < _vertsArray.size(); i++) {
        norms[i].normalize();
    }

    return norms;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark ChunkedTerrainModel definitions
//////////////////////////////////////////////////////////////////////////////////////////
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


