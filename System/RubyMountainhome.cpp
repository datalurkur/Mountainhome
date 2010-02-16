/*
 *  RubyMountainhome.cpp
 *  System
 *
 *  Created by loch on 2/15/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RubyState.h"
#include "RubyMountainhome.h"
#include "Mountainhome.h"

VALUE RubyMountainhome::Class = NULL;
VALUE RubyMountainhome::Object = NULL;

void RubyMountainhome::SetupBindings() {
    Class = rb_define_class("Mountainhome", rb_cObject);
    rb_define_method(Class, "register_state", RUBY_METHOD_FUNC(RubyMountainhome::RegisterState), 2);
    rb_define_method(Class, "state=", RUBY_METHOD_FUNC(RubyMountainhome::SetState), 1);
    rb_define_method(Class, "exit", RUBY_METHOD_FUNC(RubyMountainhome::StopMainLoop), 0);
#if 0
    rb_include_module(Class, rb_intern("Singleton"));
    Object = rb_funcall(Class, rb_intern("instance"), 0);
#else
    Object = rb_class_new_instance(NULL, 0, Class);
#endif
#if 0
    rb_define_variable("$mountainhome", Object);
#else
    rb_gv_set("$mountainhome", Object);
#endif
}

VALUE RubyMountainhome::StopMainLoop(VALUE self) {
    Mountainhome::Get()->stopMainLoop();
    return self;
}

VALUE RubyMountainhome::SetState(VALUE self, VALUE name) {
    std::string strName = rb_string_value_cstr(&name);
    Mountainhome::Get()->setActiveState(strName);
    return name;
}

VALUE RubyMountainhome::RegisterState(VALUE self, VALUE state, VALUE name) {
    std::string strName = rb_string_value_cstr(&name);
    Info("Registering state " << RubyState::GetState(state) << " under '" << strName << "'.");
    Mountainhome::Get()->registerState(RubyState::GetState(state), strName);
    return state;
}
