/*
 *  TerrainChunk.h
 *  Mountainhome
 *
 *  Created by loch on 1/24/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TerrainChunk_H_
#define _TerrainChunk_H_
#include <Engine/Entity.h>
#include "Terrain.h"

class TerrainChunkRenderable;
class VoxelPalette;
class VoxelGrid;

class TerrainChunk : public Entity {
public:
    TerrainChunk(int xChunkIndex, int yChunkIndex, int zChunkIndex, VoxelPalette *palette);

    /*! Marks the Renderable for the given palette index as dirty, indicating its geometry
     *  should be regenerated before being rendered to screen. */
    void markDirty(PaletteIndex index);

    /*! Gets the local VoxelGrid that represents the contents of the world in this chunk. */
    VoxelGrid *getLocalGrid();

    /*! Forces a complete regeneration of all node geometry, regardless of dirty flags.
     *  This can be used to bring a node's geometry totally up to date if previous calls
     *  to markDirty were being skipped, such as during world generation, which uses this
     *  technique to make large changes to the world while only displaying the changes at
     *  certain, controlled intervals.
     * \return The number of renderables in the node.
     * \note This doesn't test to see if the renderables actually have any geometry
     *  associated with them before adding them to the returned renderable count. */
    int populate();

    int getXChunkIndex() { return _xChunkIndex; }
    int getYChunkIndex() { return _yChunkIndex; }
    int getZChunkIndex() { return _zChunkIndex; }

protected:
    static const int ChunkSize = Terrain::ChunkSize;
    static const int DefaultCapacity = 128;

protected:
    int _xChunkIndex, _yChunkIndex, _zChunkIndex;

    std::vector<TerrainChunkRenderable*> _paletteRenderables;

    VoxelPalette *_palette;

    VoxelGrid *_grid;

};

#endif
