/*
 *  MHCameraBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 4/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHCameraBindings.h"
#include "MHWorldBindings.h"
#include "SceneNodeBindings.h"

MHCameraBindings::MHCameraBindings()
: RubyBindings<MHCamera, false>(
    rb_define_class("MHCamera", SceneNodeBindings::Get()->getClass()), "MHCameraBindings")
{
    rb_define_method(_class, "make_center_ortho", RUBY_METHOD_FUNC(MHCameraBindings::MakeCenterOrtho), 4);
    rb_define_method(_class, "make_perspective", RUBY_METHOD_FUNC(MHCameraBindings::MakePerspective), 1);
    rb_define_method(_class, "world", RUBY_METHOD_FUNC(MHCameraBindings::GetWorld), 0);
    rb_define_method(_class, "ratio", RUBY_METHOD_FUNC(MHCameraBindings::GetRatio), 0);
    rb_define_method(_class, "name", RUBY_METHOD_FUNC(MHCameraBindings::GetName), 0);
}

VALUE MHCameraBindings::MakeCenterOrtho(VALUE rSelf, VALUE x, VALUE y, VALUE width, VALUE height) {
    MHCamera *cSelf = Get()->getPointer(rSelf);
    cSelf->makeCenterOrtho(NUM2DBL(x), NUM2DBL(y), NUM2DBL(width), NUM2DBL(height));
    return Qnil;
}

VALUE MHCameraBindings::MakePerspective(VALUE rSelf, VALUE fov) {
    MHCamera *cSelf = Get()->getPointer(rSelf);
    cSelf->makePerspective(NUM2DBL(GetRatio(rSelf)), Degree(NUM2DBL(fov)));
    return Qnil;
}

VALUE MHCameraBindings::GetWorld(VALUE rSelf) {
    MHCamera *cSelf = Get()->getPointer(rSelf);
    return MHWorldBindings::Get()->getValue(cSelf->getWorld());
}

VALUE MHCameraBindings::GetName(VALUE rSelf) {
    MHCamera *cSelf = Get()->getPointer(rSelf);
    return rb_str_new_cstr(cSelf->getName().c_str());
}

VALUE MHCameraBindings::GetRatio(VALUE rSelf) {
    static ID rWindow = rb_intern("window");
    static ID rRatio = rb_intern("ratio");
    static VALUE rCore = rb_gv_get("$mhcore");
    return rb_funcall(rb_funcall(rCore, rWindow, 0), rRatio, 0);
}
