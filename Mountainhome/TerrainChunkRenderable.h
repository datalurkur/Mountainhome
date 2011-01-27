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
#include "TilePalette.h"

class TileGrid;
class Material;

class TerrainChunkRenderable : public Renderable {
public:
    TerrainChunkRenderable(
        int xChunkIndex, int yChunkIndex, int zChunkIndex,
        PaletteIndex index, TileGrid *grid, Material *mat);

    void enablePreRenderPolyReduction(bool value);

    virtual void preRenderNotice();

    void markDirty();

    void generateGeometry();

protected:
    static const int ChunkSize;

    virtual void generateGeometry(bool doPolyReduction) = 0;

protected:
    int _xChunkIndex, _yChunkIndex, _zChunkIndex;

    bool _preRenderPolyReduction;

    bool _dirty;

    PaletteIndex _index;

    TileGrid *_grid;

};

#endif
