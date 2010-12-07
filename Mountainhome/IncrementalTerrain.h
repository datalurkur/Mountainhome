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

    virtual PaletteIndex getPaletteIndex(int x, int y, int z);
    virtual void setPaletteIndex(int x, int y, int z, PaletteIndex type);

    virtual int getSurfaceLevel(int x, int y);

    virtual void clear();

    virtual void save(const std::string &filename);
    virtual void load(const std::string &filename);

    virtual void populate();

private:
    TileGrid *_grid;
    std::vector<IncrementalTerrainModel*> _groups;
};

#endif
