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
#include <Base/Timer.h>
#include <Base/FileSystem.h>
#include <Render/MaterialManager.h>
#include <Render/Entity.h>

#include "OctreeSceneManager.h"
#include "ChunkedTerrain.h"
#include "MatrixTileGrid.h"
#include "ChunkedTerrainGroup.h"

ChunkedTerrain::ChunkedTerrain(int width, int height, int depth,
OctreeSceneManager *scene, MaterialManager *manager)
: MHTerrain(width, height, depth), _materialManager(manager)
{
    _grid = new MatrixTileGrid(width, height, depth);

    _groups.reserve(TILE_TYPE_COUNT);
    _groups[0] = NULL;

    for (int i = 1; i < TILE_TYPE_COUNT; i++) {
        _groups[i] = new ChunkedTerrainGroup(i, _grid, scene, getMaterialForType(i));
    }

    clear();
}

ChunkedTerrain::~ChunkedTerrain() {
    clear_list(_groups);
    delete _grid;
}

Material *ChunkedTerrain::getMaterialForType(int type) {
    // Determine the model name to use.
    const char *name;
    switch (type) {
    case 1: name = "grass"; break;
    case 2: name = "gravel"; break;
    default:
        THROW(ItemNotFoundError, "Tile type " << type << " is unknown.");
    }

    return _materialManager->getCachedResource(name);
}
    
TileType ChunkedTerrain::getTile(int x, int y, int z) {
    return _grid->getTile(x, y, z);
}

void ChunkedTerrain::setTile(int x, int y, int z, TileType newType) {
    TileType oldType = _grid->getTile(x, y, z);

    ASSERT(newType < TILE_TYPE_COUNT);
    ASSERT(oldType < TILE_TYPE_COUNT);

    if (oldType != newType) {
        _grid->setTile(x, y, z, newType);

        if (_autoUpdate) {
            if (oldType != 0) { _groups[oldType]->update(x, y, z, _polyReduction); }
            if (newType != 0) { _groups[newType]->update(x, y, z, _polyReduction); }
        }
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
    
void ChunkedTerrain::save(const std::string &filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Write);
    _grid->save(file);
    delete file;
}

void ChunkedTerrain::load(const std::string &filename) {
    File *file = FileSystem::GetFile(filename, IOTarget::Read);
    _grid->load(file);
    delete file;

    populate();
}

void ChunkedTerrain::populate() {
    Timer t;
    if (_polyReduction) { t.start(); }

    int count = 0;
    for (int i = 0; i < TILE_TYPE_COUNT; i++) {
        if (_groups[i]) { count += _groups[i]->updateAll(_polyReduction); }
    }

    if (_polyReduction) {
        t.stop();
        Info("World population stats:");
        Info("    World size:      " << _width << "x" << _height << "x" << _depth);
        Info("    Chunks updated:  " << count);
        Info("    Time (seconds):  " << t.seconds()  << " (" << (t.seconds()  / count) << " per chunk)");
        Info("    Time (mseconds): " << t.mseconds() << " (" << (t.mseconds() / count) << " per chunk)");
    }
}
