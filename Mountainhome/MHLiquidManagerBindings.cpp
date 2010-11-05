/*
 *  MHLiquidManagerBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHLiquidManagerBindings.h"

MHLiquidManagerBindings::MHLiquidManagerBindings()
: RubyBindings<MHLiquidManager, false>(
    rb_define_class("MHLiquidManager", rb_cObject),
    "MHLiquidManagerBindings")
{
    rb_define_method(_class, "set_liquid", RUBY_METHOD_FUNC(MHLiquidManagerBindings::SetLiquid), 5);
    rb_define_method(_class, "get_liquid_type", RUBY_METHOD_FUNC(MHLiquidManagerBindings::GetLiquidType), 3);
    rb_define_method(_class, "get_liquid_volume", RUBY_METHOD_FUNC(MHLiquidManagerBindings::GetLiquidVolume), 3);
}

VALUE MHLiquidManagerBindings::SetLiquid(VALUE rSelf, VALUE x, VALUE y, VALUE z, VALUE type, VALUE volume) {
    MHLiquidManager *cSelf = Get()->getPointer(rSelf);
    cSelf->setLiquid(NUM2INT(x), NUM2INT(y), NUM2INT(z), NUM2INT(type), NUM2DBL(volume));

    return rSelf;
}

VALUE MHLiquidManagerBindings::GetLiquidType(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHLiquidManager *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getLiquidType(NUM2INT(x), NUM2INT(y), NUM2INT(z)));
}

VALUE MHLiquidManagerBindings::GetLiquidVolume(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    MHLiquidManager *cSelf = Get()->getPointer(rSelf);
    return DBL2NUM(cSelf->getLiquidVolume(NUM2INT(x), NUM2INT(y), NUM2INT(z)));
}
