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
    rb_define_method(_class, "register_tile_material", RUBY_METHOD_FUNC(MHTerrainBindings::RegisterTileMaterial), 1);

    rb_define_method(_class, "get_tile_property", RUBY_METHOD_FUNC(MHTerrainBindings::GetTileProperty), 4);
    rb_define_method(_class, "set_tile_property", RUBY_METHOD_FUNC(MHTerrainBindings::SetTileProperty), 5);

    rb_define_method(_class, "get_tile_numeric_property", RUBY_METHOD_FUNC(MHTerrainBindings::GetTileNumericProperty), 4);
    rb_define_method(_class, "set_tile_numeric_property", RUBY_METHOD_FUNC(MHTerrainBindings::SetTileNumericProperty), 5);

    rb_define_method(_class, "get_tile_text_property", RUBY_METHOD_FUNC(MHTerrainBindings::GetTileTextProperty), 4);
    rb_define_method(_class, "set_tile_text_property", RUBY_METHOD_FUNC(MHTerrainBindings::SetTileTextProperty), 5);

    rb_define_method(_class, "tile_empty?", RUBY_METHOD_FUNC(MHTerrainBindings::IsTileEmpty), 3);
    rb_define_method(_class, "set_tile_empty", RUBY_METHOD_FUNC(MHTerrainBindings::SetTileEmpty), 3);

    rb_define_method(_class, "out_of_bounds?", RUBY_METHOD_FUNC(MHTerrainBindings::OutOfBounds), 3);
    rb_define_method(_class, "get_surface", RUBY_METHOD_FUNC(MHTerrainBindings::SurfaceTile), 2);
    rb_define_method(_class, "each_empty_range", RUBY_METHOD_FUNC(MHTerrainBindings::EachEmptyRange), 2);
    rb_define_method(_class, "clear", RUBY_METHOD_FUNC(MHTerrainBindings::Clear), 0);
    rb_define_method(_class, "width", RUBY_METHOD_FUNC(MHTerrainBindings::GetWidth), 0);
    rb_define_method(_class, "height", RUBY_METHOD_FUNC(MHTerrainBindings::GetHeight), 0);
    rb_define_method(_class, "depth", RUBY_METHOD_FUNC(MHTerrainBindings::GetDepth), 0);
    rb_define_method(_class, "poly_reduction=", RUBY_METHOD_FUNC(MHTerrainBindings::SetPolyReduction), 1);
    rb_define_method(_class, "auto_update=", RUBY_METHOD_FUNC(MHTerrainBindings::SetAutoUpdate), 1);
    
}

VALUE MHTerrainBindings::RegisterTileMaterial(VALUE rSelf, VALUE rMaterialName) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    std::string cMaterialName = rb_string_value_cstr(&rMaterialName);
    return INT2NUM(cSelf->getPalette()->registerTileMaterial(cMaterialName));
}

VALUE MHTerrainBindings::OutOfBounds(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    int cX = NUM2INT(x);
    int cY = NUM2INT(y);
    int cZ = NUM2INT(z);

    return cSelf->isOutOfBounds(cX, cY, cZ) ? Qtrue : Qfalse;
}

VALUE MHTerrainBindings::GetTileProperty(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE param) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return (cSelf->getTileProperty(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(param)))?
            Qtrue : Qfalse;
}

VALUE MHTerrainBindings::SetTileProperty(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE param, VALUE value) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    cSelf->setTileProperty(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(param), PropertyType(RTEST(value)));
    return rSelf;
}

VALUE MHTerrainBindings::GetTileNumericProperty(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE param) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getTileNumericProperty(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(param)));
}

VALUE MHTerrainBindings::SetTileNumericProperty(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE param, VALUE value) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    cSelf->setTileProperty(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(param), PropertyType((char)NUM2INT(value)));
    return rSelf;
}

VALUE MHTerrainBindings::GetTileTextProperty(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE param) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return rb_str_new2(cSelf->getTileTextProperty(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(param)).c_str());
}

VALUE MHTerrainBindings::SetTileTextProperty(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE param, VALUE value) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    std::string cString = rb_string_value_cstr(&value);
    cSelf->setTileProperty(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(param), PropertyType(cString));
    return rSelf;
}

VALUE MHTerrainBindings::IsTileEmpty(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return (cSelf->isTileEmpty(NUM2INT(x), NUM2INT(y), NUM2INT(z)))? Qtrue : Qfalse;
}

VALUE MHTerrainBindings::SetTileEmpty(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    cSelf->setTileEmpty(NUM2INT(x), NUM2INT(y), NUM2INT(z));
    return rSelf;
}

VALUE MHTerrainBindings::SurfaceTile(VALUE rSelf, VALUE x, VALUE y) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getSurfaceLevel(NUM2INT(x), NUM2INT(y)));
}

VALUE MHTerrainBindings::EachEmptyRange(VALUE rSelf, VALUE x, VALUE y) {
    MHTerrain *cSelf = Get()->getPointer(rSelf);
    std::vector<std::pair<int,int> > ranges;

    int numRanges = cSelf->getEmptyRanges(NUM2INT(x), NUM2INT(y), ranges);

    std::vector<std::pair<int,int> >::iterator itr;
    for(itr = ranges.begin(); itr != ranges.end(); itr++) {
        rb_yield(rb_ary_new3(2, INT2NUM((*itr).first), INT2NUM((*itr).second)));
    }

    return INT2NUM(numRanges);
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
