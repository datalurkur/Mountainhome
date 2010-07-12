/*
 *  MHTerrain.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHTerrain.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHTerrain ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHTerrain::SetupBindings() {
    Class = rb_define_class("MHTerrain", rb_cObject);
    rb_define_method(Class, "set_tile", RUBY_METHOD_FUNC(MHTerrain::SetTile), 4);
    rb_define_method(Class, "get_tile", RUBY_METHOD_FUNC(MHTerrain::GetTile), 3);
    rb_define_method(Class, "get_surface", RUBY_METHOD_FUNC(MHTerrain::SurfaceTile), 2);
    rb_define_method(Class, "clear", RUBY_METHOD_FUNC(MHTerrain::Clear), 0);
    rb_define_method(Class, "width", RUBY_METHOD_FUNC(MHTerrain::GetWidth), 0);
    rb_define_method(Class, "height", RUBY_METHOD_FUNC(MHTerrain::GetHeight), 0);
    rb_define_method(Class, "depth", RUBY_METHOD_FUNC(MHTerrain::GetDepth), 0);
}

VALUE MHTerrain::GetTile(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    return INT2NUM(cSelf->getTile(NUM2INT(x), NUM2INT(y), NUM2INT(z)));
}

VALUE MHTerrain::SetTile(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE type) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    cSelf->setTile(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(type));
    return rSelf;
}

VALUE MHTerrain::SurfaceTile(VALUE rSelf, VALUE x, VALUE y) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    return INT2NUM(cSelf->getSurfaceLevel(NUM2INT(x), NUM2INT(y)));
}

VALUE MHTerrain::Clear(VALUE rSelf) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    cSelf->clear();
    return rSelf;
}

VALUE MHTerrain::GetWidth(VALUE rSelf) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    return INT2NUM(cSelf->getWidth());
}

VALUE MHTerrain::GetHeight(VALUE rSelf) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    return INT2NUM(cSelf->getHeight());
}

VALUE MHTerrain::GetDepth(VALUE rSelf) {
    AssignCObjFromValue(MHTerrain, cSelf, rSelf);
    return INT2NUM(cSelf->getDepth());
}
