/*
 *  MHLiquidManager.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHLIQUIDMANAGER_H_
#define _MHLIQUIDMANAGER_H_

#include "RubyBindings.h"
#include "MHTerrain.h"

// TEMPORARY DEFINES UNTIL WE GET SOME ACTUAL TILE TYPES
enum {
    LIQUID_EMPTY = 0,
    LIQUID_WATER,
    LIQUID_TYPE_COUNT
};

class MHLiquidManager: public RubyBindings<MHLiquidManager, false> {
public:
    static void SetupBindings();

    static VALUE SetLiquid(VALUE self, VALUE x, VALUE y, VALUE z, VALUE type, VALUE volume);
    static VALUE GetLiquidType(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE GetLiquidVolume(VALUE self, VALUE x, VALUE y, VALUE z);

public:
    MHLiquidManager();
    virtual ~MHLiquidManager();

    // Returns the type of liquid present in [x,y,z]
    virtual TileType getLiquidType(int x, int y, int z) = 0;

    // Returns the amount of liquid contained in [x,y,z]
    virtual float getLiquidVolume(int x, int y, int z) = 0;

    // Sets all variables for a given [x,y,z]
    virtual void setLiquid(int x, int y, int z, TileType type, float depth) = 0;

    // Sets the type of [x,y,z]
    virtual void setLiquidType(int x, int y, int z, TileType type) = 0;

    // Sets the depth of [x,y,z]
    virtual void setLiquidDepth(int x, int y, int z, float depth) = 0;

    virtual void populate(bool reduce) = 0;

};

#endif
