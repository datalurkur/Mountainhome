/*
 *  LiquidSystemBindings.h
 *  Mountainhome
 *
 *  Created by datalurkur on 4/25/12.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _LIQUIDSYSTEMBINDINGS_H_
#define _LIQUIDSYSTEMBINDINGS_H_
#include "RubyBindings.h"
#include "LiquidSystem.h"

class LiquidSystemBindings : public RubyBindings<LiquidSystem, false> {
public:
    static VALUE Setup(VALUE self);

    static VALUE ProcessLiquid(VALUE self, VALUE x, VALUE y, VALUE z, VALUE offset);
    static VALUE ProcessVacuum(VALUE self, VALUE x, VALUE y, VALUE z, VALUE offset);

public:
    LiquidSystemBindings();
    virtual ~LiquidSystemBindings() {}
};

#endif
