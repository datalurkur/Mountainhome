/*
 *  MHWorld.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "MHWorld.h"
#include "MHTerrain.h"
#include "MHCore.h"
#include "MHSceneManager.h"
#include "MHCamera.h"
#include "MHObject.h"

#include <Render/Light.h>
#include <Render/Camera.h>
#include <Engine/Window.h>

#include <Render/MaterialManager.h>
#include <Render/ModelManager.h>
#include <Render/Entity.h>
#include <Render/Light.h>
#include <Render/Node.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHWorld::SetupBindings() {
    Class = rb_define_class("MHWorld", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHWorld::Initialize), 3);
    rb_define_method(Class, "terrain", RUBY_METHOD_FUNC(MHWorld::GetTerrain), 0);
    rb_define_method(Class, "terrain=", RUBY_METHOD_FUNC(MHWorld::SetTerrain), 1);
    rb_define_method(Class, "populate", RUBY_METHOD_FUNC(MHWorld::Populate), 0);
}

VALUE MHWorld::Initialize(VALUE self, VALUE width, VALUE height, VALUE depth) {
    RegisterObject(self, new MHWorld(NUM2INT(width), NUM2INT(height), NUM2INT(depth)));
    return self;
}

VALUE MHWorld::GetTerrain(VALUE self) {
    MHWorld *world = (MHWorld*)GetObject(self);
    return MHTerrain::GetValue(world->_terrain);
}

VALUE MHWorld::SetTerrain(VALUE self, VALUE terrain) {
    MHWorld *world = (MHWorld*)GetObject(self);

    if (NIL_P(terrain)) {
        // Delete the terrain if given nil.
        delete world->_terrain;
        world->_terrain = NULL;
    } else if (world->_terrain) {
        // This probably isn't what people someone wanted to do.
        Error("Someone's trying to stomp on terrain!");
    } else {
        // The typical case. Set the terrain in the proper world object.
        world->_terrain = MHTerrain::GetObject(terrain);
    }

    return rb_iv_set(self, "@terrain", terrain);
}

VALUE MHWorld::Populate(VALUE self) {
    GetObject(self)->populate();
    return self;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld implementation
//////////////////////////////////////////////////////////////////////////////////////////
MHWorld::MHWorld(int width, int height, int depth): _scene(NULL), _width(width), _height(height), _depth(depth), _terrain(NULL), _split(true) {
    _materialManager = MHCore::GetMaterialManager();
    _modelManager = MHCore::GetModelManager();

    initializeScene();
}

MHWorld::~MHWorld() {
    MHCore::GetWindow()->removeAllViewports();
    delete _scene;
}

MHObject *MHWorld::createObject(const std::string &name, const std::string model, const std::string material) {
    Entity *entity = NULL;

    // Create the entity and add it to the scene.
    entity = _scene->createEntity(_modelManager->getOrLoadResource(model), name);
    entity->setMaterial(_materialManager->getOrLoadResource(material));
    _scene->getRootNode()->attach(entity);

    return new MHObject(name, this, entity);
}

MHSceneManager* MHWorld::getScene() const {
    return _scene;
}

MHTerrain* MHWorld::getTerrain() const {
    return _terrain;
}

#include "TestSceneManager2.h"
void MHWorld::initializeScene() {
    _scene = new TestSceneManager2(this, _materialManager);
	Light *l = _scene->createLight("mainLight");
    ///\todo Make this a directional light.
    l->setAmbient(0.1f, 0.1f, 0.1f);
    l->setDiffuse(0.8f, 0.8f, 0.8f);
	l->setPosition(16, 16, 32);

	MHCore::GetWindow()->setBGColor(Color4(.4,.6,.8,1));
    MHCore::GetWindow()->addViewport(NULL, 0, 0.0f, 0.0f, 1.0f, 1.0f);
}

void MHWorld::populate() {
    if(_terrain) {
        _scene->populate();
    }
    else {
        Error("Can't populate empty scene");
    }
}

int MHWorld::getWidth() { return _width; }
int MHWorld::getHeight() { return _height; }
int MHWorld::getDepth() { return _depth; }
