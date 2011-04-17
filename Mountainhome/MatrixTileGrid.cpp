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
        _tileMatrix[i] = TilePalette::EmptyTile;
    }
}

MatrixTileGrid::~MatrixTileGrid() {}

PaletteIndex MatrixTileGrid::getPaletteIndex(int x, int y, int z) {
    ASSERT(x >= 0 && x < _width);
    ASSERT(y >= 0 && y < _height);
    ASSERT(z >= 0 && z < _depth);
    return _tileMatrix[(z * _width * _height) + (y * _width) + x];
}

void MatrixTileGrid::setPaletteIndex(int x, int y, int z, PaletteIndex index) {
    ASSERT(x >= 0 && x < _width);
    ASSERT(y >= 0 && y < _height);
    ASSERT(z >= 0 && z < _depth);
    _tileMatrix[(z * _width * _height) + (y * _width) + x] = index;
}

///\note XXXBMW: Could be made faster by making the matrix zyx ordered instead of xyz ordered...
int MatrixTileGrid::getSurfaceLevel(int x, int y) {
    ASSERT(x >= 0 && x < _width);
    ASSERT(y >= 0 && y < _height);
    for (int z = _depth - 1; z >= 0; z--) {
        if (getPaletteIndex(x, y, z) != TilePalette::EmptyTile) {
            return z;
        }
    }
    return -1;
}

int MatrixTileGrid::getEmptyRanges(int x, int y, std::vector<std::pair<int,int> > &ranges) {
    ASSERT(x >= 0 && x < _width);
    ASSERT(y >= 0 && y < _height);

    int startZ = -1;

    for(int z=0; z < _depth; z++) {
        if(getPaletteIndex(x, y, z) == TilePalette::EmptyTile && startZ == -1) {
            // An empty range begins here
            startZ = z;
        }
        else if(getPaletteIndex(x, y, z) != TilePalette::EmptyTile && startZ != -1) {
            // An empty range ends here
            ranges.push_back(std::pair<int,int>(startZ, z-1));
            startZ = -1;
        }
    }
    if(startZ != -1) {
        // An empty range ends at the top of the world.
        ranges.push_back(std::pair<int,int>(startZ, _depth - 1));
    }

    return ranges.size();
}

int MatrixTileGrid::getFilledRanges(int x, int y, std::vector<std::pair<int,int> > &ranges) {
    ASSERT(x >= 0 && x < _width);
    ASSERT(y >= 0 && y < _height);

    int startZ = -1;

    for(int z=0; z < _depth; z++) {
        if(getPaletteIndex(x, y, z) != TilePalette::EmptyTile && startZ == -1) {
            // A filled range begins here
            startZ = z;
        }
        else if(getPaletteIndex(x, y, z) == TilePalette::EmptyTile && startZ != -1) {
            // A filled range ends here
            ranges.push_back(std::pair<int,int>(startZ, z-1));
            startZ = -1;
        }
    }
    if(startZ != -1) {
        // A filled range ends at the top of the world.
        ranges.push_back(std::pair<int,int>(startZ, _depth - 1));
    }

    return ranges.size();
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
    memset(_tileMatrix, TilePalette::EmptyTile, sizeof(PaletteIndex) * _width * _height * _depth);
}
