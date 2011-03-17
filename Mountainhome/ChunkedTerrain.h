/*
 *  ChunkedTerrain.h
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _CHUNKEDTERRAIN_H_
#define _CHUNKEDTERRAIN_H_
#include "MHTerrain.h"

class TerrainChunkNode;
class TileGrid;

class ChunkedTerrain : public MHTerrain {
public:
    static const int ChunkSize = 25;

public:
    ChunkedTerrain(int width, int height, int depth, OctreeSceneManager *scene);

    virtual ~ChunkedTerrain();

    virtual PaletteIndex getPaletteIndex(int x, int y, int z);

    virtual void setPaletteIndex(int x, int y, int z, PaletteIndex type);

    virtual int getSurfaceLevel(int x, int y);

    virtual int getEmptyRanges(int x, int y, std::vector<std::pair<int,int> > &ranges);

    virtual int getFilledRanges(int x, int y, std::vector<std::pair<int,int> > &ranges);

    /*! Resets the terrain to be entirely empty and destroys all terrain geometry. */
    virtual void clear();

    /*! Saves the terrain to the given file. */
    virtual void save(const std::string &filename);

    /*! Loads the terrain to the given file and generates all geometry. */
    virtual void load(const std::string &filename);

    /*! Causes all geometry in the world to generate itself. This is an expensive
     *  operation and should generally only be done as a precursor to initial
     *  rendering. */
    virtual void populate();

private:
    typedef int IndexType;
    typedef std::map<IndexType, TerrainChunkNode*> ChunkLookupMap;
    static const size_t BitsPerDim = sizeof(IndexType) * 8 / 3;

private:
    /*! Marks the given PaletteIndex in the TerrainChunkNode at the given location as
     *  dirty, creating the TerrainChunkNode if it does not already exist.
     * \note This will only dirty the node if set to auto update. */
    void markDirty(int x, int y, int z, PaletteIndex type);

    /*! Retrieves the TerrainChunkNode for the giving location. It one does not already
     *  exist, it will be created and added to the chunk map. */
    TerrainChunkNode *findOrCreateChunkNode(int x, int y, int z);

private:
    TileGrid *_grid;

    ChunkLookupMap _chunks;

};
#endif
