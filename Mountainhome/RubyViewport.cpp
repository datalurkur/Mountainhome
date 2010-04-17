/*
 *  RubyViewport.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RubyViewport.h"
#include <Render/Viewport.h>

void RubyViewport::SetupBindings() {
    Class = rb_define_class("MHViewport", rb_cObject);
    rb_define_method(Class, "add_source", RUBY_METHOD_FUNC(RubyViewport::AddSource), 2);
}

VALUE RubyViewport::AddSource(VALUE rSelf, VALUE rSource, VALUE zLevel) {
    AssignCObjFromValue(RenderSource, cSource, rSource);
    AssignCObjFromValue(Viewport, cSelf, rSelf);
    cSelf->addSource(cSource, NUM2INT(zLevel));
    return rSelf;
}
