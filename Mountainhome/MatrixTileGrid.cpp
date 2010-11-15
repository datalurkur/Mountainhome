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
    _tileMatrix = new Tile[_width * _height * _depth];
}

MatrixTileGrid::~MatrixTileGrid() {}

Tile MatrixTileGrid::getTile(int x, int y, int z) {
	return _tileMatrix[(z * _width * _height) + (y * _width) + x];
}

TileType MatrixTileGrid::getTileType(int x, int y, int z) {
    return _tileMatrix[(z * _width * _height) + (y * _width) + x].Type;
}

bool MatrixTileGrid::getTileParameter(int x, int y, int z, TileParameter param) {
    return _tileMatrix[(z * _width * _height) + (y * _width) + x].getParameter(param);
}

void MatrixTileGrid::setTile(int x, int y, int z, Tile tile) {
	_tileMatrix[(z * _width * _height) + (y * _width) + x] = tile;
}

void MatrixTileGrid::setTileType(int x, int y, int z, TileType type) {
    _tileMatrix[(z * _width * _height) + (y * _width) + x].Type = type;
}

void MatrixTileGrid::setTileParameter(int x, int y, int z, TileParameter param, bool value) {
    _tileMatrix[(z * _width * _height) + (y * _width) + x].setParameter(param, value);
}

///\note XXXBMW: Could be made faster by making the matrix zyx ordered instead of xyz ordered...
int MatrixTileGrid::getSurfaceLevel(int x, int y) {
    for (int z = _depth - 1; z >= 0; z--) {
        if (getTileType(x, y, z) > 0) {
            return z;
        }
    }
    
    return OutOfBounds;
}

void MatrixTileGrid::save(IOTarget *target) {
    target->write(&_width,     sizeof(int));
    target->write(&_height,    sizeof(int));
    target->write(&_depth,     sizeof(int));
    target->write(_tileMatrix, sizeof(Tile) * _width * _height * _depth);
}

void MatrixTileGrid::load(IOTarget *target) {
    target->read(&_width,     sizeof(int));
    target->read(&_height,    sizeof(int));
    target->read(&_depth,     sizeof(int));
    target->read(_tileMatrix, sizeof(Tile) * _width * _height * _depth);
}

void MatrixTileGrid::clear() {
    memset(_tileMatrix, 0, sizeof(Tile) * _width * _height * _depth);
}
