/*
 *  MHObject.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "MHObject.h"
#include "MHWorld.h"
#include "MHSceneManager.h"

#include <Render/Entity.h>
#include <Render/Node.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHObject ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHObject::SetupBindings() {
    Class = rb_define_class("MHObject", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHObject::Initialize), 4);
	rb_define_method(Class, "set_position", RUBY_METHOD_FUNC(MHObject::SetPosition), 3);
	rb_define_method(Class, "rotate", RUBY_METHOD_FUNC(MHObject::Rotate),4);
}

VALUE MHObject::Initialize(VALUE self, VALUE name, VALUE world, VALUE model, VALUE mat) {
    std::string strName  = rb_string_value_cstr(&name);
    std::string strModel = rb_string_value_cstr(&model);
    std::string strMat   = rb_string_value_cstr(&mat);
    MHWorld *objWorld    = MHWorld::GetObject(world);

    ///\todo Don't call MHObject.new anymore. Instead, call create_object on the world in
    //  ruby and have it bind directly to MHWorld::createObject.
    MHObject::RegisterObject(self, objWorld->createObject(strName, strModel, strMat));
    return self;
}

VALUE MHObject::SetPosition(VALUE self, VALUE x, VALUE y, VALUE z) {
	GetObject(self)->_entity->setPosition(NUM2INT(x), NUM2INT(y), NUM2INT(z));
	return self;
}

VALUE MHObject::Rotate(VALUE self, VALUE angle, VALUE x_axis, VALUE y_axis, VALUE z_axis) {
	GetObject(self)->_entity->rotate((Degree)NUM2INT(angle), Vector3(NUM2INT(x_axis),NUM2INT(y_axis),NUM2INT(z_axis)));
	return self;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHObject implementation
//////////////////////////////////////////////////////////////////////////////////////////
MHObject::MHObject(const std::string name, MHWorld *world, Entity *entity):
_name(name), _world(world), _entity(entity) {}

MHObject::~MHObject() {}
