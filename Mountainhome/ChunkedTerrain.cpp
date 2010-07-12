/*
 *  ChunkedTerrain.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ChunkedTerrain.h"

#include <Base/File.h>
#include <Base/FileSystem.h>
#include <Render/MaterialManager.h>
#include <Render/Entity.h>

#include "OctreeSceneManager.h"
#include "ChunkedTerrain.h"
#include "MatrixTileGrid.h"
#include "ChunkedTerrainGroup.h"

ChunkedTerrain::ChunkedTerrain(int width, int height, int depth,
OctreeSceneManager *scene, MaterialManager *manager)
: MHTerrain(width, height, depth)
{
    _grid = new MatrixTileGrid(width, height, depth);

    _groups.reserve(TILE_TYPE_COUNT);
    for (int i = 0; i < TILE_TYPE_COUNT; i++) {
        _groups[i] = (i == 0 ? NULL : new ChunkedTerrainGroup(_grid, scene, manager));
    }

    clear();
}

ChunkedTerrain::~ChunkedTerrain() {
    clear_list(_groups);
}
    
TileType ChunkedTerrain::getTile(int x, int y, int z) {
    return _grid->getTile(x, y, z);
}

void ChunkedTerrain::setTile(int x, int y, int z, TileType newType) {
    TileType oldType = _grid->getTile(x, y, z);

    if (oldType != newType) {
        _grid->setTile(x, y, z, newType);
        _groups[oldType]->update(x, y, z);
        _groups[newType]->update(x, y, z);
    }
}

int ChunkedTerrain::getSurfaceLevel(int x, int y) {
    return _grid->getSurfaceLevel(x, y);
}

void ChunkedTerrain::clear() {
    for (int i = 0; i < TILE_TYPE_COUNT; i++) {
        if (_groups[i]) { _groups[i]->clear(); }
    }

    _grid->clear();
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

    for (int i = 0; i < TILE_TYPE_COUNT; i++) {
        if (_groups[i]) { _groups[i]->updateAll(); }
    }
}

void ChunkedTerrain::populate(bool reduce) {

}
