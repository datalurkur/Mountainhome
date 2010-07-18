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

// FIXME: Need to add a ton of PositionableObject bindings.
public:
    static void SetupBindings();
	
	static VALUE GetParent(VALUE rSelf);
	static VALUE GetBoundingBox(VALUE rSelf);
	static VALUE GetMaterial(VALUE rSelf);
	
	static VALUE SetNode(VALUE rSelf, VALUE rNode);
	static VALUE SetMaterial(VALUE rSelf, VALUE rMaterial);

    static VALUE SetPosition(VALUE rSelf, VALUE x, VALUE y, VALUE z);

	static VALUE UpdateImplementationValues(VALUE rSelf);
};

#endif

