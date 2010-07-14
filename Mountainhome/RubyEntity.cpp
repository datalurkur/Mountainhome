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
	
	rb_define_method(Class, "getParent", RUBY_METHOD_FUNC(RubyEntity::GetParent), 0);
	rb_define_method(Class, "getBoundingBox", RUBY_METHOD_FUNC(RubyEntity::GetBoundingBox), 0);
	rb_define_method(Class, "getMaterial", RUBY_METHOD_FUNC(RubyEntity::GetMaterial), 0);
	
	rb_define_method(Class, "setNode", RUBY_METHOD_FUNC(RubyEntity::SetNode), 1);
	rb_define_method(Class, "setMaterial", RUBY_METHOD_FUNC(RubyEntity::SetMaterial), 1);

	rb_define_method(Class, "updateImplementationValues", RUBY_METHOD_FUNC(RubyEntity::UpdateImplementationValues), 1);
}

VALUE RubyEntity::GetParent(VALUE rSelf) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    // returns PositionableObject*; handle in Ruby-land? 
    //return cSelf->getParent();
	return rSelf;
}

VALUE RubyEntity::GetBoundingBox(VALUE rSelf) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    // returns const AABB3&; handle in Ruby-land?
    //cSelf->getBoundingBox();
	return rSelf;
}

VALUE RubyEntity::GetMaterial(VALUE rSelf) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    //returns Material*; convert to RubyMaterial? pack CMaterial in VALUE?
    //cSelf->getMaterial();
	return rSelf;
}

VALUE RubyEntity::SetNode(VALUE rSelf, VALUE rNode) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    // takes Node; needs RubyNode?
    //AssignCObjFromValue(Node, cNode, rNode);    
    //cSelf->setNode(cNode);
	return rSelf;
}

VALUE RubyEntity::SetMaterial(VALUE rSelf, VALUE rMaterial) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    // takes Material; needs RubyMaterial?
    //AssignCObjFromValue(Material, cMaterial, rMaterial);    
    //cSelf->setMaterial(cMaterial);
	return rSelf;
}

VALUE RubyEntity::UpdateImplementationValues(VALUE rSelf) {
    AssignCObjFromValue(Entity, cSelf, rSelf);
    cSelf->updateImplementationValues();
	return rSelf;
}
