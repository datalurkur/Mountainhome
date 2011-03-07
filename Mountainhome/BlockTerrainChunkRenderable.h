/*
 *  BlockTerrainChunkRenderable.h
 *  Mountainhome
 *
 *  Created by loch on 8/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef BLOCKTERRAINCHUNKRENDERABLE_H_
#define BLOCKTERRAINCHUNKRENDERABLE_H_
#include "TerrainChunkRenderable.h"

class DynamicModel;
class BlockTerrainChunkRenderable : public TerrainChunkRenderable {
public:
    BlockTerrainChunkRenderable(
        int xChunkIndex, int yChunkIndex, int zChunkIndex,
        PaletteIndex index, TileGrid *grid, Material *mat);

protected:
    virtual void generateGeometry(bool doPolyReduction);

    void addGeometry(int xPos, int yPos, int zPos, DynamicModel *model);

private:
    /*! A chunk may not fit the maximum size in all directions. These represent the actual
     *  size of the chunk in different dimensions. */
    unsigned int _xChunkSize, _yChunkSize, _zChunkSize;

};

#endif
