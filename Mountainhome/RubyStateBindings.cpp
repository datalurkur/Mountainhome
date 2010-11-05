/*
 *  RubyStateBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RubyStateBindings.h"

#include "RubyState.h"
#include <Base/Logger.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark StateObject ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
VALUE RubyStateBindings::Alloc(VALUE klass) {
    RubyState *cState = new RubyState();
    VALUE rState = NEW_RUBY_OBJECT_FULL(RubyStateBindings, cState, klass);
    cState->_rubyObject = rState;
    return rState;
}

RubyStateBindings::RubyStateBindings()
: RubyBindings<RubyState, false>(
    rb_define_class("MHState", rb_cObject),
    "RubyStateBindings")
{
    rb_define_alloc_func(_class, RubyStateBindings::Alloc);
}
