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
    Class = rb_define_class("MHLogger", rb_cObject);
    rb_define_method(Class, "info",  RUBY_METHOD_FUNC(RubyLogger::LogInfo),  1);
    rb_define_method(Class, "warn",  RUBY_METHOD_FUNC(RubyLogger::LogWarn),  1);
    rb_define_method(Class, "error", RUBY_METHOD_FUNC(RubyLogger::LogError), 1);
    rb_define_method(Class, "debug", RUBY_METHOD_FUNC(RubyLogger::LogDebug), 1);

    rb_define_method(Class, "indent", RUBY_METHOD_FUNC(RubyLogger::IncrementIndent), 0);
    rb_define_method(Class, "unindent", RUBY_METHOD_FUNC(RubyLogger::DecrementIndent), 0);

    Object = rb_class_new_instance(NULL, 0, Class);
    rb_gv_set("$logger", Object);
}

VALUE RubyLogger::Log(VALUE str, LogStream::LogType level) {
    LogAtLevelWithFL(rb_string_value_cstr(&str), true, level, rb_sourcefile(), rb_sourceline());
    return str;
}

VALUE RubyLogger::IncrementIndent(VALUE self) { LogStream::IncrementIndent(); }
VALUE RubyLogger::DecrementIndent(VALUE self) { LogStream::DecrementIndent(); }

VALUE RubyLogger::LogWarn(VALUE self, VALUE str)  { return Log(str, LogStream::WarningMessage); }
VALUE RubyLogger::LogInfo(VALUE self, VALUE str)  { return Log(str, LogStream::InfoMessage);    }
VALUE RubyLogger::LogError(VALUE self, VALUE str) { return Log(str, LogStream::ErrorMessage);   }
VALUE RubyLogger::LogDebug(VALUE self, VALUE str) { return Log(str, LogStream::DebugMessage);   }
