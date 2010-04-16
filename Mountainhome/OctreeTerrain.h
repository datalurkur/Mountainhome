/*
 *  OctreeTerrain.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _OCTREETERRAIN_H_
#define _OCTREETERRAIN_H_
#include "TileGroup.h"
#include "MHTerrain.h"

class Model;
class OctreeTerrain: public MHTerrain {
public:
    OctreeTerrain(int width, int height, int depth);
    virtual ~OctreeTerrain();
    
    virtual TileGroup::TileData getTile(int x, int y, int z);
    virtual void setTile(int x, int y, int z, TileGroup::TileData type);
    virtual int getSurfaceLevel(int x, int y);
    virtual void clear();
    
    virtual int getWidth();
    virtual int getHeight();
    virtual int getDepth();
    
    virtual void save(std::string filename);
    virtual void load(std::string filename);

    virtual void populate(OctreeSceneManager *scene, MaterialManager *mManager);

private:
    TileGroup *_rootGroup;
    std::list<Model*> _models;

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
