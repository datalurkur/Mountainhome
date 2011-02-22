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
	rb_define_method(_class, "ratio",  RUBY_METHOD_FUNC(WindowBindings::Ratio), 0);
	rb_define_method(_class, "width",  RUBY_METHOD_FUNC(WindowBindings::Width), 0);
	rb_define_method(_class, "height", RUBY_METHOD_FUNC(WindowBindings::Height), 0);
}

VALUE WindowBindings::Ratio(VALUE rSelf) {
    Window *cSelf = Get()->getPointer(rSelf);
    return DBL2NUM(cSelf->getWidth() / (float)cSelf->getHeight());
}

VALUE WindowBindings::Width(VALUE rSelf) {
    Window *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getWidth());
}

VALUE WindowBindings::Height(VALUE rSelf) {
    Window *cSelf = Get()->getPointer(rSelf);
    return INT2NUM(cSelf->getHeight());
}
