/*
 *  SingleStepLiquidManager.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _SINGLESTEPLIQUIDMANAGER_H_
#define _SINGLESTEPLIQUIDMANAGER_H_

#include "MHLiquidManager.h"
#include <Render/RenderContext.h>
#include "Model.h"

class TestLiquidModel : public Model {
public:
    TestLiquidModel(const Vector3 &position, const float &volume);
    virtual ~TestLiquidModel();

    void render(RenderContext *context);

protected:
    Vector3 _pos;
    float _volume;
};

class SingleStepLiquidManager: public MHLiquidManager {
public:
    SingleStepLiquidManager(MHTerrain *terrain, OctreeSceneManager *scene);
    virtual ~SingleStepLiquidManager();

    /* Returns the type of liquid present in [x,y,z] */
    virtual TileType getLiquidType(int x, int y, int z);

    // Returns the amount of liquid contained in [x,y,z]
    virtual float getLiquidVolume(int x, int y, int z);

    // Sets all variables for a given [x,y,z]
    virtual void setLiquid(int x, int y, int z, TileType type, float depth);

    // Sets the type of [x,y,z]
    virtual void setLiquidType(int x, int y, int z, TileType type);

    // Sets the depth of [x,y,z]
    virtual void setLiquidDepth(int x, int y, int z, float depth);

    // Add entities to the scenemanager for rendering
    virtual void populate();

private:
    TileGrid *_rootPool;

    MHTerrain *_terrain;
    OctreeSceneManager *_sceneManager;

};

#endif
