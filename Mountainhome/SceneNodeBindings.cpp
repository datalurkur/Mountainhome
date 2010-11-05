/*
 *  SceneNodeBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "SceneNodeBindings.h"
#include "CameraBindings.h"
#include "EntityBindings.h"

SceneNodeBindings::SceneNodeBindings()
: RubyBindings<SceneNode, false>(
    rb_define_class("SceneNode", rb_cObject),
    "SceneNodeBindings")
{}

SceneNodeBindings::~SceneNodeBindings() {}

SceneNode *SceneNodeBindings::getPointer(VALUE rObj) {
    VALUE klass = rb_obj_class(rObj);
    if (klass == CameraBindings::Get()->getClass()) { return CameraBindings::Get()->getPointer(rObj); }
    if (klass == EntityBindings::Get()->getClass()) { return EntityBindings::Get()->getPointer(rObj); }
    return RubyBindings<SceneNode, false>::getPointer(rObj);
}
