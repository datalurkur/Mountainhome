/*
 *  RubyWindow.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RubyWindow.h"
#include "RubyViewport.h"

///\todo What if something in C removes the viewports from the window?! It could do ALL sorts of scary things...

void RubyWindow::SetupBindings() {
    Class = rb_define_class("MHWindow", rb_cObject);
    rb_define_method(Class, "add_viewport", RUBY_METHOD_FUNC(RubyWindow::AddViewport), 5);
    rb_define_method(Class, "set_bg_color", RUBY_METHOD_FUNC(RubyWindow::SetBGColor), 3);
}

void RubyWindow::Mark(Window *cWindow) {
    const RenderTarget::ViewportMap viewports = cWindow->getViewports();
    RenderTarget::ViewportMap::const_iterator itr = viewports.begin();
    for(; itr != viewports.end(); itr++) {
        rb_gc_mark(RubyViewport::GetValue(itr->second));
    }
}

VALUE RubyWindow::AddViewport(VALUE self, VALUE zLevel, VALUE x, VALUE y, VALUE w, VALUE z) {
    AssignCObjFromValue(Window, cWindow, self);
    Viewport *cViewport = cWindow->addViewport(NUM2INT(zLevel), NUM2DBL(x), NUM2DBL(y), NUM2DBL(w), NUM2DBL(z));
    VALUE rViewport = CreateBindingPair(RubyViewport, cViewport);
    return rViewport;
}

VALUE RubyWindow::SetBGColor(VALUE rSelf, VALUE r, VALUE g, VALUE b) {
    AssignCObjFromValue(Window, cSelf, rSelf);
    cSelf->setBGColor(Color4(NUM2DBL(r), NUM2DBL(g), NUM2DBL(b), 1));
    return rSelf;
}
