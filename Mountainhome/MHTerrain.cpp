/*
 *  MHTerrain.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHTerrain.h"
#include "OctreeSceneManager.h"
#include "TilePalette.h"

MHTerrain::MHTerrain(
    int width,
    int height,
    int depth,
    OctreeSceneManager *sceneManager
):
    _width(width),
    _height(height),
    _depth(depth),
    _polyReduction(false),
    _autoUpdate(false),
    _sceneManager(sceneManager)
{
    _tilePalette = new TilePalette();
}

MHTerrain::~MHTerrain() {
    delete _tilePalette;
}

bool MHTerrain::isOutOfBounds(Vector3 pos) { return isOutOfBounds(pos[0], pos[1], pos[2]); }
bool MHTerrain::isOutOfBounds(int x, int y, int z) {
    return (x < 0 || x >= _width  ||
            y < 0 || y >= _height ||
            z < 0 || z >= _depth);
}

int MHTerrain::getWidth()  { return _width;  }
int MHTerrain::getHeight() { return _height; }
int MHTerrain::getDepth()  { return _depth;  }

void MHTerrain::setPolyReduction(bool val) { _polyReduction = val; }
void MHTerrain::setAutoUpdate   (bool val) { _autoUpdate    = val; }

TilePalette * MHTerrain::getPalette() { return _tilePalette; }

bool MHTerrain::getTileProperty(int x, int y, int z, TileProperty prop) {
    const PropertyType &rawProp = _tilePalette->getProperty(getPaletteIndex(x,y,z), prop);
    bool propValue = boost::any_cast<bool>(rawProp);
    return propValue;
}

char MHTerrain::getTileNumericProperty(int x, int y, int z, TileProperty prop) {
    const PropertyType &rawProp = _tilePalette->getProperty(getPaletteIndex(x,y,z), prop);
    char propValue = boost::any_cast<char>(rawProp);
    return propValue;
}

std::string MHTerrain::getTileTextProperty(int x, int y, int z, TileProperty prop) {
    const PropertyType &rawProp = _tilePalette->getProperty(getPaletteIndex(x,y,z), prop);
    std::string propValue = boost::any_cast<std::string>(rawProp);
    return propValue;
}

void MHTerrain::setTileProperty(int x, int y, int z, TileProperty prop, PropertyType value) {
    PaletteIndex oldIndex, newIndex;
    oldIndex = getPaletteIndex(x, y, z);
    newIndex = _tilePalette->setProperty(oldIndex, prop, value);
    if (oldIndex != newIndex) {
        setPaletteIndex(x, y, z, newIndex);
    }
}

bool MHTerrain::isTileEmpty(int x, int y, int z) {
    return (getPaletteIndex(x,y,z)==TILE_EMPTY);
}

void MHTerrain::setTileEmpty(int x, int y, int z) {
    setPaletteIndex(x,y,z,TILE_EMPTY);
}
