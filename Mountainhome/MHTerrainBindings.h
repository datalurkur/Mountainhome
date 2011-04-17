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
    static VALUE IsTileEmpty(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE SetTileEmpty(VALUE self, VALUE x, VALUE y, VALUE z);

    static VALUE SetTile(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rTile);
    static VALUE GetTile(VALUE rSelf, VALUE x, VALUE y, VALUE z);
    static VALUE SetTileParameter(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rParameter, VALUE rParamValue);
    static VALUE GetTileParameter(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE rParameter);

    static VALUE OutOfBounds(VALUE self, VALUE x, VALUE y, VALUE z);

    static VALUE SurfaceTile(VALUE self, VALUE x, VALUE y);
    static VALUE EachEmptyRange(VALUE self, VALUE x, VALUE y);
    static VALUE EachFilledRange(VALUE self, VALUE x, VALUE y);

    static VALUE Clear(VALUE self);

    /*! Gets the world's width. */
    static VALUE GetWidth(VALUE self);

    /*! Gets the world's height. */
    static VALUE GetHeight(VALUE self);

    /*! Gets the world's depth. */
    static VALUE GetDepth(VALUE self);

    static VALUE SetPolyReduction(VALUE self, VALUE val);

    static VALUE SetAutoUpdate(VALUE self, VALUE val);

protected:
    static ID parametersKey;

public:
    MHTerrainBindings();
    virtual ~MHTerrainBindings() {}

private:
    static bool convertRubyParameter(VALUE rParameter, ParameterData &cParameter);
    static bool convertCParameter(const ParameterData &cParameter, VALUE &rParameter);
};

#endif
