/*
 *  HashVoxelGrid.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/11/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/IOTarget.h>
#include "HashVoxelGrid.h"

#define PACK(x, y, z) ((x) << (BitsPerDim * 2)) | ((y) << BitsPerDim) | (z)

HashVoxelGrid::HashVoxelGrid(int width, int height, int depth)
: VoxelGrid(width, height, depth) {
    if ((width >> BitsPerDim) > 0 || (width >> BitsPerDim) > 0 || (width >> BitsPerDim) > 0) {
        THROW(InternalError, "HashVoxelGrid only supports dimensions up to " <<
            (1 << (BitsPerDim + 1)) << ".");
    }
}

HashVoxelGrid::~HashVoxelGrid() {}

void HashVoxelGrid::setPaletteIndex(int x, int y, int z, PaletteIndex type) {
    _voxelHash[PACK(x, y, z)] = type;
}

PaletteIndex HashVoxelGrid::getPaletteIndex(int x, int y, int z) {
    return _voxelHash[PACK(x, y, z)];
}

int HashVoxelGrid::getSurfaceLevel(int x, int y) {
    for (int z = _depth - 1; z >= 0; z--) {
        if (getPaletteIndex(x, y, z) > VoxelPalette::EmptyVoxel) {
            return z;
        }
    }
    return -1;
}

void HashVoxelGrid::save(IOTarget *target) {
    int size = _voxelHash.size();
    target->write(&_width,  sizeof(int));
    target->write(&_height, sizeof(int));
    target->write(&_depth,  sizeof(int));
    target->write(&size,    sizeof(int));

    std::map<LookupType, PaletteIndex>::iterator itr;
    for (itr = _voxelHash.begin(); itr != _voxelHash.end(); itr++) {
        target->write(&(itr->first),  sizeof(LookupType));
        target->write(&(itr->second), sizeof(Voxel));
    }
}

void HashVoxelGrid::load(IOTarget *target) {
    int size = _voxelHash.size();
    target->read(&_width,  sizeof(int));
    target->read(&_height, sizeof(int));
    target->read(&_depth,  sizeof(int));
    target->read(&size,    sizeof(int));

    PaletteIndex value;
    LookupType key;
    for (int i = 0; i < size; i++) {
        target->read(&key,   sizeof(LookupType));
        target->read(&value, sizeof(PaletteIndex));
        _voxelHash[key] = value;
    }
}

void HashVoxelGrid::clear() {
    _voxelHash.clear();
}
