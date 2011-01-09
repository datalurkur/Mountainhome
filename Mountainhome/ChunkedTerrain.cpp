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
#include <Engine/Entity.h>

#include "ChunkedTerrain.h"
#include "MatrixTileGrid.h"
#include "ChunkedTerrainGroup.h"

ChunkedTerrain::ChunkedTerrain(int width, int height, int depth,
OctreeSceneManager *scene, MaterialManager *manager): MHTerrain(width, height, depth, scene, manager)
{
    _grid = new MatrixTileGrid(width, height, depth);
    clear();
}

ChunkedTerrain::~ChunkedTerrain() {
    clear_list(_groups);
    delete _grid;
}

PaletteIndex ChunkedTerrain::getPaletteIndex(int x, int y, int z) {
    return _grid->getPaletteIndex(x, y, z);
}

void ChunkedTerrain::setPaletteIndex(int x, int y, int z, PaletteIndex newType) {
    PaletteIndex oldType = _grid->getPaletteIndex(x, y, z);

    if(newType >= _groups.size() && newType != TILE_EMPTY) {
        Material *mat = _tilePalette->getMaterialForPalette(newType);
        for(int i=_groups.size(); i<=newType; i++) { _groups.push_back(NULL); }
        _groups[newType] = new ChunkedTerrainGroup(newType, _grid, _sceneManager, mat);
    }

    if (oldType != newType) {
        _grid->setPaletteIndex(x, y, z, newType);

        if (_autoUpdate) {
            if (oldType != TILE_EMPTY) { _groups[oldType]->update(x, y, z, _polyReduction); }
            if (newType != TILE_EMPTY) { _groups[newType]->update(x, y, z, _polyReduction); }
        }
    }
}

int ChunkedTerrain::getSurfaceLevel(int x, int y) {
    return _grid->getSurfaceLevel(x, y);
}

int ChunkedTerrain::getEmptyRanges(int x, int y, std::vector<std::pair<int,int> > &ranges) {
    return _grid->getEmptyRanges(x, y, ranges);
}

int ChunkedTerrain::getFilledRanges(int x, int y, std::vector<std::pair<int,int> > &ranges) {
    return _grid->getFilledRanges(x, y, ranges);
}

void ChunkedTerrain::clear() {
    for (int i = 0; i < _groups.size(); i++) {
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
    for (int i = 0; i < _groups.size(); i++) {
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
