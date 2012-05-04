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
#include "VoxelPalette.h"

class Terrain;
class TerrainChunk;
class VoxelGrid;
class Material;

class TerrainChunkRenderable : public Renderable {
public:
    TerrainChunkRenderable(Terrain *terrain, TerrainChunk *parent, PaletteIndex index, Material *mat);

    void enablePreRenderPolyReduction(bool value);

    virtual void preRenderNotice();

    void markDirty();

    void generateGeometry();

protected:
    static const int ChunkSize;

    virtual void generateGeometry(bool doPolyReduction) = 0;

protected:
    Terrain *_terrain;

    TerrainChunk *_parent;

    bool _preRenderPolyReduction;

    bool _dirty;

    PaletteIndex _index;

};

#endif
