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

void MountainhomeBinding::SetupBindings() {
    RubyClass = rb_define_class("Mountainhome", rb_cObject);
    rb_define_method(RubyClass, "register_state", RUBY_METHOD_FUNC(MountainhomeBinding::RegisterState), 2);
    rb_define_method(RubyClass, "state=", RUBY_METHOD_FUNC(MountainhomeBinding::SetState), 1);
    rb_define_method(RubyClass, "exit", RUBY_METHOD_FUNC(MountainhomeBinding::StopMainLoop), 0);
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
    Mountainhome::Get()->stopMainLoop();
    return self;
}

VALUE MountainhomeBinding::SetState(VALUE self, VALUE name) {
    std::string strName = rb_string_value_cstr(&name);
    Mountainhome::Get()->setActiveState(strName);
    return name;
}

VALUE MountainhomeBinding::RegisterState(VALUE self, VALUE state, VALUE name) {
    std::string strName = rb_string_value_cstr(&name);
    Info("Registering state " << StateBinding::GetState(state) << " under '" << strName << "'.");
    Mountainhome::Get()->registerState(StateBinding::GetState(state), strName);
    return state;
}
















VALUE StateBinding::RubyClass = NULL;
VALUE StateBinding::UpdateMethod = NULL;
VALUE StateBinding::TeardownMethod = NULL;
VALUE StateBinding::SetupMethod = NULL;
StateBinding::RubyObjectMap StateBinding::RubyObjects;

void StateBinding::SetupBindings() {
    TeardownMethod = rb_intern("teardown");
    UpdateMethod = rb_intern("update");
    SetupMethod = rb_intern("setup");
    RubyClass = rb_define_class("State", rb_cObject);

    rb_define_method(RubyClass, "initialize", RUBY_METHOD_FUNC(StateBinding::Initialize), 0);
    rb_define_method(RubyClass, "teardown", RUBY_METHOD_FUNC(StateBinding::Teardown), 0);
    rb_define_method(RubyClass, "setup", RUBY_METHOD_FUNC(StateBinding::Setup), 0);
}

StateBinding* StateBinding::GetState(VALUE robj) {
    if (RubyObjects.find(robj) == RubyObjects.end()) {
        THROW(InternalError, "StateBinding does not exist for " << robj << "!");
    }

    return StateBinding::RubyObjects[robj];
}

VALUE StateBinding::Setup(VALUE self) { return self; }
VALUE StateBinding::Teardown(VALUE self) { return self; }
VALUE StateBinding::Initialize(VALUE self) {
    if (RubyObjects.find(self) != RubyObjects.end()) {
        THROW(DuplicateItemError, "StateBinding already exists for " << self << "!");
    }

    RubyObjects[self] = new StateBinding(self);
    return self;
}

StateBinding::StateBinding(VALUE robj): _rubyObject(robj) {}
StateBinding::~StateBinding() {}
void StateBinding::update(int elapsed) { rb_funcall(_rubyObject, UpdateMethod, 1, INT2FIX(elapsed)); }
void StateBinding::setup(va_list args) { rb_funcall(_rubyObject, SetupMethod, 0);                    }
void StateBinding::teardown()          { rb_funcall(_rubyObject, TeardownMethod, 0);                 }
