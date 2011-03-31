/*
 *  RenderContextBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RenderContextBindings.h"
#include "RenderableBindings.h"

RenderContextBindings::RenderContextBindings()
: RubyBindings<RenderContext, false>(
    rb_define_class("MHRenderContext", rb_cObject),
    "RenderContextBindings")
{
    rb_define_method(_class, "set_wireframe", RUBY_METHOD_FUNC(RenderContextBindings::SetWireframe), 1);
    rb_define_method(_class, "set_viewport", RUBY_METHOD_FUNC(RenderContextBindings::SetViewport), 4);
    rb_define_method(_class, "clear", RUBY_METHOD_FUNC(RenderContextBindings::Clear), 4);
    rb_define_method(_class, "render_2d", RUBY_METHOD_FUNC(RenderContextBindings::Render2D), 3);
}

VALUE RenderContextBindings::SetViewport(VALUE rSelf, VALUE x, VALUE y, VALUE w, VALUE h) {
    RenderContext *cSelf = Get()->getPointer(rSelf);
    cSelf->setViewport(Viewport(NUM2DBL(x), NUM2DBL(y), NUM2DBL(w), NUM2DBL(h)));
    return Qnil;
}

VALUE RenderContextBindings::Clear(VALUE rSelf, VALUE r, VALUE g, VALUE b, VALUE a) {
    RenderContext *cSelf = Get()->getPointer(rSelf);
    cSelf->clear(Color4(NUM2DBL(r), NUM2DBL(g), NUM2DBL(b), NUM2DBL(a)));
    return Qnil;
}

VALUE RenderContextBindings::SetWireframe(VALUE rSelf, VALUE val) {
    RenderContext *cSelf = Get()->getPointer(rSelf);
    cSelf->setWireframe(val != Qfalse && val != Qnil);
    return val;
}

VALUE RenderContextBindings::Render2D(VALUE rSelf, VALUE rWidth, VALUE rHeight, VALUE rRenderables) {
    RenderContext *cSelf = Get()->getPointer(rSelf);

    // Construct a renderable list using the references provided by UIManager
    RenderableList list;
    VALUE rRenderable;
    while((rRenderable = rb_ary_shift(rRenderables)) != Qnil) {
        Renderable *cRenderable = RenderableBindings::Get()->getPointer(rRenderable);
        list.push_back(cRenderable);
    }

    cSelf->render2D(NUM2INT(rWidth), NUM2INT(rHeight), list);
    return rSelf;
}
