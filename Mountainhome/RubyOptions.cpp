/*
 *  RubyOptions.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RubyOptions.h"

void RubyOptions::SetupBindings() {
    Class = rb_define_class("MHOptions", rb_cObject);
    rb_define_method(Class, "apply", RUBY_METHOD_FUNC(RubyOptions::Apply), 0);
    rb_define_method(Class, "get", RUBY_METHOD_FUNC(RubyOptions::Get), 1);
    rb_define_method(Class, "put", RUBY_METHOD_FUNC(RubyOptions::Put), 2);
}

VALUE RubyOptions::Apply(VALUE rSelf) {
    AssignCObjFromValue(OptionsModule, cSelf, rSelf);
    cSelf->apply();

    return rSelf;
}

VALUE RubyOptions::Get(VALUE rSelf, VALUE rKey) {
    AssignCObjFromValue(OptionsModule, cSelf, rSelf);
    std::string cKey = rb_string_value_cstr(&rKey);
    std::string cValue = cSelf->get<std::string>(cKey);

    return rb_str_new2(cValue.c_str());
}

VALUE RubyOptions::Put(VALUE rSelf, VALUE rKey, VALUE rVal) {
    AssignCObjFromValue(OptionsModule, cSelf, rSelf);
    std::string cKey = rb_string_value_cstr(&rKey);
    std::string cValue = rb_string_value_cstr(&rVal);

    cSelf->put(cKey, cValue);

    return rSelf;
}
