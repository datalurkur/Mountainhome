/*
 *  ChunkedTerrainGroup.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ChunkedTerrainGroup.h"
#include "TileGrid.h"
#include "MaterialManager.h"
#include "OctreeSceneManager.h"

ChunkedTerrainGroup::ChunkedTerrainGroup(TileGrid *grid, OctreeSceneManager *scene,
MaterialManager *manager): _grid(grid), _sceneManager(scene), _materialManager(manager) {}

ChunkedTerrainGroup::~ChunkedTerrainGroup() {}

void ChunkedTerrainGroup::updateAll() {}
void ChunkedTerrainGroup::update(int x, int y, int z) {}
void ChunkedTerrainGroup::clear() {}
