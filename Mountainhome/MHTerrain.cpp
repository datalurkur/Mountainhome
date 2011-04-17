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

void MHTerrain::setTile(int x, int y, int z, const Tile &tile) {
    setTileIndex(x, y, z, _tilePalette->getPaletteIndex(tile));
}

void MHTerrain::setTileIndex(int x, int y, int z, PaletteIndex index) {
    setPaletteIndex(x, y, z, index);
}

void MHTerrain::setTileParameter(int x, int y, int z, ParameterID id, const ParameterData &value) {
    Tile newTile = getTile(x, y, z);
    newTile.setParameter(id, value);
    setTile(x, y, z, newTile);
}

const ParameterData &MHTerrain::getTileParameter(int x, int y, int z, ParameterID id) {
    const Tile &ref = getTile(x, y, z);
    return ref.getParameter(id);
}

const Tile &MHTerrain::getTile(int x, int y, int z) {
    PaletteIndex index = getPaletteIndex(x, y, z);
    return _tilePalette->getTileForIndex(index);
}

bool MHTerrain::isTileEmpty(int x, int y, int z) {
    return (getPaletteIndex(x, y, z) == TILE_EMPTY);
}

void MHTerrain::setTileEmpty(int x, int y, int z) {
    setPaletteIndex(x, y, z, TILE_EMPTY);
}
