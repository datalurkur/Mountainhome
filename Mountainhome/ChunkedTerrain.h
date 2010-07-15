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

class Model;
class ChunkedTerrainGroup;
class ChunkedTerrain : public MHTerrain {
public:
    ChunkedTerrain(int width, int height, int depth,
        OctreeSceneManager *scene, MaterialManager *manager);

    virtual ~ChunkedTerrain();
    
    virtual TileType getTile(int x, int y, int z);
    virtual void setTile(int x, int y, int z, TileType type);
    virtual int getSurfaceLevel(int x, int y);
    virtual void clear();
    
    virtual void save(const std::string &filename);
    virtual void load(const std::string &filename);

    virtual void populate();

private:
    TileGrid *_grid;
    std::vector<ChunkedTerrainGroup*> _groups;

};
#endif
