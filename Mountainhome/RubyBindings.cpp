/*
 *  RubyBindings.cpp
 *  System
 *
 *  Created by loch on 2/15/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RubyBindings.h"

VALUE get_class_value(const char *name) {
    VALUE klass = Qnil;
    ID id = rb_intern(name);
    if (rb_const_defined(rb_cObject, id)) {
        klass = rb_const_get(rb_cObject, id);
        if (TYPE(klass) != T_CLASS) {
            rb_raise(rb_eTypeError, "%s is not a class", name);
        }
    }
	return klass;
}

void raise_ruby_error() {
    
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

