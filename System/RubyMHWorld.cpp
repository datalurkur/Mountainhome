/*
 *  RubyMHWorld.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RubyMHWorld.h"
#include "MHWorld.h"

RubyMHWorld::RubyObjectMap RubyMHWorld::Objects;
VALUE RubyMHWorld::Class = NULL;

void RubyMHWorld::SetupBindings() {
    Class = rb_define_class("MHWorld", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(RubyMHWorld::Initialize), 0);
}

MHWorld* RubyMHWorld::GetWorld(VALUE robj) {
    if (Objects.find(robj) == Objects.end()) {
        THROW(InternalError, "RubyMHWorld does not exist for " << robj << "!");
    }

    return RubyMHWorld::Objects[robj];
}

VALUE RubyMHWorld::Initialize(VALUE self) {
    if (Objects.find(self) != Objects.end()) {
        THROW(DuplicateItemError, "RubyMHWorld already exists for " << self << "!");
    }

    Objects[self] = new MHWorld();
    return self;
}
