/*
 *  LoggerBindings.cpp
 *  System
 *
 *  Created by loch on 2/22/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "LoggerBindings.h"

LoggerBindings::LoggerBindings(): _object(NULL) {
    _class = rb_define_class("MHLogger", rb_cObject);
    rb_define_method(_class, "info",  RUBY_METHOD_FUNC(LoggerBindings::LogInfo),  1);
    rb_define_method(_class, "warn",  RUBY_METHOD_FUNC(LoggerBindings::LogWarn),  1);
    rb_define_method(_class, "error", RUBY_METHOD_FUNC(LoggerBindings::LogError), 1);
    rb_define_method(_class, "debug", RUBY_METHOD_FUNC(LoggerBindings::LogDebug), 1);

    rb_define_method(_class, "indent", RUBY_METHOD_FUNC(LoggerBindings::IncrementIndent), 0);
    rb_define_method(_class, "unindent", RUBY_METHOD_FUNC(LoggerBindings::DecrementIndent), 0);

    _object = rb_class_new_instance(NULL, 0, _class);
    rb_gv_set("$logger", _object);
}

VALUE LoggerBindings::Log(VALUE str, LogStream::LogType level) {
    std::ostringstream stream;
    stream << rb_string_value_cstr(&str);
    std::string str_val = stream.str();

    std::list<std::string> log_list;
    tokenize <std::list<std::string> > (str_val, "\n", log_list);

    std::list<std::string>::iterator itr;
    for(itr = log_list.begin(); itr != log_list.end(); itr++) {
        LogAtLevelWithFL((*itr), true, level, rb_sourcefile(), rb_sourceline());
    }

    return str;
}

VALUE LoggerBindings::IncrementIndent(VALUE self) { LogStream::IncrementIndent(); return self; }
VALUE LoggerBindings::DecrementIndent(VALUE self) { LogStream::DecrementIndent(); return self; }

VALUE LoggerBindings::LogWarn(VALUE self, VALUE str)  { return Log(str, LogStream::WarningMessage); }
VALUE LoggerBindings::LogInfo(VALUE self, VALUE str)  { return Log(str, LogStream::InfoMessage);    }
VALUE LoggerBindings::LogError(VALUE self, VALUE str) { return Log(str, LogStream::ErrorMessage);   }
VALUE LoggerBindings::LogDebug(VALUE self, VALUE str) { return Log(str, LogStream::DebugMessage);   }
