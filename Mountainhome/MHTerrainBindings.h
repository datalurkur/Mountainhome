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
    static VALUE RegisterTileMaterial(VALUE rSelf, VALUE rMaterialName);

    static VALUE GetTileProperty(VALUE self, VALUE x, VALUE y, VALUE z, VALUE property);
    static VALUE SetTileProperty(VALUE self, VALUE x, VALUE y, VALUE z, VALUE property, VALUE value);
    static VALUE GetTileNumericProperty(VALUE self, VALUE x, VALUE y, VALUE z, VALUE property);
    static VALUE SetTileNumericProperty(VALUE self, VALUE x, VALUE y, VALUE z, VALUE property, VALUE value);
    static VALUE GetTileTextProperty(VALUE self, VALUE x, VALUE y, VALUE z, VALUE property);
    static VALUE SetTileTextProperty(VALUE self, VALUE x, VALUE y, VALUE z, VALUE property, VALUE value);
    static VALUE IsTileEmpty(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE SetTileEmpty(VALUE self, VALUE x, VALUE y, VALUE z);

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
