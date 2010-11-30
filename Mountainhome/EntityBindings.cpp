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
#include "MHUIElementBindings.h"
#include "MHActorBindings.h"

// Entities are SceneNodes, so inherit SceneNode's Ruby bindings.
EntityBindings::EntityBindings()
: RubyBindings<Entity, false>(
    rb_define_class("MHEntity", SceneNodeBindings::Get()->getClass()),
    "EntityBindings")
{
//  rb_define_method(_class, "get_parent", RUBY_METHOD_FUNC(EntityBindings::GetParent), 0);
//	rb_define_method(_class, "get_boundingBox", RUBY_METHOD_FUNC(EntityBindings::GetBoundingBox), 0);
//	rb_define_method(_class, "get_material", RUBY_METHOD_FUNC(EntityBindings::GetMaterial), 0);
//	
//	rb_define_method(_class, "set_node", RUBY_METHOD_FUNC(EntityBindings::SetNode), 1);
//	rb_define_method(_class, "set_material", RUBY_METHOD_FUNC(EntityBindings::SetMaterial), 1);

    rb_define_method(_class, "visible=", RUBY_METHOD_FUNC(EntityBindings::SetVisibility), 1);
}

Entity *EntityBindings::getPointer(VALUE rObj) {
    VALUE klass = rb_obj_class(rObj);
    if (klass == MHUIElementBindings::Get()->getClass()) { return MHUIElementBindings::Get()->getPointer(rObj); }
    if (klass ==     MHActorBindings::Get()->getClass()) { return     MHActorBindings::Get()->getPointer(rObj); }
    return RubyBindings<Entity, false>::getPointer(rObj);
}

//VALUE EntityBindings::GetParent(VALUE rSelf) {
//    Entity *cSelf = Get()->getPointer(rSelf);
//    // returns PositionableObject*; handle in Ruby-land? 
//    //return cSelf->getParent();
//	return rSelf;
//}
//
//VALUE EntityBindings::GetBoundingBox(VALUE rSelf) {
//    Entity *cSelf = Get()->getPointer(rSelf);
//    // returns const AABB3&; handle in Ruby-land?
//    //cSelf->getBoundingBox();
//	return rSelf;
//}
//
//VALUE EntityBindings::GetMaterial(VALUE rSelf) {
//    Entity *cSelf = Get()->getPointer(rSelf);
//    //returns Material*; convert to RubyMaterial? pack CMaterial in VALUE?
//    //cSelf->getMaterial();
//	return rSelf;
//}
//
//VALUE EntityBindings::SetNode(VALUE rSelf, VALUE rNode) {
//    Entity *cSelf = Get()->getPointer(rSelf);
//    // takes Node; needs RubyNode?
//    //Node *cNode = Get()->getPointer(rNode);    
//    //cSelf->setNode(cNode);
//	return rSelf;
//}
//
//VALUE EntityBindings::SetMaterial(VALUE rSelf, VALUE rMaterial) {
//    Entity *cSelf = Get()->getPointer(rSelf);
//    // takes Material; needs RubyMaterial?
//    //Material *cMaterial = Get()->getPointer(rMaterial);    
//    //cSelf->setMaterial(cMaterial);
//	return rSelf;
//}

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
