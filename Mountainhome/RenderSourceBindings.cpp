/*
 *  RenderSourceBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/24/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RenderSourceBindings.h"
#include "MHUIManagerBindings.h"
#include "CameraBindings.h"

RenderSourceBindings::RenderSourceBindings()
: RubyBindings<RenderSource, false>(
    rb_define_class("RenderSource", rb_cObject),
    "RenderSourceBindings")
{}

RenderSourceBindings::~RenderSourceBindings() {}

RenderSource *RenderSourceBindings::getPointer(VALUE rObj) {
    VALUE klass = rb_obj_class(rObj);
    if (klass ==      CameraBindings::Get()->getClass()) { return      CameraBindings::Get()->getPointer(rObj); }
    if (klass == MHUIManagerBindings::Get()->getClass()) { return MHUIManagerBindings::Get()->getPointer(rObj); }
    return RubyBindings<RenderSource, false>::getPointer(rObj);
}
