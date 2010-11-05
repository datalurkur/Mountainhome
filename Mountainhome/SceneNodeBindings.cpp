/*
 *  SceneNodeBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "SceneNodeBindings.h"
#include "MHUIElementBindings.h"
#include "CameraBindings.h"
#include "EntityBindings.h"

VALUE SceneNodeBindings::SetFixedYaw(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    SceneNode *cSelf = Get()->getPointer(rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->setFixedYawAxis(true, vec);
	return rSelf;
}

VALUE SceneNodeBindings::LookAt(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    SceneNode *cSelf = Get()->getPointer(rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->lookAt(vec);
	return rSelf;
}

VALUE SceneNodeBindings::RotateOnAxis(VALUE rSelf, VALUE amt, VALUE x, VALUE y, VALUE z) {
    SceneNode *cSelf = Get()->getPointer(rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
    cSelf->rotate(Radian(NUM2DBL(amt)), vec);
	return rSelf;
}

VALUE SceneNodeBindings::MoveAbsolute(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    SceneNode *cSelf = Get()->getPointer(rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->moveAbsolute(vec);
	return rSelf;
}

VALUE SceneNodeBindings::MoveRelative(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    SceneNode *cSelf = Get()->getPointer(rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->moveRelative(vec);
	return rSelf;
}

VALUE SceneNodeBindings::AdjustYaw(VALUE rSelf, VALUE yaw) {
    SceneNode *cSelf = Get()->getPointer(rSelf);
    cSelf->adjustYaw(Radian(NUM2DBL(yaw)));
    return rSelf;
}

VALUE SceneNodeBindings::AdjustPitch(VALUE rSelf, VALUE pitch) {
    SceneNode *cSelf = Get()->getPointer(rSelf);
    cSelf->adjustPitch(Radian(NUM2DBL(pitch)));
    return rSelf;
}

VALUE SceneNodeBindings::AdjustRoll(VALUE rSelf, VALUE roll) {
    SceneNode *cSelf = Get()->getPointer(rSelf);
    cSelf->adjustRoll(Radian(NUM2DBL(roll)));
    return rSelf;
}

VALUE SceneNodeBindings::SetPosition(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    SceneNode *cSelf = Get()->getPointer(rSelf);
    Vector3 vec(NUM2DBL(x), NUM2DBL(y), NUM2DBL(z));
	cSelf->setPosition(vec);
	return rSelf;
}

SceneNodeBindings::SceneNodeBindings()
: RubyBindings<SceneNode, false>(
    rb_define_class("SceneNode", rb_cObject),
    "SceneNodeBindings")
{
	rb_define_method(_class, "set_fixed_yaw", RUBY_METHOD_FUNC(SceneNodeBindings::SetFixedYaw), 3);
	rb_define_method(_class, "look_at", RUBY_METHOD_FUNC(SceneNodeBindings::LookAt), 3);
	rb_define_method(_class, "rotate_on_axis", RUBY_METHOD_FUNC(SceneNodeBindings::RotateOnAxis), 4);
    rb_define_method(_class, "move_absolute", RUBY_METHOD_FUNC(SceneNodeBindings::MoveAbsolute), 3);
	rb_define_method(_class, "move_relative", RUBY_METHOD_FUNC(SceneNodeBindings::MoveRelative), 3);
	rb_define_method(_class, "adjust_yaw", RUBY_METHOD_FUNC(SceneNodeBindings::AdjustYaw), 1);
	rb_define_method(_class, "adjust_pitch", RUBY_METHOD_FUNC(SceneNodeBindings::AdjustPitch), 1);
	rb_define_method(_class, "adjust_roll", RUBY_METHOD_FUNC(SceneNodeBindings::AdjustRoll), 1);
	rb_define_method(_class, "set_position", RUBY_METHOD_FUNC(SceneNodeBindings::SetPosition), 3);
}

SceneNodeBindings::~SceneNodeBindings() {}

SceneNode *SceneNodeBindings::getPointer(VALUE rObj) {
    VALUE klass = rb_obj_class(rObj);
    if (klass == CameraBindings::Get()->getClass()) { return CameraBindings::Get()->getPointer(rObj); }
    if (klass == EntityBindings::Get()->getClass()) { return EntityBindings::Get()->getPointer(rObj); }
    return RubyBindings<SceneNode, false>::getPointer(rObj);
}
