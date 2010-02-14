/*
 *  Bindings.cpp
 *  Mountainhome
 *
 *  Created by Brent Wilson on 2/13/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "Bindings.h"
#include "Mountainhome.h"

VALUE MountainhomeBinding::RubyClass = NULL;
VALUE MountainhomeBinding::RubyObject = NULL;

void MountainhomeBinding::Setup() {
    RubyClass = rb_define_class("Mountainhome", rb_cObject);
    rb_define_method(RubyClass, "state=", RUBY_METHOD_FUNC(MountainhomeBinding::StopMainLoop), 1);
    rb_define_method(RubyClass, "exit", RUBY_METHOD_FUNC(MountainhomeBinding::SetState), 0);
#if 0
    rb_include_module(RubyClass, rb_intern("Singleton"));
    RubyObject = rb_funcall(RubyClass, rb_intern("instance"), 0);
#else
    RubyObject = rb_class_new_instance(NULL, 0, RubyClass);
#endif
#if 0
    rb_define_variable("$mountainhome", RubyObject);
#else
    rb_gv_set("$mountainhome", RubyObject);
#endif
}

VALUE MountainhomeBinding::StopMainLoop(VALUE self) {
    Mountainhome::GetSingleton()->stopMainLoop();
    return self;
}

VALUE MountainhomeBinding::SetState(VALUE self, VALUE name) {
    std::string strName = rb_string_value_cstr(&name);
    Mountainhome::GetSingleton()->setActiveState(strName);
    return name;
}

VALUE MountainhomeBinding::RegisterState(VALUE self, VALUE state, VALUE name) {
    std::string strName = rb_string_value_cstr(&name);
    Mountainhome::GetSingleton()->registerState(StateBinding::GetState(state), strName);
    return state;
}

VALUE StateBinding::RubyClass = 0;
VALUE StateBinding::UpdateMethod = 0;
StateBinding::RubyObjectMap StateBinding::RubyObjects;

void StateBinding::Setup() {
  UpdateMethod = rb_intern("update");
  RubyClass = rb_define_class("State", rb_cObject);
  rb_define_method(RubyClass, "initialize", RUBY_METHOD_FUNC(StateBinding::Initialize), 0);
}

StateBinding* StateBinding::GetState(VALUE robj) {
    if (RubyObjects.find(robj) == RubyObjects.end()) {
        RAISE(InternalError, "StateBinding does not exist for " << robj << "!");
    }

    return StateBinding::RubyObjects[robj];
}

VALUE StateBinding::Initialize(VALUE self) {
    if (RubyObjects.find(self) != RubyObjects.end()) {
        RAISE(DuplicateItemError, "StateBinding already exists for " << self << "!");
    }

    RubyObjects[self] = new StateBinding(self);
    return self;
}

StateBinding::StateBinding(VALUE robj): _rubyObject(robj) {}
StateBinding::~StateBinding() {}
void StateBinding::update() {
    rb_funcall(_rubyObject, UpdateMethod, 0);
}
