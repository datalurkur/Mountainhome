/*
 *  RubyEntity.h
 *  Mountainhome
 *
 *  Created by Paul on 6/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RUBYENTITY_H_
#define _RUBYENTITY_H_
#include <Render/Entity.h>
#include "RubyBindings.h"

class RubyEntity : public RubyBindings<Entity, false> {

// FIXME: Need to add a ton of SceneNode bindings.
public:
    static void SetupBindings();

    static VALUE GetPosition(VALUE rSelf);
    static VALUE SetPosition(VALUE rSelf, VALUE x, VALUE y, VALUE z);
    static VALUE SetPositionX(VALUE rSelf, VALUE x);
    static VALUE SetPositionY(VALUE rSelf, VALUE y);
    static VALUE SetPositionZ(VALUE rSelf, VALUE z);

    static VALUE SetVisibility(VALUE rSelf, VALUE rState);
};

// A specialization that doesn't try to delete the Camera, which wouldn't compile.
template <> void RubyBindings<Entity, false>::Free(Entity* cobj);

#endif
