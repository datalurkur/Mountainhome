/*
 *  WindowBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "WindowBindings.h"
#include "ViewportBindings.h"

///\todo What if something in C removes the viewports from the window?! It could do ALL sorts of scary things...

WindowBindings::WindowBindings()
: RubyBindings<Window, false>(
    rb_define_class("MHWindow", rb_cObject),
    "WindowBindings")
{
    rb_define_method(_class, "clear_viewports", RUBY_METHOD_FUNC(WindowBindings::ClearViewports), 0);
    rb_define_method(_class, "add_viewport", RUBY_METHOD_FUNC(WindowBindings::AddViewport), 5);
    rb_define_method(_class, "set_bg_color", RUBY_METHOD_FUNC(WindowBindings::SetBGColor), 3);
}

void WindowBindings::Mark(Window *cWindow) {
    const RenderTarget::ViewportMap viewports = cWindow->getViewports();
    RenderTarget::ViewportMap::const_iterator itr = viewports.begin();
    for(; itr != viewports.end(); itr++) {
        rb_gc_mark(ViewportBindings::Get()->getValue(itr->second));
    }
}

VALUE WindowBindings::ClearViewports(VALUE rSelf) {
    Window *cSelf = Get()->getPointer(rSelf);
    cSelf->removeAllViewports();
    return rSelf;
}

VALUE WindowBindings::AddViewport(VALUE rSelf, VALUE zLevel, VALUE x, VALUE y, VALUE w, VALUE z) {
    Window *cSelf = Get()->getPointer(rSelf);
    Viewport *cViewport = cSelf->addViewport(NUM2INT(zLevel), NUM2DBL(x), NUM2DBL(y), NUM2DBL(w), NUM2DBL(z));
    VALUE rViewport = NEW_RUBY_OBJECT(ViewportBindings, cViewport);
    return rViewport;
}

VALUE WindowBindings::SetBGColor(VALUE rSelf, VALUE r, VALUE g, VALUE b) {
    Window *cSelf = Get()->getPointer(rSelf);
    cSelf->setBGColor(Color4(NUM2DBL(r), NUM2DBL(g), NUM2DBL(b), 1));
    return rSelf;
}
