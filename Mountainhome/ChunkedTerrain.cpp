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
OctreeSceneManager *scene, MaterialManager *manager): MHTerrain(width, height, depth),
_sceneManager(scene), _materialManager(manager)
{
    _grid = new MatrixTileGrid(width, height, depth);

    // We MUST set set place 0 to NULL to handle air.
    _groups.push_back(NULL);

    clear();
}

ChunkedTerrain::~ChunkedTerrain() {
    clear_list(_groups);
    delete _grid;
}

TileType ChunkedTerrain::registerTileType(const std::string &materialName) {
    TileType newType = _tileTypeCount++;

    Material *mat = _materialManager->getCachedResource(materialName);
    _groups.push_back(new ChunkedTerrainGroup(newType, _grid, _sceneManager, mat));

    ASSERT_EQ(_tileTypeCount, _groups.size());

    return newType;
}

TileType ChunkedTerrain::getTileType(int x, int y, int z) {
    return _grid->getTileType(x, y, z);
}

void ChunkedTerrain::setTileType(int x, int y, int z, TileType newType) {
    TileType oldType = _grid->getTileType(x, y, z);

    ASSERT(newType < getTileTypeCount());
    ASSERT(oldType < getTileTypeCount());

    if (oldType != newType) {
        _grid->setTileType(x, y, z, newType);

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
    for (int i = 0; i < getTileTypeCount(); i++) {
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
    for (int i = 0; i < getTileTypeCount(); i++) {
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
