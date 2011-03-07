/*
 *  TerrainChunkNode.h
 *  Mountainhome
 *
 *  Created by loch on 1/24/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TERRAINCHUNKNODE_H_
#define _TERRAINCHUNKNODE_H_
#include <Engine/Entity.h>
#include "ChunkedTerrain.h"
#include "TilePalette.h"

class TerrainChunkRenderable;

class TerrainChunkNode : public Entity {
public:
    TerrainChunkNode(int xChunkIndex, int yChunkIndex, int zChunkIndex, TilePalette *palette, TileGrid *grid);

    /*! Marks the Renderable for the given palette index as dirty, indicating its geometry
     *  should be regenerated before being rendered to screen. */
    void markDirty(PaletteIndex index);

    /*! Forces all Renderables to be created and all geometry to be generated. */
    int populate();

protected:
    static const int ChunkSize = ChunkedTerrain::ChunkSize;
    static const int DefaultCapacity = 128;

protected:
    int _xChunkIndex, _yChunkIndex, _zChunkIndex;

    std::vector<TerrainChunkRenderable*> _paletteRenderables;

    TilePalette *_palette;

    TileGrid *_grid;

};

#endif
