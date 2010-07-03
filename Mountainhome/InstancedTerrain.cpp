/*
 *  InstancedTerrain.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "InstancedTerrain.h"

InstancedTerrain::InstancedTerrain(int width, int height, int depth):
_width(width), _height(height), _depth(depth) {
    _typeMatrix = new TileType[_width * _height * _depth];
    clear();
}

InstancedTerrain::~InstancedTerrain() {}
    
TileType InstancedTerrain::getTile(int x, int y, int z) {
    return _typeMatrix[(z * _width * _height) + (y * _width) + x];
}

void InstancedTerrain::setTile(int x, int y, int z, TileType type) {
    _typeMatrix[(z * _width * _height) + (y * _width) + x] = type;
}

int InstancedTerrain::getSurfaceLevel(int x, int y) {
    THROW(InternalError, "This shouldn't exist.");
}

void InstancedTerrain::clear() {
    memset(_typeMatrix, 0, sizeof(TileType) * _width * _height * _depth);
}
    
int InstancedTerrain::getWidth() {
    return _width;
}

int InstancedTerrain::getHeight() {
    return _height;
}

int InstancedTerrain::getDepth() {
    return _depth;
}
    
void InstancedTerrain::save(std::string filename) {
    THROW(NotImplementedError, "This functionality doesn't exist yet!");
}

void InstancedTerrain::load(std::string filename) {
    THROW(NotImplementedError, "This functionality doesn't exist yet!");
}

void InstancedTerrain::populate(OctreeSceneManager *scene, MaterialManager *mManager, bool reduce) {

}
