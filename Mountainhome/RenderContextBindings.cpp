/*
 *  RenderContextBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RenderContextBindings.h"

RenderContextBindings::RenderContextBindings()
: RubyBindings<RenderContext, false>(
    rb_define_class("MHRenderContext", rb_cObject),
    "RenderContextBindings")
{
    rb_define_method(_class, "set_filled", RUBY_METHOD_FUNC(RenderContextBindings::SetFilled), 0);
    rb_define_method(_class, "set_wireframe", RUBY_METHOD_FUNC(RenderContextBindings::SetWireframe), 0);
}

VALUE RenderContextBindings::SetFilled(VALUE rSelf) {
    RenderContext *cSelf = Get()->getPointer(rSelf);
    cSelf->setFilled();
    return rSelf;
}

VALUE RenderContextBindings::SetWireframe(VALUE rSelf) {
    RenderContext *cSelf = Get()->getPointer(rSelf);
    cSelf->setWireFrame();
    return rSelf;
}
