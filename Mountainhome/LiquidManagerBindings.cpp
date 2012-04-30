/*
 *  LiquidManagerBindings.cpp
 *  Mountainhome
 *
 *  Created by datalurkur on 4/30/12.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "LiquidManagerBindings.h"
#include "LiquidManager.h"

#include "TerrainBindings.h"

LiquidManagerBindings::LiquidManagerBindings()
: RubyBindings<LiquidManager, false>(
    rb_define_class("LiquidManager", rb_cObject),
    "LiquidManagerBindings")
{
    rb_define_method(_class, "liquid_created", RUBY_METHOD_FUNC(LiquidManagerBindings::LiquidCreated), 3);
    rb_define_method(_class, "vacuum_created", RUBY_METHOD_FUNC(LiquidManagerBindings::VacuumCreated), 3);
    rb_define_method(_class, "setup", RUBY_METHOD_FUNC(LiquidManagerBindings::Setup), 1);
    rb_define_method(_class, "update", RUBY_METHOD_FUNC(LiquidManagerBindings::Update), 1);
    rb_define_method(_class, "each_new_liquid", RUBY_METHOD_FUNC(LiquidManagerBindings::EachNewLiquid), 0);
    rb_define_method(_class, "clear_new_liquids", RUBY_METHOD_FUNC(LiquidManagerBindings::ClearNewLiquids), 0);
}

VALUE LiquidManagerBindings::LiquidCreated(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    LiquidManager *cSelf = LiquidManagerBindings::Get()->getPointer(rSelf);
    Vector3 coords(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ));
    cSelf->liquidCreated(coords);
    return rSelf;
}

VALUE LiquidManagerBindings::VacuumCreated(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    LiquidManager *cSelf = LiquidManagerBindings::Get()->getPointer(rSelf);
    Vector3 coords(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ));
    cSelf->vacuumCreated(coords);
    return rSelf;
}

VALUE LiquidManagerBindings::Setup(VALUE rSelf, VALUE rTerrain) {
    LiquidManager *cSelf = LiquidManagerBindings::Get()->getPointer(rSelf);
    Terrain *terrain = TerrainBindings::Get()->getPointer(rTerrain);
    cSelf->setup(terrain);
    return rSelf;
}

VALUE LiquidManagerBindings::Update(VALUE rSelf, VALUE rElapsed) {
    LiquidManager *cSelf = LiquidManagerBindings::Get()->getPointer(rSelf);
    return(cSelf->update(NUM2INT(rElapsed)) ? Qtrue: Qfalse);
}

VALUE LiquidManagerBindings::EachNewLiquid(VALUE rSelf) {
    LiquidManager *cSelf = LiquidManagerBindings::Get()->getPointer(rSelf);

    LiquidManager::Vector3Set newLiquids;
    LiquidManager::Vector3Set::iterator itr;

    cSelf->getNewLiquids(newLiquids);
    for(itr = newLiquids.begin(); itr != newLiquids.end(); itr++) {
        VALUE array = rb_ary_new();
        rb_ary_push(array, INT2NUM((*itr).x));
        rb_ary_push(array, INT2NUM((*itr).y));
        rb_ary_push(array, INT2NUM((*itr).z));
        rb_yield(array);
    }

    return rSelf;
}

VALUE LiquidManagerBindings::ClearNewLiquids(VALUE rSelf) {
    LiquidManager *cSelf = LiquidManagerBindings::Get()->getPointer(rSelf);
    cSelf->clearNewLiquids();
    return rSelf;
}
