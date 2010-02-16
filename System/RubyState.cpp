/*
 *  RubyState.cpp
 *  System
 *
 *  Created by loch on 2/15/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RubyState.h"

VALUE RubyState::Class          = NULL;
VALUE RubyState::UpdateMethod   = NULL;
VALUE RubyState::TeardownMethod = NULL;
VALUE RubyState::SetupMethod    = NULL;
RubyState::RubyObjectMap RubyState::Objects;

void RubyState::SetupBindings() {
    TeardownMethod = rb_intern("teardown");
    UpdateMethod   = rb_intern("update");
    SetupMethod    = rb_intern("setup");

    Class = rb_define_class("State", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(RubyState::Initialize), 0);
    rb_define_method(Class, "teardown", RUBY_METHOD_FUNC(RubyState::Teardown), 0);
    rb_define_method(Class, "setup", RUBY_METHOD_FUNC(RubyState::Setup), 0);
}

RubyState* RubyState::GetState(VALUE robj) {
    if (Objects.find(robj) == Objects.end()) {
        THROW(InternalError, "RubyState does not exist for " << robj << "!");
    }

    return RubyState::Objects[robj];
}

VALUE RubyState::Setup(VALUE self) { return self; }
VALUE RubyState::Teardown(VALUE self) { return self; }
VALUE RubyState::Initialize(VALUE self) {
    if (Objects.find(self) != Objects.end()) {
        THROW(DuplicateItemError, "RubyState already exists for " << self << "!");
    }

    Objects[self] = new RubyState(self);
    return self;
}

RubyState::RubyState(VALUE robj): _rubyObject(robj) {}
RubyState::~RubyState() {}

void RubyState::update(int elapsed) {
    rb_funcall(_rubyObject, UpdateMethod, 1, INT2FIX(elapsed));
}

void RubyState::setup(va_list args) {
    //\todo This needs to read out the argument list and pass it down to ruby.
    rb_funcall(_rubyObject, SetupMethod, 0);
}

void RubyState::teardown() {
    rb_funcall(_rubyObject, TeardownMethod, 0);
}
