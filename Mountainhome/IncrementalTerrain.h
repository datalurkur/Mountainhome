/*
 *  IncrementalTerrain.h
 *  Mountainhome
 *
 *  Created by loch on 7/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _INCREMENTALTERRAIN_H_
#define _INCREMENTALTERRAIN_H_
#include "MHTerrain.h"

class Model;
class IncrementalTerrainModel;
class IncrementalTerrain : public MHTerrain {
public:
    IncrementalTerrain(int width, int height, int depth,
        OctreeSceneManager *scene, MaterialManager *manager);

    virtual ~IncrementalTerrain();
    
    virtual TileType getTile(int x, int y, int z);
    virtual void setTile(int x, int y, int z, TileType type);
    virtual int getSurfaceLevel(int x, int y);
    virtual void clear();

    virtual void save(std::string filename);
    virtual void load(std::string filename);

    virtual void populate(bool reduce);

private:
    TileGrid *_grid;
    std::vector<IncrementalTerrainModel*> _groups;
    OctreeSceneManager *_sceneManager;
    MaterialManager *_materialManager;
};

#endif
