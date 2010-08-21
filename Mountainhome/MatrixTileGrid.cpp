/*
 *  MatrixTileGrid.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/9/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/IOTarget.h>
#include "MatrixTileGrid.h"

MatrixTileGrid::MatrixTileGrid(int width, int height, int depth)
: TileGrid(width, height, depth) {
    _typeMatrix = new TileType[_width * _height * _depth];
}

MatrixTileGrid::~MatrixTileGrid() {}

void MatrixTileGrid::setTile(int x, int y, int z, TileType type) {
    ///\todo XXXBMW: This bounds checking is a BIG speed hit in world gen. Kill it?
    if (isOutOfBounds(x, y, z)) {
        THROW(InternalError, "" << x << ", " << y << ", " << z << " is out of bounds.");
    }

    _typeMatrix[(z * _width * _height) + (y * _width) + x] = type;
}

TileType MatrixTileGrid::getTile(int x, int y, int z) {
    ///\todo XXXBMW: This bounds checking is a BIG speed hit in world gen. Kill it?
    if (isOutOfBounds(x, y, z)) { return OutOfBounds; }
    return _typeMatrix[(z * _width * _height) + (y * _width) + x];
}

///\note XXXBMW: Could be made faster by making the matrix zyx ordered instead of xyz ordered...
int MatrixTileGrid::getSurfaceLevel(int x, int y) {
    for (int z = _depth - 1; z >= 0; z--) {
        if (getTile(x, y, z) > 0) {
            return z;
        }
    }
    
    return OutOfBounds;
}

void MatrixTileGrid::save(IOTarget *target) {
    target->write(&_width,     sizeof(int));
    target->write(&_height,    sizeof(int));
    target->write(&_depth,     sizeof(int));
    target->write(_typeMatrix, sizeof(TileType) * _width * _height * _depth);
}

void MatrixTileGrid::load(IOTarget *target) {
    target->read(&_width,     sizeof(int));
    target->read(&_height,    sizeof(int));
    target->read(&_depth,     sizeof(int));
    target->read(_typeMatrix, sizeof(TileType) * _width * _height * _depth);
}

void MatrixTileGrid::clear() {
    memset(_typeMatrix, 0, sizeof(TileType) * _width * _height * _depth);
}
