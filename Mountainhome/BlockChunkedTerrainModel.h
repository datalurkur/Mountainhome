/*
 *  BlockChunkedTerrainModel.h
 *  Mountainhome
 *
 *  Created by loch on 8/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef BLOCKCHUNKEDTERRAINMODEL_H_
#define BLOCKCHUNKEDTERRAINMODEL_H_
#include "ChunkedTerrainModel.h"

class BlockChunkedTerrainModel : public ChunkedTerrainModel {
public:
    BlockChunkedTerrainModel(TileGrid *grid, TileType type,
        int xChunkIndex, int yChunkIndex, int zChunkIndex);

    virtual int update(bool doPolyReduction);

protected:
    void addGeometry(int xPos, int yPos, int zPos, DynamicModel *model);

};

#endif
