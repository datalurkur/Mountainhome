#ifndef _MHLIQUIDMANAGER_H_
#define _MHLIQUIDMANAGER_H_

#include "RubyBindings.h"
#include "MHTerrain.h"

class MHLiquidManager: public RubyBindings<MHLiquidManager, false> {
public:
    static void SetupBindings();

public:
    MHLiquidManager(MHTerrain *terrain);
    virtual ~MHLiquidManager();

    // Returns the type of liquid present in [x,y,z]
    virtual int getLiquidType(int x, int y, int z) = 0;

    // Returns the amount of liquid contained in [x,y,z]
    virtual float getLiquidDepth(int x, int y, int z) = 0;

    // Sets all variables for a given [x,y,z]
    virtual void setLiquid(int x, int y, int z, short type, float depth) = 0;

    // Sets the type of [x,y,z]
    virtual void setLiquidType(int x, int y, int z, short type) = 0;

    // Sets the depth of [x,y,z]
    virtual void setLiquidDepth(int x, int y, int z, float depth) = 0;

    // Adds some amount of liquid to a pool, returning the amount left over if the
    // destination pool is filled in the process
    virtual float addDepth(int x, int y, int z, float amount) = 0;

    // Attempt to fill to the level specified, returning the difference
    virtual float fillTo(int x, int y, int z, float depth) = 0;

protected:
    MHTerrain *_terrain;
};

#endif
