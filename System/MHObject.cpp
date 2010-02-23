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

#include <Render/MaterialManager.h>
#include <Render/ModelManager.h>
#include <Render/Entity.h>
#include <Render/Scene.h>
#include <Render/Node.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHObject ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHObject::SetupBindings() {
    Class = rb_define_class("MHObject", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHObject::Initialize), 4);
	rb_define_method(Class, "set_position", RUBY_METHOD_FUNC(MHObject::SetPosition), 3);
}

VALUE MHObject::Initialize(VALUE self, VALUE name, VALUE world, VALUE model, VALUE mat) {
    std::string strName  = rb_string_value_cstr(&name);
    std::string strModel = rb_string_value_cstr(&model);
    std::string strMat   = rb_string_value_cstr(&mat);
    MHWorld *objWorld    = MHWorld::GetObject(world);

    MHObject::RegisterObject(self, new MHObject(strName, objWorld, strModel, strMat));
    return self;
}

VALUE MHObject::SetPosition(VALUE self, VALUE x, VALUE y, VALUE z) {
	GetObject(self)->_entity->setPosition(NUM2INT(x), NUM2INT(y), NUM2INT(z));
	return self;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHObject implementation
//////////////////////////////////////////////////////////////////////////////////////////
MHObject::MHObject(const std::string name, MHWorld *world, const std::string model,
const std::string material): _world(world), _entity(NULL) {
    Scene *scene = _world->getScene();

    // Create the entity and add it to the scene.
    _entity = scene->createEntity(ModelManager::Get()->loadResource(model), name);
    _entity->setMaterial(MaterialManager::Get()->loadResource(material));

    scene->getRootNode()->attach(_entity);
}

MHObject::~MHObject() {}
