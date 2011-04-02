/*
 *  RenderContextBindings.h
 *  Mountainhome
 *
 *  Created by loch on 4/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RENDERCONTEXTBINDINGS_H_
#define _RENDERCONTEXTBINDINGS_H_
#include <Render/RenderContext.h>
#include "RubyBindings.h"

class RenderContextBindings : public RubyBindings<RenderContext, false> {
public:
    static VALUE SetViewport(VALUE self, VALUE x, VALUE y, VALUE w, VALUE h);

    static VALUE Clear(VALUE self, VALUE r, VALUE g, VALUE b, VALUE a);

    static VALUE SetWireframe(VALUE self, VALUE val);

    static VALUE Render2D(VALUE self, VALUE width, VALUE height, VALUE renderables);

public:
    RenderContextBindings();
    virtual ~RenderContextBindings() {}

};

#endif
