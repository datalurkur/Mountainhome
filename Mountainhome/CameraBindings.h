/*
 *  CameraBindings.h
 *  Mountainhome
 *
 *  Created by loch on 4/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _CAMERABINDINGS_H_
#define _CAMERABINDINGS_H_
#include <Render/Camera.h>
#include "RubyBindings.h"

class CameraBindings : public RubyBindings<Camera, false> {
public:
    static VALUE Ratio(VALUE self);
    static VALUE CenterOrtho(VALUE rSelf, VALUE width, VALUE x, VALUE y, VALUE near, VALUE far);

public:
    CameraBindings();
    virtual ~CameraBindings() {}

    virtual Camera *getPointer(VALUE rObj);

};

// A specialization that doesn't try to delete the Camera, which wouldn't compile.
template <> void RubyBindings<Camera, false>::Free(Camera* cobj);

#endif
