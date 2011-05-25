/*
 *  SceneNodeBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHSCENENODEBINDINGS_H_
#define _MHSCENENODEBINDINGS_H_
#include "RubyBindings.h"
#include "SceneNode.h"

class SceneNodeBindings : public RubyBindings<SceneNode, false> {
public:
    static VALUE SetDirection(VALUE rSelf, VALUE x, VALUE y, VALUE z);
	static VALUE SetFixedYaw(VALUE self, VALUE x, VALUE y, VALUE z);
	static VALUE LookAt(VALUE self, VALUE x, VALUE y, VALUE z);
	static VALUE RotateOnAxis(VALUE self, VALUE amt, VALUE x, VALUE y, VALUE z);
	static VALUE MoveAbsolute(VALUE self, VALUE x, VALUE y, VALUE z);
	static VALUE MoveRelative(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE AdjustYaw(VALUE rSelf, VALUE yaw);
    static VALUE AdjustPitch(VALUE rSelf, VALUE pitch);
    static VALUE AdjustRoll(VALUE rSelf, VALUE roll);

    static VALUE SetPosition(VALUE rSelf, VALUE x, VALUE y, VALUE z);
    static VALUE GetPosition(VALUE rSelf);

    static VALUE SetPositionX(VALUE rSelf, VALUE x);
    static VALUE SetPositionY(VALUE rSelf, VALUE y);
    static VALUE SetPositionZ(VALUE rSelf, VALUE z);

    static VALUE GetPositionX(VALUE rSelf);
    static VALUE GetPositionY(VALUE rSelf);
    static VALUE GetPositionZ(VALUE rSelf);

public:
    SceneNodeBindings();
    virtual ~SceneNodeBindings();
};

#endif
