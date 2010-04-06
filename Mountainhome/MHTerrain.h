#ifndef _MHTERRAIN_H_
#define _MHTERRAIN_H_

#include <Base/Vector.h>
#include "RubyBindings.h"

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
    static VALUE Initialize(VALUE self, VALUE w, VALUE h, VALUE d);
    static VALUE GetTile(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE SetTile(VALUE self, VALUE x, VALUE y, VALUE z, VALUE type);
    static VALUE SurfaceTile(VALUE self, VALUE x, VALUE y);

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

    virtual short getTile(int x, int y, int z) = 0;
    virtual void setTile(int x, int y, int z, short type) = 0;
    virtual int getSurfaceLevel(int x, int y) = 0;
    
    virtual int getWidth() = 0;
    virtual int getHeight() = 0;
    virtual int getDepth() = 0;
};

#endif
