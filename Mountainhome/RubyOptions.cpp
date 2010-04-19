/*
 *  RubyOptions.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RubyOptions.h"

void RubyOptions::SetupBindings() {
    Class = rb_define_class("MHOptions", rb_cObject);
    rb_define_method(Class, "apply", RUBY_METHOD_FUNC(RubyOptions::Apply), 0);
}

VALUE RubyOptions::Apply(VALUE rSelf) {
    AssignCObjFromValue(OptionsModule, cSelf, rSelf);
    cSelf->apply();

    return rSelf;
}