/*
 *  MHLiquidManager.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHLiquidManager.h"

MHLiquidManager::MHLiquidManager() {}
MHLiquidManager::~MHLiquidManager() {}

void MHLiquidManager::SetupBindings() {
    Class = rb_define_class("MHLiquidManager", rb_cObject);
    rb_define_method(Class, "set_liquid", RUBY_METHOD_FUNC(MHLiquidManager::SetLiquid), 5);
    rb_define_method(Class, "get_liquid_type", RUBY_METHOD_FUNC(MHLiquidManager::GetLiquidType), 3);
    rb_define_method(Class, "get_liquid_volume", RUBY_METHOD_FUNC(MHLiquidManager::GetLiquidVolume), 3);
}

VALUE MHLiquidManager::SetLiquid(VALUE self, VALUE x, VALUE y, VALUE z, VALUE type, VALUE volume) {
    AssignCObjFromValue(MHLiquidManager, cSelf, self);
    cSelf->setLiquid(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(type), NUM2DBL(volume));

    return self;
}

VALUE MHLiquidManager::GetLiquidType(VALUE self, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(MHLiquidManager, cSelf, self);
    return INT2NUM(cSelf->getLiquidType(NUM2INT(x), NUM2INT(y), NUM2INT(z)));
}

VALUE MHLiquidManager::GetLiquidVolume(VALUE self, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(MHLiquidManager, cSelf, self);
    return DBL2NUM(cSelf->getLiquidVolume(NUM2INT(x), NUM2INT(y), NUM2INT(z)));
}
