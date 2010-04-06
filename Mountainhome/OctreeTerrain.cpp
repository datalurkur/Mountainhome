#include "OctreeTerrain.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark OctreeTerrain definitions
//////////////////////////////////////////////////////////////////////////////////////////
OctreeTerrain::OctreeTerrain(int width, int height, int depth) {
    _rootGroup = new TileGroup<short>(Vector3(0, 0, 0), Vector3(width, height, depth), 0, 0);
}

OctreeTerrain::~OctreeTerrain() {}

short OctreeTerrain::getTile(int x, int y, int z) {
    return _rootGroup->getTile(Vector3(x, y, z));
}

void OctreeTerrain::setTile(int x, int y, int z, short type) {
    _rootGroup->setTile(Vector3(x, y, z), type);
}

int OctreeTerrain::getSurfaceLevel(int x, int y) {
    int nX = x,
        nY = y;
    Vector3 dims = _rootGroup->getDims();
    
    if(nX < 0) { nX = 0; }
    else if(nX >= dims[0]) { nX = dims[0]-1; }
    if(nY < 0) { nY = 0; }
    else if(nY >= dims[1]) { nY = dims[1]-1; }
    
    return _rootGroup->getSurfaceLevel(Vector2(nX, nY));
}

int OctreeTerrain::getHeight() {
    return _rootGroup->getDims()[0];
}

int OctreeTerrain::getWidth() {
    return _rootGroup->getDims()[1];
}

int OctreeTerrain::getDepth() {
    return _rootGroup->getDims()[2];
}