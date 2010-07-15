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

#include "OctreeTileGrid.h"

class OctreeSceneManager;
class MaterialManager;

// TEMPORARY DEFINES UNTIL WE GET SOME ACTUAL TILE TYPES
enum {
    TILE_EMPTY = 0,
    TILE_SEDIMENT,
    TILE_ROCK,
    TILE_TYPE_COUNT
};

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

    static VALUE SetPolyReduction(VALUE self, VALUE val);

    static VALUE SetAutoUpdate(VALUE self, VALUE val);

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHTerrain declarations
//////////////////////////////////////////////////////////////////////////////////////////
public:
    MHTerrain(int width, int height, int depth):
        _width(width), _height(height), _depth(depth),
        _polyReduction(false), _autoUpdate(false) {}

    virtual ~MHTerrain() {}

    virtual TileType getTile(int x, int y, int z) = 0;
    virtual void setTile(int x, int y, int z, TileType type) = 0;
    virtual int getSurfaceLevel(int x, int y) = 0;
    virtual void clear() = 0;

    virtual void save(const std::string &filename) = 0;
    virtual void load(const std::string &filename) = 0;

    virtual void populate() = 0;

    virtual int getWidth()  { return _width;  }
    virtual int getHeight() { return _height; }
    virtual int getDepth()  { return _depth;  }

    virtual void setPolyReduction(bool val) { _polyReduction = val; }
    virtual void setAutoUpdate   (bool val) { _autoUpdate    = val; }

protected:
    int _width, _height, _depth;
    bool _polyReduction;
    bool _autoUpdate;

};

#endif
