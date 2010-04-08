/*
 *  MHWorld.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RubyCamera.h"

#include "MHWorld.h"
#include "OctreeTerrain.h"
#include "MHCore.h"
#include "OctreeSceneManager.h"

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

    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHWorld::Initialize), 4);
    rb_define_method(Class, "terrain", RUBY_METHOD_FUNC(MHWorld::GetTerrain), 0);

    rb_define_method(Class, "populate", RUBY_METHOD_FUNC(MHWorld::Populate), 0);
    rb_define_method(Class, "camera", RUBY_METHOD_FUNC(MHWorld::GetCamera), 0);
    rb_define_method(Class, "width", RUBY_METHOD_FUNC(MHWorld::GetWidth), 0);
    rb_define_method(Class, "height", RUBY_METHOD_FUNC(MHWorld::GetHeight), 0);
    rb_define_method(Class, "depth", RUBY_METHOD_FUNC(MHWorld::GetDepth), 0);
    rb_define_alloc_func(Class, MHWorld::Alloc);
}

void MHWorld::Mark(MHWorld* world) {
    rb_gc_mark(RubyCamera::GetValue(world->_camera));
    rb_gc_mark(MHTerrain::GetValue(world->_terrain));
}

VALUE MHWorld::Initialize(VALUE rSelf, VALUE width, VALUE height, VALUE depth, VALUE rCore) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    AssignCObjFromValue(MHCore, cCore, rCore);

    cSelf->initialize(NUM2INT(width), NUM2INT(height), NUM2INT(depth), cCore);
    CreateBindingPair(RubyCamera, cSelf->_camera);
    CreateBindingPair(MHTerrain, cSelf->_terrain);

    return rSelf;
}

VALUE MHWorld::Populate(VALUE rSelf) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    cSelf->populate();
    return rSelf;
}

VALUE MHWorld::GetCamera(VALUE rSelf) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    return RubyCamera::GetValue(cSelf->_camera);
}

VALUE MHWorld::GetTerrain(VALUE rSelf) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    return MHTerrain::GetValue(cSelf->_terrain);
}

VALUE MHWorld::GetWidth(VALUE rSelf) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    return INT2NUM(cSelf->_width);
}

VALUE MHWorld::GetHeight(VALUE rSelf) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    return INT2NUM(cSelf->_height);
}

VALUE MHWorld::GetDepth(VALUE rSelf) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    return INT2NUM(cSelf->_depth);
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld implementation
//////////////////////////////////////////////////////////////////////////////////////////
MHWorld::MHWorld(): _materialManager(NULL), _modelManager(NULL), _scene(NULL),
_camera(NULL), _width(0), _height(0), _depth(0), _terrain(NULL) {}

MHWorld::~MHWorld() {
    delete _scene;   _scene   = NULL;
    delete _terrain; _terrain = NULL;
}

void MHWorld::initialize(int width, int height, int depth, MHCore *core) {
    _width = width;
    _height = height;
    _depth = depth;

    _materialManager = core->getMaterialManager();
    _modelManager = core->getModelManager();

    _terrain = new OctreeTerrain(_width, _height, _depth);
    _scene = new OctreeSceneManager();

    _camera = _scene->createCamera("MainCamera");

	Light *l = _scene->createLight("mainLight");
    ///\todo Make this a directional light.
    l->setAmbient(0.1f, 0.1f, 0.1f);
    l->setDiffuse(0.9f, 0.9f, 0.9f);
	l->makeDirectionalLight(Vector3(5, 5, -5));
}

OctreeSceneManager* MHWorld::getScene() const {
    return _scene;
}

MHTerrain* MHWorld::getTerrain() const {
    return _terrain;
}

void MHWorld::populate() {
    _terrain->populate(_scene, _materialManager);
}

int MHWorld::getWidth() { return _width; }
int MHWorld::getHeight() { return _height; }
int MHWorld::getDepth() { return _depth; }
