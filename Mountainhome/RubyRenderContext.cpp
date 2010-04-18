/*
 *  RubyRenderContext.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RubyRenderContext.h"

void RubyRenderContext::SetupBindings() {
    Class = rb_define_class("MHRenderContext", rb_cObject);
    rb_define_method(Class, "set_filled", RUBY_METHOD_FUNC(RubyRenderContext::SetFilled), 0);
    rb_define_method(Class, "set_wireframe", RUBY_METHOD_FUNC(RubyRenderContext::SetWireframe), 0);
}

VALUE RubyRenderContext::SetFilled(VALUE rSelf) {
    AssignCObjFromValue(RenderContext, cSelf, rSelf);
    cSelf->setFilled();
    return rSelf;
}

VALUE RubyRenderContext::SetWireframe(VALUE rSelf) {
    AssignCObjFromValue(RenderContext, cSelf, rSelf);
    cSelf->setWireFrame();
    return rSelf;
}
