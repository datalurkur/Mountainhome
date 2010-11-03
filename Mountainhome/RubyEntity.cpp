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

    // set_position should probably be a position=, but this can wait until SceneNode merging.
	rb_define_method(Class, "position", RUBY_METHOD_FUNC(RubyEntity::GetPosition), 0);
	rb_define_method(Class, "set_position", RUBY_METHOD_FUNC(RubyEntity::SetPosition), 3);
	rb_define_method(Class, "x=", RUBY_METHOD_FUNC(RubyEntity::SetPositionX), 3);
	rb_define_method(Class, "y=", RUBY_METHOD_FUNC(RubyEntity::SetPositionY), 3);
	rb_define_method(Class, "z=", RUBY_METHOD_FUNC(RubyEntity::SetPositionZ), 3);
    rb_define_method(Class, "visible=", RUBY_METHOD_FUNC(RubyEntity::SetVisibility), 1);
}

#pragma mark Position bindings

VALUE RubyEntity::GetPosition(VALUE rSelf) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    Vector3 pos = cSelf->getLocalPosition();
    Info("C position: " << pos);
	return rb_ary_new3(3, DBL2NUM(pos[0]), DBL2NUM(pos[1]), DBL2NUM(pos[2]));
}

VALUE RubyEntity::SetPosition(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    cSelf->setPosition(Vector3(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z)));
	return rSelf;
}

VALUE RubyEntity::SetPositionX(VALUE rSelf, VALUE x) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    cSelf->setPositionX(NUM2DBL(x));
	return rSelf;
}

VALUE RubyEntity::SetPositionY(VALUE rSelf, VALUE y) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    cSelf->setPositionY(NUM2DBL(y));
	return rSelf;
}

VALUE RubyEntity::SetPositionZ(VALUE rSelf, VALUE z) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    cSelf->setPositionZ(NUM2DBL(z));
	return rSelf;
}

#pragma mark Other bindings

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
