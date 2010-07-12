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

    virtual TileType getTile(int x, int y, int z);
    virtual void setTile(int x, int y, int z, TileType type);
    virtual int getSurfaceLevel(int x, int y);
    virtual void clear();
    
    virtual int getWidth();
    virtual int getHeight();
    virtual int getDepth();
    
    virtual void save(std::string filename);
    virtual void load(std::string filename);

    virtual void populate(bool final);

private:
    TileGrid *_rootGroup;
    std::list<Model*> _models;

    OctreeSceneManager *_sceneManager;
    MaterialManager *_materialManager;

    int **_surfaceCache;

    Real _tileWidth;       /*!< The width  of the tile in world units. */
    Real _tileHeight;      /*!< The height of the tile in world units. */
    Real _tileDepth;       /*!< The depth  of the tile in world units. */

    void initCache();
    int cacheIndex(int x, int y);
    bool getCacheValue(int x, int y, int *value);
    void setCacheValue(int x, int y, int value);
    void clearCache();
};


#endif
