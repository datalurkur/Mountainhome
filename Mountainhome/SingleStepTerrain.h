/*
 *  SingleStepTerrain.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _SINGLESTEPTERRAIN_H_
#define _SINGLESTEPTERRAIN_H_
#include "OctreeTileGrid.h"
#include "MHTerrain.h"

class Model;
class SingleStepTerrain : public MHTerrain {
public:
    SingleStepTerrain(int width, int height, int depth,
        OctreeSceneManager *scene, MaterialManager *manager);

    virtual ~SingleStepTerrain();

    TileType registerTileType(const std::string &materialName);

    virtual TileType getTileType(int x, int y, int z);
    virtual bool getTileParameter(int x, int y, int z, TileParameter type);

    virtual void setTileType(int x, int y, int z, TileType type);
    virtual void setTileParameter(int x, int y, int z, TileParameter param, bool value);

    virtual int getSurfaceLevel(int x, int y);
    virtual void clear();

    virtual void save(const std::string &filename);
    virtual void load(const std::string &filename);

    virtual void populate();

private:
    TileGrid *_grid;
    std::list<Model*> _models;

    OctreeSceneManager *_sceneManager;
    MaterialManager *_materialManager;

    int **_surfaceCache;

    void initCache();
    int cacheIndex(int x, int y);
    bool getCacheValue(int x, int y, int *value);
    void setCacheValue(int x, int y, int value);
    void clearCache();
};


#endif
