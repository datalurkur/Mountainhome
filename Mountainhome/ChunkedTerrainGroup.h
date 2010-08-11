/*
 *  ChunkedTerrainGroup.h
 *  Mountainhome
 *
 *  Created by loch on 7/12/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _CHUNKEDTERRAINGROUP_H_
#define _CHUNKEDTERRAINGROUP_H_
#include "TileGrid.h"

class Material;
class OctreeSceneManager;
class ChunkedTerrainModel;

class ChunkedTerrainGroup {
public:
    static const int ChunkSize = 25;

public:
    ChunkedTerrainGroup(TileType type, TileGrid *grid, OctreeSceneManager *scene, Material *material);
    virtual ~ChunkedTerrainGroup();

    void updateAll(bool doPolyReduction);
    void update(int x, int y, int z, bool doPolyReduction);
    void clear();

private:
    typedef int IndexType;
    typedef std::map<IndexType, ChunkedTerrainModel*> ChunkLookupMap;

    static const size_t BitsPerDim = sizeof(IndexType) * 8 / 3;

private:
    void updateIfExists(int x, int y, int z, bool doPolyReduction);
    void removeChunk(ChunkLookupMap::iterator itr);
    void createChunkIfNeeded(int x, int y, int z);

private:
    TileType _type;
    TileGrid *_grid;
    OctreeSceneManager *_sceneManager;
    Material *_material;
    ChunkLookupMap _chunks;

};

#endif
