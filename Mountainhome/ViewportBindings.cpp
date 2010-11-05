/*
 *  ViewportBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RenderSourceBindings.h"
#include "ViewportBindings.h"
#include <Render/Viewport.h>

ViewportBindings::ViewportBindings()
: RubyBindings<Viewport, false>(
    rb_define_class("MHViewport", rb_cObject),
    "ViewportBindings")
{
    rb_define_method(_class, "add_source", RUBY_METHOD_FUNC(ViewportBindings::AddSource), 2);
}

VALUE ViewportBindings::AddSource(VALUE rSelf, VALUE rSource, VALUE zLevel) {
    RenderSource *cSource = RenderSourceBindings::Get()->getPointer(rSource);
    Viewport *cSelf = Get()->getPointer(rSelf);
    cSelf->addSource(cSource, NUM2INT(zLevel));
    return rSelf;
}
