/*
 *  MHUIManagerBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHUIMANAGERBINDINGS_H_
#define _MHUIMANAGERBINDINGS_H_
#include "RubyBindings.h"
#include "MHUIManager.h"

class MHUIManagerBindings : public RubyBindings<MHUIManager, true> {
public:
    static void Mark(MHUIManager *cSelf);

    static VALUE SetRoot(VALUE self, VALUE element);
    static VALUE GetRoot(VALUE self);

    static VALUE GetHeight(VALUE self);
    static VALUE GetWidth(VALUE self);

    static VALUE Render(VALUE self, VALUE context);

public:
    MHUIManagerBindings();
    virtual ~MHUIManagerBindings() {}

};

#endif
