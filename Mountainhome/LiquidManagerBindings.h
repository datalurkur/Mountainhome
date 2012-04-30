/*
 *  LiquidManagerBindings.h
 *  Mountainhome
 *
 *  Created by datalurkur on 4/30/12.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _LIQUIDMANAGERBINDINGS_H_
#define _LIQUIDMANAGERBINDINGS_H_
#include "RubyBindings.h"
#include "LiquidManager.h"

class LiquidManagerBindings : public RubyBindings<LiquidManager, false> {
public:
    /*! Creates the LiquidManager bindings. */
    static void SetupBindings();

    static VALUE LiquidCreated(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE VacuumCreated(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE Setup(VALUE self, VALUE terrain);
    static VALUE Update(VALUE self, VALUE elapsed);

    // Used for processing newly created liquid tiles
    static VALUE EachNewLiquid(VALUE self);
    static VALUE ClearNewLiquids(VALUE self);

public:
    LiquidManagerBindings();
    virtual ~LiquidManagerBindings() {}
};

#endif
