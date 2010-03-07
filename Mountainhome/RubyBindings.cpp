/*
 *  RubyBindings.cpp
 *  System
 *
 *  Created by loch on 2/15/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RubyBindings.h"

VALUE require_setup_wrapper(VALUE arg) {
    return rb_require("Mountainhome");
}

void translate_ruby_exception(int error) {
    if (error == 0) { return; }

    VALUE lasterr = rb_gv_get("$!");
    if (NIL_P(lasterr)) { return; }

    // class
    VALUE klass = rb_class_path(CLASS_OF(lasterr));
    VALUE message = rb_obj_as_string(lasterr);
    Error("Exception (" << rb_string_value_cstr(&klass) << "): " <<
        rb_string_value_cstr(&message));

    // backtrace
    VALUE ary = rb_funcall(lasterr, rb_intern("backtrace"), 0);
    int length = NUM2INT(rb_funcall(ary, rb_intern("size"), 0));

    LogStream::IncrementIndent();
    for (int i = 0; i < length; i++) {
        VALUE entry = rb_ary_entry(ary, i);
        Error(rb_string_value_cstr(&entry));
    }
    LogStream::DecrementIndent();

    THROW(InternalError, "Ruby exception caught!");
}

