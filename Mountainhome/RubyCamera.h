/*
 *  RubyCamera.h
 *  Mountainhome
 *
 *  Created by loch on 4/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RUBYCAMERA_H_
#define _RUBYCAMERA_H_
#include <Render/Camera.h>
#include "RubyBindings.h"

class RubyCamera : public RubyBindings<Camera, false> {
public:
    static void SetupBindings();
    static VALUE Initialize(VALUE self, VALUE world, VALUE level);
	static VALUE SetFixedYaw(VALUE self, VALUE x, VALUE y, VALUE z);
	static VALUE SetPosition(VALUE self, VALUE x, VALUE y, VALUE z);
	static VALUE LookAt(VALUE self, VALUE x, VALUE y, VALUE z);
	static VALUE RotateOnAxis(VALUE self, VALUE amt, VALUE x, VALUE y, VALUE z);
	static VALUE MoveRelative(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE AdjustYaw(VALUE rSelf, VALUE yaw);
    static VALUE AdjustPitch(VALUE rSelf, VALUE pitch);
    static VALUE AdjustRoll(VALUE rSelf, VALUE roll);
};

#endif
