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
    int size = _width * _height * _depth;
    _tileMatrix = new PaletteIndex[size];
    for(int i=0; i<size; i++) {
        _tileMatrix[i] = TILE_EMPTY;
    }
}

MatrixTileGrid::~MatrixTileGrid() {}

PaletteIndex MatrixTileGrid::getPaletteIndex(int x, int y, int z) {
	return _tileMatrix[(z * _width * _height) + (y * _width) + x];
}

void MatrixTileGrid::setPaletteIndex(int x, int y, int z, PaletteIndex index) {
	_tileMatrix[(z * _width * _height) + (y * _width) + x] = index;
}

///\note XXXBMW: Could be made faster by making the matrix zyx ordered instead of xyz ordered...
int MatrixTileGrid::getSurfaceLevel(int x, int y) {
    for (int z = _depth - 1; z >= 0; z--) {
        if (getPaletteIndex(x, y, z) != TILE_EMPTY) {
            return z;
        }
    }
    return -1;
}

void MatrixTileGrid::save(IOTarget *target) {
    target->write(&_width,     sizeof(int));
    target->write(&_height,    sizeof(int));
    target->write(&_depth,     sizeof(int));
    target->write(_tileMatrix, sizeof(PaletteIndex) * _width * _height * _depth);
}

void MatrixTileGrid::load(IOTarget *target) {
    target->read(&_width,     sizeof(int));
    target->read(&_height,    sizeof(int));
    target->read(&_depth,     sizeof(int));
    target->read(_tileMatrix, sizeof(PaletteIndex) * _width * _height * _depth);
}

void MatrixTileGrid::clear() {
    memset(_tileMatrix, TILE_EMPTY, sizeof(PaletteIndex) * _width * _height * _depth);
}
