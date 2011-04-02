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

    rb_define_method(_class, "enable_channel",  RUBY_METHOD_FUNC(LoggerBindings::EnableChannel ), 1);
    rb_define_method(_class, "disable_channel", RUBY_METHOD_FUNC(LoggerBindings::DisableChannel), 1);

    rb_define_method(_class, "enable_all_channels", RUBY_METHOD_FUNC(LoggerBindings::EnableAllChannels), 0);
    rb_define_method(_class, "disable_all_channels", RUBY_METHOD_FUNC(LoggerBindings::DisableAllChannels), 0);

    rb_define_method(_class, "indent", RUBY_METHOD_FUNC(LoggerBindings::IncrementIndent), 0);
    rb_define_method(_class, "unindent", RUBY_METHOD_FUNC(LoggerBindings::DecrementIndent), 0);

    _object = rb_class_new_instance(NULL, 0, _class);
    rb_gv_set("$logger", _object);
}

VALUE LoggerBindings::Log(VALUE str, LogStream::LogType level, LogChannel channel) {
    if (LogStream::IsChannelEnabled(channel)) {
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
    return Qnil;
}

VALUE LoggerBindings::IncrementIndent(VALUE self) { LogStream::IncrementIndent(); return self; }
VALUE LoggerBindings::DecrementIndent(VALUE self) { LogStream::DecrementIndent(); return self; }

VALUE LoggerBindings::LogWarn(VALUE self, VALUE str)  { return Log(str, LogStream::WarningMessage, LogStream::DefaultChannel); }
VALUE LoggerBindings::LogInfo(VALUE self, VALUE str)  { return Log(str, LogStream::InfoMessage,    LogStream::DefaultChannel); }
VALUE LoggerBindings::LogError(VALUE self, VALUE str) { return Log(str, LogStream::ErrorMessage,   LogStream::DefaultChannel); }
VALUE LoggerBindings::LogDebug(VALUE self, VALUE str) { return Log(str, LogStream::DebugMessage,   LogStream::DefaultChannel); }

VALUE LoggerBindings::EnableChannel (VALUE self, VALUE channel) {
    LogChannel lchan = (LogChannel)NUM2INT(channel);
    LogStream::EnableLogChannel(lchan);
    return Qtrue;
}

VALUE LoggerBindings::DisableChannel(VALUE self, VALUE channel) {
    LogChannel lchan = (LogChannel)NUM2INT(channel);
    LogStream::DisableLogChannel(lchan);
    return Qfalse;
}

VALUE LoggerBindings::EnableAllChannels(VALUE self) { LogStream::EnableAllChannels(); return Qtrue; }
VALUE LoggerBindings::DisableAllChannels(VALUE self) { LogStream::DisableAllChannels(); return Qfalse; }
