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

void HashTileGrid::setPaletteIndex(int x, int y, int z, PaletteIndex type) {
    _tileHash[PACK(x, y, z)] = type;
}

PaletteIndex HashTileGrid::getPaletteIndex(int x, int y, int z) {
    return _tileHash[PACK(x, y, z)];
}

int HashTileGrid::getSurfaceLevel(int x, int y) {
    for (int z = _depth - 1; z >= 0; z--) {
        if (getPaletteIndex(x, y, z) > TilePalette::EmptyTile) {
            return z;
        }
    }
    return -1;
}

void HashTileGrid::save(IOTarget *target) {
    int size = _tileHash.size();
    target->write(&_width,  sizeof(int));
    target->write(&_height, sizeof(int));
    target->write(&_depth,  sizeof(int));
    target->write(&size,    sizeof(int));

    std::map<LookupType, PaletteIndex>::iterator itr;
    for (itr = _tileHash.begin(); itr != _tileHash.end(); itr++) {
        target->write(&(itr->first),  sizeof(LookupType));
        target->write(&(itr->second), sizeof(Tile));
    }
}

void HashTileGrid::load(IOTarget *target) {
    int size = _tileHash.size();
    target->read(&_width,  sizeof(int));
    target->read(&_height, sizeof(int));
    target->read(&_depth,  sizeof(int));
    target->read(&size,    sizeof(int));

    PaletteIndex value;
    LookupType key;
    for (int i = 0; i < size; i++) {
        target->read(&key,   sizeof(LookupType));
        target->read(&value, sizeof(PaletteIndex));
        _tileHash[key] = value;
    }
}

void HashTileGrid::clear() {
    _tileHash.clear();
}
