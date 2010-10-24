/*
 *  RubyEntity.cpp
 *  Mountainhome
 *
 *  Created by Paul on 6/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RubyEntity.h"

void RubyEntity::SetupBindings() {
    Class = rb_define_class("MHEntity", rb_cObject);

	rb_define_method(Class, "set_position", RUBY_METHOD_FUNC(RubyEntity::SetPosition), 3);
    rb_define_method(Class, "visible=", RUBY_METHOD_FUNC(RubyEntity::SetVisibility), 1);
}

VALUE RubyEntity::SetPosition(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    cSelf->setPosition(Vector3(NUM2INT(x), NUM2INT(y), NUM2INT(z)));
	return rSelf;
}

VALUE RubyEntity::SetVisibility(VALUE rSelf, VALUE rState) {
    AssignCObjFromValue(Entity, cSelf, rSelf);

    bool cState = (rState == Qtrue) ? true : false;
    cSelf->setVisibility(cState);

    return rSelf;
}

template <>
void RubyBindings<Entity, false>::Free(Entity* cobj) {
    UnregisterPair(cobj);
}
