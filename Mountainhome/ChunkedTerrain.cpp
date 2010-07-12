/*
 *  ChunkedTerrain.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/File.h>
#include <Base/FileSystem.h>
#include "ChunkedTerrain.h"
#include "MatrixTileGrid.h"
#include "TerrainChunk.h"

ChunkedTerrain::ChunkedTerrain(int width, int height, int depth,
OctreeSceneManager *scene, MaterialManager *manager)
: _sceneManager(scene), _materialManager(manager)
{
    _grid = new MatrixTileGrid(width, height, depth);

    _chunks.reserve(TILE_TYPE_COUNT);
    for (int i = 0; i < TILE_TYPE_COUNT; i++) {
        _chunks[i] = (i == 0 ? NULL : new TerrainChunk());
    }

    clear();
}

ChunkedTerrain::~ChunkedTerrain() {
    clear_list(_chunks);
}
    
TileType ChunkedTerrain::getTile(int x, int y, int z) {
    return _grid->getTile(x, y, z);
}

void ChunkedTerrain::setTile(int x, int y, int z, TileType newType) {
    TileType oldType = _grid->getTile(x, y, z);
    _chunks[oldType]->removeTile(x, y, z);
    _chunks[newType]->addTile(x, y, z);
    _grid->setTile(x, y, z, newType);
}

int ChunkedTerrain::getSurfaceLevel(int x, int y) {
    return _grid->getSurfaceLevel(x, y);
}

void ChunkedTerrain::clear() {
    for (int i = 0; i < TILE_TYPE_COUNT; i++) {
        if (_chunks[i]) { _chunks[i]->clear(); }
    }

    _grid->clear();
}
    
int ChunkedTerrain::getWidth() {
    return _grid->getDimensions().x;
}

int ChunkedTerrain::getHeight() {
    return _grid->getDimensions().y;
}

int ChunkedTerrain::getDepth() {
    return _grid->getDimensions().z;
}
    
void ChunkedTerrain::save(std::string filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Write);
    _grid->save(file);
    delete file;
}

void ChunkedTerrain::load(std::string filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Read);
    _grid->load(file);
    delete file;

    // And regenerate the geometry.
    for (int x = 0; x < getWidth(); x++) {
        for (int y = 0; y < getHeight(); y++) {
            for (int z = 0; z < getDepth(); z++) {
                _chunks[getTile(x, y, z)]->addTile(x, y, z);
            }
        }
    }
}

void ChunkedTerrain::populate(bool reduce) {

}
