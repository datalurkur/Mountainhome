/*
 *  HashTileGrid.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/11/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/IOTarget.h>
#include "HashTileGrid.h"

#define PACK(x, y, z) ((x) << (BitsPerDim * 2)) | ((y) << BitsPerDim) | (z)

HashTileGrid::HashTileGrid(int width, int height, int depth)
: TileGrid(width, height, depth) {
    if ((width >> BitsPerDim) > 0 || (width >> BitsPerDim) > 0 || (width >> BitsPerDim) > 0) {
        THROW(InternalError, "HashTileGrid only supports dimensions up to " <<
            (1 << (BitsPerDim + 1)) << ".");
    }
}

HashTileGrid::~HashTileGrid() {}

void HashTileGrid::setTile(int x, int y, int z, TileType type) {
    if (isOutOfBounds(x, y, z)) {
        THROW(InternalError, "" << x << ", " << y << ", " << z << " is out of bounds.");
    }

    _typeHash[PACK(x, y, z)] = type;
}

TileType HashTileGrid::getTile(int x, int y, int z) {
    if (isOutOfBounds(x, y, z)) { return OutOfBounds; }
    return _typeHash[PACK(x, y, z)];
}

int HashTileGrid::getSurfaceLevel(int x, int y) {
    for (int z = _depth - 1; z >= 0; z--) {
        if (getTile(x, y, z) > 0) {
            return z;
        }
    }
    
    return OutOfBounds;
}

void HashTileGrid::save(IOTarget *target) {
    int size = _typeHash.size();
    target->write(&_width,  sizeof(int));
    target->write(&_height, sizeof(int));
    target->write(&_depth,  sizeof(int));
    target->write(&size,    sizeof(int));

    std::map<LookupType, TileType>::iterator itr;
    for (itr = _typeHash.begin(); itr != _typeHash.end(); itr++) {
        target->write(&(itr->first),  sizeof(LookupType));
        target->write(&(itr->second), sizeof(TileType));
    }
}

void HashTileGrid::load(IOTarget *target) {
    int size = _typeHash.size();
    target->read(&_width,  sizeof(int));
    target->read(&_height, sizeof(int));
    target->read(&_depth,  sizeof(int));
    target->read(&size,    sizeof(int));

    TileType value;
    LookupType key;
    for (int i = 0; i < size; i++) {
        target->read(&key,   sizeof(LookupType));
        target->read(&value, sizeof(TileType));
        _typeHash[key] = value;
    }
}

void HashTileGrid::clear() {
    _typeHash.clear();
}
