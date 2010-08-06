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
#include "MHTerrain.h"

ChunkedTerrainModel::ChunkedTerrainModel(TileGrid *grid, TileType type,
int xChunkIndex, int yChunkIndex, int zChunkIndex):
_grid(grid), _type(type),
_xLoc(xChunkIndex * ChunkSize),
_yLoc(yChunkIndex * ChunkSize),
_zLoc(zChunkIndex * ChunkSize)
{
    char buffer[32];
    snprintf(buffer, 32, "terrain_chunk_%i_%i_%i_%i", _type, xChunkIndex, yChunkIndex, zChunkIndex);
    _name = buffer;
}
ChunkedTerrainModel::~ChunkedTerrainModel() {}

std::string ChunkedTerrainModel::getName() {
    return _name;
}


