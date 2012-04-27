/*
 *  MatrixVoxelGrid.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/9/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/IOTarget.h>
#include "MatrixVoxelGrid.h"

MatrixVoxelGrid::MatrixVoxelGrid(int width, int height, int depth)
: VoxelGrid(width, height, depth) {
    int size = _width * _height * _depth;
    _voxelMatrix = new PaletteIndex[size];
    for(int i=0; i<size; i++) {
        _voxelMatrix[i] = VoxelPalette::EmptyVoxel;
    }
}

MatrixVoxelGrid::~MatrixVoxelGrid() {}

PaletteIndex MatrixVoxelGrid::getPaletteIndex(int x, int y, int z) {
    ASSERT(x >= 0 && x < _width);
    ASSERT(y >= 0 && y < _height);
    ASSERT(z >= 0 && z < _depth);
    return _voxelMatrix[(z * _width * _height) + (y * _width) + x];
}

void MatrixVoxelGrid::setPaletteIndex(int x, int y, int z, PaletteIndex index) {
    ASSERT(x >= 0 && x < _width);
    ASSERT(y >= 0 && y < _height);
    ASSERT(z >= 0 && z < _depth);
    _voxelMatrix[(z * _width * _height) + (y * _width) + x] = index;
}

///\note XXXBMW: Could be made faster by making the matrix zyx ordered instead of xyz ordered...
int MatrixVoxelGrid::getSurfaceLevel(int x, int y) {
    ASSERT(x >= 0 && x < _width);
    ASSERT(y >= 0 && y < _height);
    for (int z = _depth - 1; z >= 0; z--) {
        if (getPaletteIndex(x, y, z) != VoxelPalette::EmptyVoxel) {
            return z;
        }
    }
    return -1;
}

int MatrixVoxelGrid::getEmptyRanges(int x, int y, std::vector<std::pair<int,int> > &ranges) {
    ASSERT(x >= 0 && x < _width);
    ASSERT(y >= 0 && y < _height);

    int startZ = -1;

    for(int z=0; z < _depth; z++) {
        if(getPaletteIndex(x, y, z) == VoxelPalette::EmptyVoxel && startZ == -1) {
            // An empty range begins here
            startZ = z;
        }
        else if(getPaletteIndex(x, y, z) != VoxelPalette::EmptyVoxel && startZ != -1) {
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

int MatrixVoxelGrid::getFilledRanges(int x, int y, std::vector<std::pair<int,int> > &ranges) {
    ASSERT(x >= 0 && x < _width);
    ASSERT(y >= 0 && y < _height);

    int startZ = -1;

    for(int z=0; z < _depth; z++) {
        if(getPaletteIndex(x, y, z) != VoxelPalette::EmptyVoxel && startZ == -1) {
            // A filled range begins here
            startZ = z;
        }
        else if(getPaletteIndex(x, y, z) == VoxelPalette::EmptyVoxel && startZ != -1) {
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

void MatrixVoxelGrid::save(IOTarget *target) {
    target->write(&_width,     sizeof(int));
    target->write(&_height,    sizeof(int));
    target->write(&_depth,     sizeof(int));
    target->write(_voxelMatrix, sizeof(PaletteIndex) * _width * _height * _depth);
}

void MatrixVoxelGrid::load(IOTarget *target) {
    target->read(&_width,     sizeof(int));
    target->read(&_height,    sizeof(int));
    target->read(&_depth,     sizeof(int));
    target->read(_voxelMatrix, sizeof(PaletteIndex) * _width * _height * _depth);
}

void MatrixVoxelGrid::clear() {
    memset(_voxelMatrix, VoxelPalette::EmptyVoxel, sizeof(PaletteIndex) * _width * _height * _depth);
}
