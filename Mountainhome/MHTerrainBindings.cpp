/*
 *  MHTerrainBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHTerrainBindings.h"

MHTerrainBindings::MHTerrainBindings()
: RubyBindings<MHTerrain, false>(
    rb_define_class("MHTerrain", rb_cObject),
    "MHTerrainBindings")
{
    rb_define_method(_class, "register_tile_type", RUBY_METHOD_FUNC(MHTerrainBindings::RegisterTileType), 1);
    rb_define_method(_class, "get_tile_type", RUBY_METHOD_FUNC(MHTerrainBindings::GetTileType), 3);
    rb_define_method(_class, "get_tile_parameter", RUBY_METHOD_FUNC(MHTerrainBindings::GetTileParameter), 4);
    rb_define_method(_class, "set_tile_type", RUBY_METHOD_FUNC(MHTerrainBindings::SetTileType), 4);
    rb_define_method(_class, "set_tile_parameter", RUBY_METHOD_FUNC(MHTerrainBindings::SetTileParameter), 5);
    rb_define_method(_class, "out_of_bounds?", RUBY_METHOD_FUNC(MHTerrainBindings::OutOfBounds), 3);
    rb_define_method(_class, "get_surface", RUBY_METHOD_FUNC(MHTerrainBindings::SurfaceTile), 2);
    rb_define_method(_class, "clear", RUBY_METHOD_FUNC(MHTerrainBindings::Clear), 0);
    rb_define_method(_class, "width", RUBY_METHOD_FUNC(MHTerrainBindings::GetWidth), 0);
    rb_define_method(_class, "height", RUBY_METHOD_FUNC(MHTerrainBindings::GetHeight), 0);
    rb_define_method(_class, "depth", RUBY_METHOD_FUNC(MHTerrainBindings::GetDepth), 0);
    rb_define_method(_class, "poly_reduction=", RUBY_METHOD_FUNC(MHTerrainBindings::SetPolyReduction), 1);
    rb_define_method(_class, "auto_update=", RUBY_METHOD_FUNC(MHTerrainBindings::SetAutoUpdate), 1);
    
}

VALUE MHTerrainBindings::RegisterTileType(VALUE rSelf, VALUE rMaterialName) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    std::string cMaterialName = rb_string_value_cstr(&rMaterialName);
    return INT2NUM(cSelf->registerTileType(cMaterialName));
}

VALUE MHTerrainBindings::OutOfBounds(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    int cX = NUM2INT(x);
    int cY = NUM2INT(y);
    int cZ = NUM2INT(z);

    return (cX >= 0 && cX < cSelf->getWidth() &&
            cY >= 0 && cY < cSelf->getWidth() &&
            cZ >= 0 && cZ < cSelf->getWidth()) ?
            Qfalse : Qtrue;
}

VALUE MHTerrainBindings::GetTileType(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getTileType(NUM2INT(x), NUM2INT(y), NUM2INT(z)));
}

VALUE MHTerrainBindings::GetTileParameter(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE param) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return (cSelf->getTileParameter(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(param)))?
            Qtrue : Qfalse;
}

VALUE MHTerrainBindings::SetTileType(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE type) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    cSelf->setTileType(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(type));
    return rSelf;
}

VALUE MHTerrainBindings::SetTileParameter(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE param, VALUE value) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    cSelf->setTileParameter(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(param), RTEST(value));
    return rSelf;
}

VALUE MHTerrainBindings::SurfaceTile(VALUE rSelf, VALUE x, VALUE y) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getSurfaceLevel(NUM2INT(x), NUM2INT(y)));
}

VALUE MHTerrainBindings::Clear(VALUE rSelf) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    cSelf->clear();
    return rSelf;
}

VALUE MHTerrainBindings::GetWidth(VALUE rSelf) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getWidth());
}

VALUE MHTerrainBindings::GetHeight(VALUE rSelf) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getHeight());
}

VALUE MHTerrainBindings::GetDepth(VALUE rSelf) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getDepth());
}

VALUE MHTerrainBindings::SetPolyReduction(VALUE rSelf, VALUE val) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    ///\todo XXXBMW: I swear there is a macro that checks both of these automatically.
    cSelf->setPolyReduction(val != Qnil && val != Qfalse);
    return Qnil;
}

VALUE MHTerrainBindings::SetAutoUpdate(VALUE rSelf, VALUE val) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    ///\todo XXXBMW: I swear there is a macro that checks both of these automatically.
    cSelf->setAutoUpdate(val != Qnil && val != Qfalse);
    return Qnil;
}
