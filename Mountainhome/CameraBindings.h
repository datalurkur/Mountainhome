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
#include <Engine/Camera.h>
#include "RubyBindings.h"

class CameraBindings : public RubyBindings<Camera, false> {
public:
    /*! Sets up an orthographic projection centered at a particular location. */
    static VALUE CenterOrtho(VALUE rSelf, VALUE width, VALUE height, VALUE x, VALUE y, VALUE near, VALUE far);

    /*! Creates a perspective projection matrix, which is what causes things to shrink in
     *  size as they move further away from the origin. */
    static VALUE Perspective(VALUE rSelf, VALUE width, VALUE height, VALUE fov, VALUE near, VALUE far);

    /*! Creates an orthographic projection matrix, which is a form of parallel projection,
     *  meaning as things move a way from the origin, their size remains constant. */
    static VALUE Ortho(VALUE rSelf, VALUE left, VALUE right, VALUE bottom, VALUE top, VALUE near, VALUE far);

public:
    CameraBindings();
    virtual ~CameraBindings() {}

    virtual Camera *getPointer(VALUE rObj);

};

// A specialization that doesn't try to delete the Camera, which wouldn't compile.
template <> void RubyBindings<Camera, false>::Free(Camera* cobj);

#endif
