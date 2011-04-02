/*
 * RenderableBindings.h
 * Mountainhome
 *
 * Created by datalurkur on 3/30/11.
 * Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RENDERABLEBINDINGS_H_
#define _RENDERABLEBINDINGS_H_
#include "RubyBindings.h"
#include "Renderable.h"

class RenderableBindings : public RubyBindings<Renderable, true> {
public:
    static VALUE SetTranslationMatrix(VALUE self, VALUE x, VALUE y, VALUE z);

public:
    RenderableBindings();
    virtual ~RenderableBindings() {}
};

#endif
