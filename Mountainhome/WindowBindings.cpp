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
{}
