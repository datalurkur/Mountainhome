/*
 *  RubyMHObject.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RubyMHObject.h"
#include "RubyMHWorld.h"
#include "MHWorld.h"
#include "MHObject.h"

RubyMHObject::RubyObjectMap RubyMHObject::Objects;
VALUE RubyMHObject::Class = NULL;

void RubyMHObject::SetupBindings() {
    Class = rb_define_class("MHObject", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(RubyMHObject::Initialize), 4);
}

VALUE RubyMHObject::Initialize(VALUE self, VALUE name, VALUE world, VALUE model, VALUE mat) {
    if (Objects.find(self) != Objects.end()) {
        THROW(DuplicateItemError, "RubyMHObject already exists for " << self << "!");
    }

    std::string strName  = rb_string_value_cstr(&name);
    std::string strModel = rb_string_value_cstr(&model);
    std::string strMat   = rb_string_value_cstr(&mat);
    MHWorld *objWorld    = RubyMHWorld::GetWorld(world);

    Objects[self] = new MHObject(strName, objWorld, strModel, strMat);
    return self;
}
