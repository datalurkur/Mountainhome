/*
 *  EntityBindings.cpp
 *  Mountainhome
 *
 *  Created by Paul on 6/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "EntityBindings.h"
#include "SceneNodeBindings.h"
#include "MHActorBindings.h"

// Entities are SceneNodes, so inherit SceneNode's Ruby bindings.
EntityBindings::EntityBindings()
: RubyBindings<Entity, false>(
    rb_define_class("MHEntity", SceneNodeBindings::Get()->getClass()),
    "EntityBindings")
{
    rb_define_method(_class, "visible=", RUBY_METHOD_FUNC(EntityBindings::SetVisibility), 1);
}

Entity *EntityBindings::getPointer(VALUE rObj) {
    VALUE klass = rb_obj_class(rObj);
    if (klass ==     MHActorBindings::Get()->getClass()) { return     MHActorBindings::Get()->getPointer(rObj); }
    return RubyBindings<Entity, false>::getPointer(rObj);
}

VALUE EntityBindings::SetVisibility(VALUE rSelf, VALUE rState) {
    Entity *cSelf = Get()->getPointer(rSelf);

    bool cState = (rState == Qtrue) ? true : false;
    cSelf->setVisibility(cState);

    return rSelf;
}

template <>
void RubyBindings<Entity, false>::Free(Entity* cobj) {
    Get()->unregisterPair(cobj);
}
