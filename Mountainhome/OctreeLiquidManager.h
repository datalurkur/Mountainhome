#ifndef _OCTREELIQUIDMANAGER_H_
#define _OCTREELIQUIDMANAGER_H_

#include "MHLiquidManager.h"
#include "LiquidPool.h"
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

class OctreeLiquidManager: public MHLiquidManager {
public:
    OctreeLiquidManager(MHTerrain *terrain);
    virtual ~OctreeLiquidManager();

    // Returns the type of liquid present in [x,y,z]
    virtual short getLiquidType(int x, int y, int z);

    // Returns the amount of liquid contained in [x,y,z]
    virtual float getLiquidVolume(int x, int y, int z);

    // Sets all variables for a given [x,y,z]
    virtual void setLiquid(int x, int y, int z, short type, float depth);

    // Sets the type of [x,y,z]
    virtual void setLiquidType(int x, int y, int z, short type);

    // Sets the depth of [x,y,z]
    virtual void setLiquidDepth(int x, int y, int z, float depth);

    // Adds some amount of liquid to a pool, returning the amount left over if the
    // destination pool is filled in the process
    virtual float addDepth(int x, int y, int z, short type, float amount);

    // Attempt to fill to the level specified, returning the difference
    virtual float fillTo(int x, int y, int z, short type, float depth);

    // Add entities to the scenemanager for rendering
    virtual void populate(OctreeSceneManager *scene, MaterialManager *mManager);

private:
    LiquidPool *_rootPool;
};

#endif
