/*
 *  SmoothChunkedTerrainModel.h
 *  Mountainhome
 *
 *  Created by loch on 8/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef SMOOTHCHUNKEDTERRAINMODEL_H_
#define SMOOTHCHUNKEDTERRAINMODEL_H_
#include "ChunkedTerrainModel.h"

class SmoothChunkedTerrainModel : public ChunkedTerrainModel {
public:
    SmoothChunkedTerrainModel(TileGrid *grid, TileType type,
        int xChunkIndex, int yChunkIndex, int zChunkIndex);

    virtual int update(bool doPolyReduction);

protected:
    void findVertexLocForTile(int x, int y, int z, int *result, TileType cardinalType);
    void buildGeometry(int xPos, int yPos, DynamicModel *model);

};

#endif
