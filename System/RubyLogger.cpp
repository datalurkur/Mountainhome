/*
 *  RubyLogger.cpp
 *  System
 *
 *  Created by loch on 2/22/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RubyLogger.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Mountainhome ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
VALUE RubyLogger::Class = NULL;
VALUE RubyLogger::Object = NULL;

void RubyLogger::SetupBindings() {
    Class = rb_define_class("MountainhomeLogger", rb_cObject);
    rb_define_method(Class, "info",  RUBY_METHOD_FUNC(RubyLogger::LogInfo),  1);
    rb_define_method(Class, "warn",  RUBY_METHOD_FUNC(RubyLogger::LogWarn),  1);
    rb_define_method(Class, "error", RUBY_METHOD_FUNC(RubyLogger::LogError), 1);
    rb_define_method(Class, "debug", RUBY_METHOD_FUNC(RubyLogger::LogDebug), 1);

    Object = rb_class_new_instance(NULL, 0, Class);
    rb_gv_set("$logger", Object);
}

VALUE RubyLogger::LogWarn(VALUE self, VALUE str) {
    Warn(rb_string_value_cstr(&str));
    return str;
}

VALUE RubyLogger::LogInfo(VALUE self, VALUE str) {
    Info(rb_string_value_cstr(&str));
    return str;
}

VALUE RubyLogger::LogError(VALUE self, VALUE str) {
    Error(rb_string_value_cstr(&str));
    return str;
}

VALUE RubyLogger::LogDebug(VALUE self, VALUE str) {
    Debug(rb_string_value_cstr(&str));
    return str;
}
