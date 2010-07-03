/*
 *  MHTerrain.h
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHTERRAIN_H_
#define _MHTERRAIN_H_
#include <Base/Vector.h>
#include "RubyBindings.h"

typedef unsigned char TileType;

#include "TileGroup.h"

class OctreeSceneManager;
class MaterialManager;

// TEMPORARY DEFINES UNTIL WE GET SOME ACTUAL TILE TYPES
#define TILE_EMPTY      0
#define TILE_SEDIMENT   1
// =====================================================

class MHTerrain: public RubyBindings<MHTerrain, false> {
//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHTerrain ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
public:
    static void SetupBindings();
    static VALUE GetTile(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE SetTile(VALUE self, VALUE x, VALUE y, VALUE z, VALUE type);
    static VALUE SurfaceTile(VALUE self, VALUE x, VALUE y);
    static VALUE Clear(VALUE self);

    /*! Gets the world's width. */
    static VALUE GetWidth(VALUE self);

    /*! Gets the world's height. */
    static VALUE GetHeight(VALUE self);

    /*! Gets the world's depth. */
    static VALUE GetDepth(VALUE self);

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHTerrain declarations
//////////////////////////////////////////////////////////////////////////////////////////
public:
    MHTerrain();
    MHTerrain(int width, int height, int depth);
    virtual ~MHTerrain();

    virtual TileType getTile(int x, int y, int z) = 0;
    virtual void setTile(int x, int y, int z, TileType type) = 0;
    virtual int getSurfaceLevel(int x, int y) = 0;
    virtual void clear() = 0;

    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    virtual int getDepth() = 0;

    virtual void save(std::string filename) = 0;
    virtual void load(std::string filename) = 0;

    virtual void populate(OctreeSceneManager *scene, MaterialManager *mManager, bool reduce) = 0;
};

#endif
