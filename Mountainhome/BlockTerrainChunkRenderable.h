/*
 *  BlockTerrainChunkRenderable.h
 *  Mountainhome
 *
 *  Created by loch on 8/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _BLOCKTERRAINCHUNKRENDERABLE_H_
#define _BLOCKTERRAINCHUNKRENDERABLE_H_
#include "TerrainChunkRenderable.h"

class DynamicModel;
class TerrainChunk;
class Terrain;

class BlockTerrainChunkRenderable : public TerrainChunkRenderable {
public:
    BlockTerrainChunkRenderable(Terrain *terrain, TerrainChunk *parent, PaletteIndex index, Material *mat);

protected:
    bool isIndexEmpty(int localX, int localY, int localZ, bool padBounries = true);

    virtual void generateGeometry(bool doPolyReduction);

    void addGeometry(int xPos, int yPos, int zPos);

private:
    DynamicModel *_dynamicModel;

};

#endif
