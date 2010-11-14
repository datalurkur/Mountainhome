/*
 *  MHTerrainBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHTERRAINBINDINGS_H_
#define _MHTERRAINBINDINGS_H_
#include "RubyBindings.h"
#include "MHTerrain.h"

class MHTerrainBindings : public RubyBindings<MHTerrain, false> {
public:
    static VALUE RegisterTileType(VALUE rSelf, VALUE rMaterialName);

    static VALUE GetTileType(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE GetTileParameter(VALUE self, VALUE x, VALUE y, VALUE z, VALUE parameter);

    static VALUE SetTileType(VALUE self, VALUE x, VALUE y, VALUE z, VALUE type);
    static VALUE SetTileParameter(VALUE self, VALUE x, VALUE y, VALUE z, VALUE parameter, VALUE value);

    static VALUE OutOfBounds(VALUE rSelf, VALUE x, VALUE y, VALUE z);
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

public:
    MHTerrainBindings();
    virtual ~MHTerrainBindings() {}

};

#endif
