/*
 *  CameraBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "CameraBindings.h"

CameraBindings::CameraBindings()
: RubyBindings<Camera, false>(
    rb_define_class("MHCamera", rb_cObject),
    "CameraBindings")
{
    rb_define_method(_class, "ratio", RUBY_METHOD_FUNC(CameraBindings::Ratio), 0);
	rb_define_method(_class, "set_fixed_yaw", RUBY_METHOD_FUNC(CameraBindings::SetFixedYaw), 3);
	rb_define_method(_class, "set_position", RUBY_METHOD_FUNC(CameraBindings::SetPosition), 3);
	rb_define_method(_class, "look_at", RUBY_METHOD_FUNC(CameraBindings::LookAt), 3);
	rb_define_method(_class, "rotate_on_axis", RUBY_METHOD_FUNC(CameraBindings::RotateOnAxis), 4);
    rb_define_method(_class, "move_absolute", RUBY_METHOD_FUNC(CameraBindings::MoveAbsolute), 3);
	rb_define_method(_class, "move_relative", RUBY_METHOD_FUNC(CameraBindings::MoveRelative), 3);
	rb_define_method(_class, "adjust_yaw", RUBY_METHOD_FUNC(CameraBindings::AdjustYaw), 1);
	rb_define_method(_class, "adjust_pitch", RUBY_METHOD_FUNC(CameraBindings::AdjustPitch), 1);
	rb_define_method(_class, "adjust_roll", RUBY_METHOD_FUNC(CameraBindings::AdjustRoll), 1);
    rb_define_method(_class, "center_ortho", RUBY_METHOD_FUNC(CameraBindings::CenterOrtho), 5);
}

Camera *CameraBindings::getPointer(VALUE rObj) {
    Camera *cObj;
    Data_Get_Struct(rObj, Camera, cObj);
    return cObj;
}

VALUE CameraBindings::Ratio(VALUE rSelf) {
    Camera *cSelf = Get()->getPointer(rSelf);
    return DBL2NUM(cSelf->getFrustum()->getRatio());
}

VALUE CameraBindings::SetFixedYaw(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    Camera *cSelf = Get()->getPointer(rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->setFixedYawAxis(true, vec);
	return rSelf;
}

VALUE CameraBindings::SetPosition(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    Camera *cSelf = Get()->getPointer(rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->setPosition(vec);
	return rSelf;
}

VALUE CameraBindings::LookAt(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    Camera *cSelf = Get()->getPointer(rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->lookAt(vec);
	return rSelf;
}

VALUE CameraBindings::RotateOnAxis(VALUE rSelf, VALUE amt, VALUE x, VALUE y, VALUE z) {
    Camera *cSelf = Get()->getPointer(rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
    cSelf->rotate(Radian(NUM2DBL(amt)), vec);
	return rSelf;
}

VALUE CameraBindings::MoveAbsolute(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    Camera *cSelf = Get()->getPointer(rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->moveAbsolute(vec);
	return rSelf;
}

VALUE CameraBindings::MoveRelative(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    Camera *cSelf = Get()->getPointer(rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->moveRelative(vec);
	return rSelf;
}

VALUE CameraBindings::AdjustYaw(VALUE rSelf, VALUE yaw) {
    Camera *cSelf = Get()->getPointer(rSelf);
    cSelf->adjustYaw(Radian(NUM2DBL(yaw)));
    return rSelf;
}

VALUE CameraBindings::AdjustPitch(VALUE rSelf, VALUE pitch) {
    Camera *cSelf = Get()->getPointer(rSelf);
    cSelf->adjustPitch(Radian(NUM2DBL(pitch)));
    return rSelf;
}

VALUE CameraBindings::AdjustRoll(VALUE rSelf, VALUE roll) {
    Camera *cSelf = Get()->getPointer(rSelf);
    cSelf->adjustRoll(Radian(NUM2DBL(roll)));
    return rSelf;
}

VALUE CameraBindings::CenterOrtho(VALUE rSelf, VALUE width, VALUE x, VALUE y, VALUE near, VALUE far) {
    Camera *cSelf = Get()->getPointer(rSelf);
    cSelf->getFrustum()->centerOrtho(NUM2DBL(width), Vector2(NUM2DBL(x), NUM2DBL(y)), NUM2DBL(near), NUM2DBL(far));
    return rSelf;
}

template <>
void RubyBindings<Camera, false>::Free(Camera* cobj) {
    Get()->unregisterPair(cobj);
}
