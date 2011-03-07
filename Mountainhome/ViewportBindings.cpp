/*
 *  ViewportBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ViewportBindings.h"
#include <Render/Viewport.h>

ViewportBindings::ViewportBindings()
: RubyBindings<Viewport, false>(
    rb_define_class("MHViewport", rb_cObject),
    "ViewportBindings")
{}
