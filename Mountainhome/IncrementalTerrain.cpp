/*
 *  IncrementalTerrain.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/File.h>
#include <Base/FileSystem.h>
#include <Render/MaterialManager.h>
#include <Render/Entity.h>

#include "OctreeSceneManager.h"
#include "IncrementalTerrain.h"
#include "MatrixTileGrid.h"
#include "IncrementalTerrainModel.h"

IncrementalTerrain::IncrementalTerrain(int width, int height, int depth,
OctreeSceneManager *scene, MaterialManager *manager)
: MHTerrain(width, height, depth), _sceneManager(scene), _materialManager(manager)
{
    _grid = new MatrixTileGrid(width, height, depth);

    char buffer[32];
    _groups.reserve(TILE_TYPE_COUNT);
    _groups[0] = NULL;

    for (int i = 1; i < TILE_TYPE_COUNT; i++) {
        _groups[i] = new IncrementalTerrainModel();
        snprintf(buffer, 32, "incremental_terrain_group_%i", i);
        Entity *entity = _sceneManager->create<Entity>(buffer);

        _groups[i]->setDefaultMaterial(_materialManager->getCachedResource("grass"));
        entity->setModel(_groups[i]);
    }

    clear();
}

IncrementalTerrain::~IncrementalTerrain() {
    clear_list(_groups);
    delete _grid;
}
    
TileType IncrementalTerrain::getTile(int x, int y, int z) {
    return _grid->getTile(x, y, z);
}

void IncrementalTerrain::setTile(int x, int y, int z, TileType newType) {
    TileType oldType = _grid->getTile(x, y, z);
    _grid->setTile(x, y, z, newType);

    if (_autoUpdate) {
        _groups[oldType]->removeTile(x, y, z);
        _groups[newType]->addTile(x, y, z);
    }
}

int IncrementalTerrain::getSurfaceLevel(int x, int y) {
    return _grid->getSurfaceLevel(x, y);
}

void IncrementalTerrain::clear() {
    for (int i = 0; i < TILE_TYPE_COUNT; i++) {
        if (_groups[i]) { _groups[i]->clear(); }
    }

    _grid->clear();
}
    
void IncrementalTerrain::save(const std::string &filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Write);
    _grid->save(file);
    delete file;
}

void IncrementalTerrain::load(const std::string &filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Read);
    _grid->load(file);
    delete file;

    populate();
}

void IncrementalTerrain::populate() {
    bool oldVal = _autoUpdate;
    _autoUpdate = true;
    for (int x = 0; x < getWidth(); x++) {
        for (int y = 0; y < getHeight(); y++) {
            for (int z = 0; z < getDepth(); z++) {
                _groups[getTile(x, y, z)]->addTile(x, y, z);
            }
        }
    }
    _autoUpdate = oldVal;
}
