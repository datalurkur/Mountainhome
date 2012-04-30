/*
 *  LiquidSystemBindings.cpp
 *  Mountainhome
 *
 *  Created by datalurkur on 4/25/12.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "LiquidSystemBindings.h"
#include "LiquidSystem.h"

LiquidSystemBindings::LiquidSystemBindings()
: RubyBindings<LiquidSystem, false>(
    rb_define_class("LiquidSystem", rb_cObject),
    "LiquidSystemBindings")
{
    rb_define_method(_class, "setup", RUBY_METHOD_FUNC(LiquidSystemBindings::Setup), 0);
    
    rb_define_method(_class, "process_liquid", RUBY_METHOD_FUNC(LiquidSystemBindings::ProcessLiquid), 4);
    rb_define_method(_class, "process_vacuum", RUBY_METHOD_FUNC(LiquidSystemBindings::ProcessVacuum), 4);
}

VALUE LiquidSystemBindings::Setup(VALUE rSelf) {
    LiquidSystem *cSelf = LiquidSystemBindings::Get()->getPointer(rSelf);
    cSelf->setup();
    return rSelf;
}

VALUE LiquidSystemBindings::ProcessLiquid(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ, VALUE rOffset) {
    LiquidSystem *cSelf = LiquidSystemBindings::Get()->getPointer(rSelf);
    Vector3 coords(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ));
    cSelf->processLiquid(coords);
    return rSelf;
}

VALUE LiquidSystemBindings::ProcessVacuum(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ, VALUE rOffset) {
    LiquidSystem *cSelf = LiquidSystemBindings::Get()->getPointer(rSelf);
    Vector3 coords(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ));
    cSelf->processVacuum(coords);
    return rSelf;
}