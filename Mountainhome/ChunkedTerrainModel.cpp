/*
 *  ChunkedTerrainModel.cpp
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ChunkedTerrainModel.h"
#include "ChunkedTerrainGroup.h"

ChunkedTerrainModel::ChunkedTerrainModel(TileGrid *grid, TileType type, int x, int y, int z):
_grid(grid), _type(type), _x(x), _y(y), _z(z), _count(0) {
    char buffer[32];
    snprintf(buffer, 32, "terrain_chunk_%i_%i_%i_%i", _type, _x, _y, _z);
    _name = buffer;
}
ChunkedTerrainModel::~ChunkedTerrainModel() {}

std::string ChunkedTerrainModel::getName() {
    return _name;
}

bool ChunkedTerrainModel::update(int change) {
    _count += change;
    return _count == 0;
}
