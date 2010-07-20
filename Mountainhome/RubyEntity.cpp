/*
 *  RubyEntity.cpp
 *  Mountainhome
 *
 *  Created by Paul on 6/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "RubyEntity.h"

void RubyEntity::SetupBindings() {
    Class = rb_define_class("MHEntity", rb_cObject);
	
//  rb_define_method(Class, "get_parent", RUBY_METHOD_FUNC(RubyEntity::GetParent), 0);
//	rb_define_method(Class, "get_boundingBox", RUBY_METHOD_FUNC(RubyEntity::GetBoundingBox), 0);
//	rb_define_method(Class, "get_material", RUBY_METHOD_FUNC(RubyEntity::GetMaterial), 0);
//	
//	rb_define_method(Class, "set_node", RUBY_METHOD_FUNC(RubyEntity::SetNode), 1);
//	rb_define_method(Class, "set_material", RUBY_METHOD_FUNC(RubyEntity::SetMaterial), 1);
	rb_define_method(Class, "set_position", RUBY_METHOD_FUNC(RubyEntity::SetPosition), 3);
}

//VALUE RubyEntity::GetParent(VALUE rSelf) {
//    AssignCObjFromValue(Entity, cSelf, rSelf);
//    // returns PositionableObject*; handle in Ruby-land? 
//    //return cSelf->getParent();
//	return rSelf;
//}
//
//VALUE RubyEntity::GetBoundingBox(VALUE rSelf) {
//    AssignCObjFromValue(Entity, cSelf, rSelf);
//    // returns const AABB3&; handle in Ruby-land?
//    //cSelf->getBoundingBox();
//	return rSelf;
//}
//
//VALUE RubyEntity::GetMaterial(VALUE rSelf) {
//    AssignCObjFromValue(Entity, cSelf, rSelf);
//    //returns Material*; convert to RubyMaterial? pack CMaterial in VALUE?
//    //cSelf->getMaterial();
//	return rSelf;
//}
//
//VALUE RubyEntity::SetNode(VALUE rSelf, VALUE rNode) {
//    AssignCObjFromValue(Entity, cSelf, rSelf);
//    // takes Node; needs RubyNode?
//    //AssignCObjFromValue(Node, cNode, rNode);    
//    //cSelf->setNode(cNode);
//	return rSelf;
//}
//
//VALUE RubyEntity::SetMaterial(VALUE rSelf, VALUE rMaterial) {
//    AssignCObjFromValue(Entity, cSelf, rSelf);
//    // takes Material; needs RubyMaterial?
//    //AssignCObjFromValue(Material, cMaterial, rMaterial);    
//    //cSelf->setMaterial(cMaterial);
//	return rSelf;
//}

VALUE RubyEntity::SetPosition(VALUE rSelf, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    cSelf->setPosition(Vector3(NUM2INT(x), NUM2INT(y), NUM2INT(z)));
	return rSelf;
}

template <>
void RubyBindings<Entity, false>::Free(Entity* cobj) {
    UnregisterPair(cobj);
}
