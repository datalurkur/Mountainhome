/*
 *  RubyCamera.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RubyCamera.h"

VALUE RubyCamera::New(Camera *cam) {
    VALUE rCam = CreateBindingPair(RubyCamera, cam);
    return rCam;
}

void RubyCamera::SetupBindings() {
    Class = rb_define_class("MHCamera", rb_cObject);
    rb_define_method(Class, "ratio", RUBY_METHOD_FUNC(RubyCamera::Ratio), 0);
	rb_define_method(Class, "set_fixed_yaw", RUBY_METHOD_FUNC(RubyCamera::SetFixedYaw), 3);
	rb_define_method(Class, "set_position", RUBY_METHOD_FUNC(RubyCamera::SetPosition), 3);
	rb_define_method(Class, "look_at", RUBY_METHOD_FUNC(RubyCamera::LookAt), 3);
	rb_define_method(Class, "rotate_on_axis", RUBY_METHOD_FUNC(RubyCamera::RotateOnAxis), 4);
    rb_define_method(Class, "move_absolute", RUBY_METHOD_FUNC(RubyCamera::MoveAbsolute), 3);
	rb_define_method(Class, "move_relative", RUBY_METHOD_FUNC(RubyCamera::MoveRelative), 3);
	rb_define_method(Class, "adjust_yaw", RUBY_METHOD_FUNC(RubyCamera::AdjustYaw), 1);
	rb_define_method(Class, "adjust_pitch", RUBY_METHOD_FUNC(RubyCamera::AdjustPitch), 1);
	rb_define_method(Class, "adjust_roll", RUBY_METHOD_FUNC(RubyCamera::AdjustRoll), 1);
    rb_define_method(Class, "center_ortho", RUBY_METHOD_FUNC(RubyCamera::CenterOrtho), 5);
}

VALUE RubyCamera::Ratio(VALUE rSelf) {
    AssignCObjFromValue(Camera, cSelf, rSelf);
    return DBL2NUM(cSelf->getFrustum()->getRatio());
}

VALUE RubyCamera::SetFixedYaw(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(Camera, cSelf, rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->setFixedYawAxis(true, vec);
	return rSelf;
}

VALUE RubyCamera::SetPosition(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(Camera, cSelf, rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->setPosition(vec);
	return rSelf;
}

VALUE RubyCamera::LookAt(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(Camera, cSelf, rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->lookAt(vec);
	return rSelf;
}

VALUE RubyCamera::RotateOnAxis(VALUE rSelf, VALUE amt, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(Camera, cSelf, rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
    cSelf->rotate(Radian(NUM2DBL(amt)), vec);
	return rSelf;
}

VALUE RubyCamera::MoveAbsolute(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(Camera, cSelf, rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->moveAbsolute(vec);
	return rSelf;
}

VALUE RubyCamera::MoveRelative(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(Camera, cSelf, rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->moveRelative(vec);
	return rSelf;
}

VALUE RubyCamera::AdjustYaw(VALUE rSelf, VALUE yaw) {
    AssignCObjFromValue(Camera, cSelf, rSelf);
    cSelf->adjustYaw(Radian(NUM2DBL(yaw)));
    return rSelf;
}

VALUE RubyCamera::AdjustPitch(VALUE rSelf, VALUE pitch) {
    AssignCObjFromValue(Camera, cSelf, rSelf);
    cSelf->adjustPitch(Radian(NUM2DBL(pitch)));
    return rSelf;
}

VALUE RubyCamera::AdjustRoll(VALUE rSelf, VALUE roll) {
    AssignCObjFromValue(Camera, cSelf, rSelf);
    cSelf->adjustRoll(Radian(NUM2DBL(roll)));
    return rSelf;
}

VALUE RubyCamera::CenterOrtho(VALUE rSelf, VALUE width, VALUE x, VALUE y, VALUE near, VALUE far) {
    AssignCObjFromValue(Camera, cSelf, rSelf);
    cSelf->getFrustum()->centerOrtho(NUM2DBL(width), Vector2(NUM2DBL(x), NUM2DBL(y)), NUM2DBL(near), NUM2DBL(far));
    return rSelf;
}

template <>
void RubyBindings<Camera, false>::Free(Camera* cobj) {
    UnregisterPair(cobj);
}
