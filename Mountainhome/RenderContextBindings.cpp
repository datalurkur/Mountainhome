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
}
