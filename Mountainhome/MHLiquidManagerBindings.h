/*
 *  MHLiquidManagerBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHLIQUIDMANAGERBINDINGS_H_
#define _MHLIQUIDMANAGERBINDINGS_H_
#include "RubyBindings.h"
#include "MHLiquidManager.h"

class MHLiquidManagerBindings : public RubyBindings<MHLiquidManager, false> {
public:
    static VALUE SetLiquid(VALUE self, VALUE x, VALUE y, VALUE z, VALUE type, VALUE volume);
    static VALUE GetLiquidType(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE GetLiquidVolume(VALUE self, VALUE x, VALUE y, VALUE z);

public:
    MHLiquidManagerBindings();
    virtual ~MHLiquidManagerBindings() {}

};

#endif
