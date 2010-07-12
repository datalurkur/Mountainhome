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
    for (int i = 0; i < TILE_TYPE_COUNT; i++) {
        _groups[i] = (i == 0 ? NULL : new IncrementalTerrainModel());

        snprintf(buffer, 32, "terrain_chunk_%i", i);
        Entity *entity = _sceneManager->createEntity(_groups[i], buffer);
        entity->setMaterial(_materialManager->getCachedResource("grass"));
    }

    clear();
}

IncrementalTerrain::~IncrementalTerrain() {
    clear_list(_groups);
}
    
TileType IncrementalTerrain::getTile(int x, int y, int z) {
    return _grid->getTile(x, y, z);
}

void IncrementalTerrain::setTile(int x, int y, int z, TileType newType) {
    TileType oldType = _grid->getTile(x, y, z);
    _groups[oldType]->removeTile(x, y, z);
    _groups[newType]->addTile(x, y, z);
    _grid->setTile(x, y, z, newType);
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
    
void IncrementalTerrain::save(std::string filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Write);
    _grid->save(file);
    delete file;
}

void IncrementalTerrain::load(std::string filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Read);
    _grid->load(file);
    delete file;

    // And regenerate the geometry.
    for (int x = 0; x < getWidth(); x++) {
        for (int y = 0; y < getHeight(); y++) {
            for (int z = 0; z < getDepth(); z++) {
                _groups[getTile(x, y, z)]->addTile(x, y, z);
            }
        }
    }
}

void IncrementalTerrain::populate(bool reduce) {

}
