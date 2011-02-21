/*
 *  CameraBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "CameraBindings.h"
#include "SceneNodeBindings.h"

CameraBindings::CameraBindings()
: RubyBindings<Camera, false>(
    rb_define_class("MHCamera", SceneNodeBindings::Get()->getClass()),
    "CameraBindings")
{
    rb_define_method(_class, "ortho", RUBY_METHOD_FUNC(CameraBindings::Ortho), 6);
    rb_define_method(_class, "center_ortho", RUBY_METHOD_FUNC(CameraBindings::CenterOrtho), 6);
    rb_define_method(_class, "perspective", RUBY_METHOD_FUNC(CameraBindings::Perspective), 5);
}

Camera * CameraBindings::getPointer(VALUE rObj) {
    Camera *cObj;
    Data_Get_Struct(rObj, Camera, cObj);
    return cObj;
}

template <>
void RubyBindings<Camera, false>::Free(Camera* cobj) {
    Get()->unregisterPair(cobj);
}

VALUE CameraBindings::CenterOrtho(VALUE rSelf, VALUE width, VALUE height, VALUE x, VALUE y, VALUE near, VALUE far) {
    Camera *cSelf = Get()->getPointer(rSelf);
    cSelf->setProjectionMatrix(Matrix::CenterOrtho(
        NUM2DBL(width), NUM2DBL(height),
        Vector2(NUM2DBL(x), NUM2DBL(y)),
        NUM2DBL(near), NUM2DBL(far)));
    return Qnil;
}

VALUE CameraBindings::Perspective(VALUE rSelf, VALUE width, VALUE height, VALUE fov, VALUE near, VALUE far) {
    Camera *cSelf = Get()->getPointer(rSelf);
    cSelf->setProjectionMatrix(Matrix::Perspective(
        NUM2DBL(width), NUM2DBL(height), Degree(NUM2DBL(fov)),
        NUM2DBL(near), NUM2DBL(far)));
    return Qnil;
}

VALUE CameraBindings::Ortho(VALUE rSelf, VALUE left, VALUE right, VALUE bottom, VALUE top, VALUE near, VALUE far) {
    Camera *cSelf = Get()->getPointer(rSelf);
    cSelf->setProjectionMatrix(Matrix::Ortho(
        NUM2DBL(left), NUM2DBL(right),
        NUM2DBL(bottom), NUM2DBL(top),
        NUM2DBL(near), NUM2DBL(far)));
    return Qnil;
}
