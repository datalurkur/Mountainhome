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

    // Push back NULL to handle air.
    _groups.push_back(NULL);

    clear();
}

IncrementalTerrain::~IncrementalTerrain() {
    clear_list(_groups);
    delete _grid;
}

TileType IncrementalTerrain::registerTileType(const std::string &materialName) {
    TileType newType = _tileTypeCount++;

    _groups.push_back(new IncrementalTerrainModel());

    ASSERT_EQ(_tileTypeCount, _groups.size());

    char buffer[32];
    snprintf(buffer, 32, "incremental_terrain_group_%i", newType);
    Entity *entity = _sceneManager->create<Entity>(buffer);

    Material *mat = _materialManager->getCachedResource(materialName);
    _groups[newType]->setDefaultMaterial(mat);
    entity->setModel(_groups[newType]);

    return newType;
}

    
TileType IncrementalTerrain::getTileType(int x, int y, int z) {
    return _grid->getTileType(x, y, z);
}

bool IncrementalTerrain::getTileParameter(int x, int y, int z, TileParameter param) {
    return _grid->getTileParameter(x, y, z, param);
}

void IncrementalTerrain::setTileType(int x, int y, int z, TileType newType) {
    TileType oldType = _grid->getTileType(x, y, z);
    _grid->setTileType(x, y, z, newType);

    if (_autoUpdate) {
        _groups[oldType]->removeTile(x, y, z);
        _groups[newType]->addTile(x, y, z);
    }
}

void IncrementalTerrain::setTileParameter(int x, int y, int z, TileParameter param, bool value) {
    bool oldValue = _grid->getTileParameter(x, y, z, param);

    if (value != oldValue) {
        _grid->setTileParameter(x, y, z, param, value);

        // TODO: Is this code valid?
        if (_autoUpdate) {
            TileType oldType = _grid->getTileType(x, y, z);
            _groups[oldType]->removeTile(x, y, z);
            _groups[oldType]->addTile(x, y, z);
        }
    }
}

int IncrementalTerrain::getSurfaceLevel(int x, int y) {
    return _grid->getSurfaceLevel(x, y);
}

void IncrementalTerrain::clear() {
    for (int i = 0; i < getTileTypeCount(); i++) {
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
                _groups[getTileType(x, y, z)]->addTile(x, y, z);
            }
        }
    }
    _autoUpdate = oldVal;
}
