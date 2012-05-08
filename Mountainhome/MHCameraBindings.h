/*
 *  MHCameraBindings.h
 *  Mountainhome
 *
 *  Created by loch on 4/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHCameraBindings_H_
#define _MHCameraBindings_H_
#include "RubyBindings.h"
#include "MHCamera.h"

class MHCameraBindings : public RubyBindings<MHCamera, false> {
public:
    /*! Sets up an orthographic projection centered at a particular location. */
    static VALUE MakeCenterOrtho(VALUE rSelf, VALUE x, VALUE y, VALUE width, VALUE height);

    /*! Creates a perspective projection matrix, which is what causes things to shrink in
     *  size as they move further away from the origin. */
    static VALUE MakePerspective(VALUE rSelf, VALUE fov);

    static VALUE GetWorld(VALUE rSelf);

    static VALUE GetName(VALUE rSelf);

    static VALUE GetRatio(VALUE rSelf);

public:
    MHCameraBindings();
    virtual ~MHCameraBindings() {}
};

#endif
