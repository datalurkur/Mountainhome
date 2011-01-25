/*
 *  BlockChunkedTerrainRenderable.h
 *  Mountainhome
 *
 *  Created by loch on 8/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef BLOCKCHUNKEDTERRAINRENDERABLE_H_
#define BLOCKCHUNKEDTERRAINRENDERABLE_H_
#include "ChunkedTerrainRenderable.h"

class DynamicModel;
class BlockChunkedTerrainRenderable : public ChunkedTerrainRenderable {
public:
    BlockChunkedTerrainRenderable(TileGrid *grid, PaletteIndex type,
        int xChunkIndex, int yChunkIndex, int zChunkIndex);

    virtual int update(bool doPolyReduction);

protected:
    void addGeometry(int xPos, int yPos, int zPos, DynamicModel *model);

private:
    unsigned int XChunkSize, YChunkSize, ZChunkSize;
};

#endif
