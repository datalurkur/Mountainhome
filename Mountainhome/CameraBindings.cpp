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
    rb_define_method(_class, "ratio", RUBY_METHOD_FUNC(CameraBindings::Ratio), 0);
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

VALUE CameraBindings::CenterOrtho(VALUE rSelf, VALUE width, VALUE x, VALUE y, VALUE near, VALUE far) {
    Camera *cSelf = Get()->getPointer(rSelf);
    cSelf->getFrustum()->centerOrtho(NUM2DBL(width), Vector2(NUM2DBL(x), NUM2DBL(y)), NUM2DBL(near), NUM2DBL(far));
    return rSelf;
}

template <>
void RubyBindings<Camera, false>::Free(Camera* cobj) {
    Get()->unregisterPair(cobj);
}
