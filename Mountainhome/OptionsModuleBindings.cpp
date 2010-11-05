/*
 *  OptionsModuleBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/8/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "OptionsModuleBindings.h"

OptionsModuleBindings::OptionsModuleBindings()
:RubyBindings<OptionsModule, false>(
    rb_define_class("MHOptions", rb_cObject),
    "OptionsModuleBindings")
{
    rb_define_method(_class, "apply", RUBY_METHOD_FUNC(OptionsModuleBindings::Apply), 0);
    rb_define_method(_class, "get", RUBY_METHOD_FUNC(OptionsModuleBindings::GetValue), 1);
    rb_define_method(_class, "put", RUBY_METHOD_FUNC(OptionsModuleBindings::PutValue), 2);
}

VALUE OptionsModuleBindings::Apply(VALUE rSelf) {
    OptionsModule *cSelf = Get()->getPointer(rSelf);
    cSelf->apply();

    return rSelf;
}

VALUE OptionsModuleBindings::GetValue(VALUE rSelf, VALUE rKey) {
    OptionsModule *cSelf = Get()->getPointer(rSelf);
    std::string cKey = rb_string_value_cstr(&rKey);
    std::string cValue = cSelf->get<std::string>(cKey);

    return rb_str_new2(cValue.c_str());
}

VALUE OptionsModuleBindings::PutValue(VALUE rSelf, VALUE rKey, VALUE rVal) {
    OptionsModule *cSelf = Get()->getPointer(rSelf);
    std::string cKey = rb_string_value_cstr(&rKey);
    std::string cValue = rb_string_value_cstr(&rVal);

    cSelf->put(cKey, cValue);

    return rSelf;
}
