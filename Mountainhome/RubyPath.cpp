/*
 *  RubyPath.cpp
 *  Mountainhome
 *
 *  Created by Andrew on 7/16/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RubyPath.h"

void RubyPath::SetupBindings() {
    Class = rb_define_class("MHPath", rb_cObject);
	
    rb_define_method(Class, "next_step", RUBY_METHOD_FUNC(RubyPath::NextStep), 0);
}

VALUE RubyPath::NextStep(VALUE rSelf) {
    AssignCObjFromValue(Path, cSelf, rSelf);
    Vector3 cStep = cSelf->getNextStep();

    VALUE *coords = new VALUE[3];
    coords[0] = INT2NUM(cStep[0]);
    coords[1] = INT2NUM(cStep[1]);
    coords[2] = INT2NUM(cStep[2]);

    return rb_ary_new4(3, coords);
}
