/*
 *  TerrainChunkRenderable.h
 *  Mountainhome
 *
 *  Created by loch on 1/24/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TERRAINCHUNKRENDERABLE_H_
#define _TERRAINCHUNKRENDERABLE_H_
#include <Render/Renderable.h>
#include "ChunkedTerrain.h"

class TileGrid;
class Material;

class TerrainChunkRenderable : public Renderable {
public:
    TerrainChunkRenderable(TileGrid * grid, Material *mat);

    void enablePreRenderPolyReduction(bool value);

    virtual void preRenderNotice();

    void markDirty();

protected:
    static const int ChunkSize = ChunkedTerrain::ChunkSize;

    virtual void generateGeometry(bool doPolyReduction) = 0;

private:
    bool _preRenderPolyReduction;

    bool _dirty;

    TileGrid *_grid;

};

#endif
