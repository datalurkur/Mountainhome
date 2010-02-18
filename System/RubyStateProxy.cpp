/*
 *  RubyStateProxy.cpp
 *  System
 *
 *  Created by loch on 2/18/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RubyStateProxy.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark StateObject ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
VALUE RubyStateProxy::TeardownMethod = NULL;
VALUE RubyStateProxy::SetupMethod    = NULL;
VALUE RubyStateProxy::UpdateMethod   = NULL;
VALUE RubyStateProxy::Setup(VALUE self) { return self; }
VALUE RubyStateProxy::Teardown(VALUE self) { return self; }
VALUE RubyStateProxy::Initialize(VALUE self) {
    RubyStateProxy::RegisterObject(self, new RubyStateProxy(self));
    return self;
}

void RubyStateProxy::SetupBindings() {
    TeardownMethod = rb_intern("teardown");
    UpdateMethod   = rb_intern("update");
    SetupMethod    = rb_intern("setup");

    Class = rb_define_class("State", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(RubyStateProxy::Initialize), 0);
    rb_define_method(Class, "teardown", RUBY_METHOD_FUNC(RubyStateProxy::Teardown), 0);
    rb_define_method(Class, "setup", RUBY_METHOD_FUNC(RubyStateProxy::Setup), 0);
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark StateObject implementation
//////////////////////////////////////////////////////////////////////////////////////////
RubyStateProxy::RubyStateProxy(VALUE robj): _rubyObject(robj) {}
RubyStateProxy::~RubyStateProxy() {}

void RubyStateProxy::update(int elapsed) {
    rb_funcall(_rubyObject, UpdateMethod, 1, INT2FIX(elapsed));
}

void RubyStateProxy::setup(va_list args) {
    //\todo This needs to read out the argument list and pass it down to ruby.
    rb_funcall(_rubyObject, SetupMethod, 0);
}

void RubyStateProxy::teardown() {
    rb_funcall(_rubyObject, TeardownMethod, 0);
}
