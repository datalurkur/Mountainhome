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
{}

Camera * CameraBindings::getPointer(VALUE rObj) {
    Camera *cObj;
    Data_Get_Struct(rObj, Camera, cObj);
    return cObj;
}

template <>
void RubyBindings<Camera, false>::Free(Camera* cobj) {
    Get()->unregisterPair(cobj);
}
