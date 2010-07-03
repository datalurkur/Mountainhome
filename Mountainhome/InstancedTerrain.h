/*
 *  InstancedTerrain.h
 *  Mountainhome
 *
 *  Created by loch on 7/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef __INSTANCEDTERRAIN_H_
#define __INSTANCEDTERRAIN_H_
#include "MHTerrain.h"

class Model;
class InstancedTerrain : public MHTerrain {
public:
    InstancedTerrain(int width, int height, int depth);
    virtual ~InstancedTerrain();
    
    virtual TileType getTile(int x, int y, int z);
    virtual void setTile(int x, int y, int z, TileType type);
    virtual int getSurfaceLevel(int x, int y);
    virtual void clear();
    
    virtual int getWidth();
    virtual int getHeight();
    virtual int getDepth();
    
    virtual void save(std::string filename);
    virtual void load(std::string filename);

    virtual void populate(OctreeSceneManager *scene, MaterialManager *mManager, bool reduce);

private:
    TileType *_typeMatrix;
    int _width, _height, _depth;
    std::list<Model*> _models;

};

#endif
