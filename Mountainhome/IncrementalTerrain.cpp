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
#include <Engine/Entity.h>

#include "OctreeSceneManager.h"
#include "IncrementalTerrain.h"
#include "MatrixTileGrid.h"
#include "IncrementalTerrainModel.h"

IncrementalTerrain::IncrementalTerrain(int width, int height, int depth,
OctreeSceneManager *scene, MaterialManager *manager)
: MHTerrain(width, height, depth, scene, manager)
{
    _grid = new MatrixTileGrid(width, height, depth);

    clear();
}

IncrementalTerrain::~IncrementalTerrain() {
    clear_list(_groups);
    delete _grid;
}

PaletteIndex IncrementalTerrain::getPaletteIndex(int x, int y, int z) {
    return _grid->getPaletteIndex(x, y, z);
}

void IncrementalTerrain::setPaletteIndex(int x, int y, int z, PaletteIndex newType) {
    PaletteIndex oldType = _grid->getPaletteIndex(x, y, z);

    if(newType >= _groups.size() && newType != TILE_EMPTY) {
        Material *mat = _tilePalette->getMaterialForPalette(newType);
        for(int i=_groups.size(); i<=newType; i++) { _groups.push_back(NULL); }

        char buffer[32];
        snprintf(buffer, 32, "incremental_terrain_group_%i", newType);
        Entity *entity = _sceneManager->create<Entity>(buffer);

        _groups[newType] = new IncrementalTerrainModel();
        _groups[newType]->setDefaultMaterial(mat);
        entity->setModel(_groups[newType]);
    }

    if(oldType != newType) {
        _grid->setPaletteIndex(x, y, z, newType);

        if (_autoUpdate) {
            _groups[oldType]->removeTile(x, y, z);
            _groups[newType]->addTile(x, y, z);
        }
    }
}

int IncrementalTerrain::getSurfaceLevel(int x, int y) {
    return _grid->getSurfaceLevel(x, y);
}

void IncrementalTerrain::clear() {
    for (int i = 0; i < _groups.size(); i++) {
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
                _groups[getPaletteIndex(x, y, z)]->addTile(x, y, z);
            }
        }
    }
    _autoUpdate = oldVal;
}
