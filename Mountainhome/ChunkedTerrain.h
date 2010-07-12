/*
 *  ChunkedTerrain.h
 *  Mountainhome
 *
 *  Created by loch on 7/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _CHUNKEDTERRAIN_H_
#define _CHUNKEDTERRAIN_H_
#include "MHTerrain.h"

class Model;
class TerrainChunk;
class ChunkedTerrain : public MHTerrain {
public:
    ChunkedTerrain(int width, int height, int depth,
        OctreeSceneManager *scene, MaterialManager *manager);

    virtual ~ChunkedTerrain();
    
    virtual TileType getTile(int x, int y, int z);
    virtual void setTile(int x, int y, int z, TileType type);
    virtual int getSurfaceLevel(int x, int y);
    virtual void clear();
    
    virtual int getWidth();
    virtual int getHeight();
    virtual int getDepth();
    
    virtual void save(std::string filename);
    virtual void load(std::string filename);

    virtual void populate(bool reduce);

private:
    TileGrid *_grid;
    std::vector<TerrainChunk*> _chunks;
    OctreeSceneManager *_sceneManager;
    MaterialManager *_materialManager;
};

#endif
