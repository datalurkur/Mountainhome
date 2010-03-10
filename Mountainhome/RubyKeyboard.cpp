/*
 *  RubyKeyboard.cpp
 *  Mountainhome
 *
 *  Created by Paul on 3/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RubyKeyboard.h"

VALUE RubyKeyboard::Class = NULL;
VALUE RubyKeyboard::RubyLookup = NULL;

VALUE RubyKeyboard::Method_Missing(int argc, VALUE * argv, VALUE self) {
    VALUE name = argv[0];
    if(rb_hash_aref(RubyLookup, name) != Qnil) {
        return rb_hash_aref(RubyLookup, name);
    } else {
        return rb_call_super(argc, argv);
    }
}

VALUE RubyKeyboard::Map_Key(VALUE self, VALUE key, VALUE value) {
    rb_hash_aset(RubyLookup, key, value);
    return Qnil;
}

void RubyKeyboard::SetupBindings() {
    Class = rb_define_class("Keyboard", rb_cObject);
    rb_define_method(Class, "method_missing?",  RUBY_METHOD_FUNC(RubyKeyboard::Method_Missing),  -1);
    rb_define_method(Class, "map_key",  RUBY_METHOD_FUNC(RubyKeyboard::Map_Key),  2);

    RubyLookup = rb_hash_new();

//    Object = rb_class_new_instance(NULL, 0, Class);
//    rb_gv_set("$logger", Object);
//    
//    
//    TeardownMethod = rb_intern("teardown");
//    UpdateMethod   = rb_intern("update");
//    SetupMethod    = rb_intern("setup");
//
//    Class = rb_define_class("State", rb_cObject);
//    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(RubyStateProxy::Initialize), 0);
//    rb_define_method(Class, "teardown", RUBY_METHOD_FUNC(RubyStateProxy::Teardown), 0);
//    rb_define_method(Class, "setup", RUBY_METHOD_FUNC(RubyStateProxy::Setup), 0);
}
