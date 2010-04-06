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
#include "MHCore.h"
#include "MHSceneManager.h"

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
    rb_define_method(Class, "update_tile", RUBY_METHOD_FUNC(MHWorld::UpdateTile), 4);
    rb_define_method(Class, "populate", RUBY_METHOD_FUNC(MHWorld::Populate), 0);
    rb_define_method(Class, "camera", RUBY_METHOD_FUNC(MHWorld::GetCamera), 0);
    rb_define_method(Class, "width", RUBY_METHOD_FUNC(MHWorld::GetWidth), 0);
    rb_define_method(Class, "height", RUBY_METHOD_FUNC(MHWorld::GetHeight), 0);
    rb_define_method(Class, "depth", RUBY_METHOD_FUNC(MHWorld::GetDepth), 0);
    rb_define_alloc_func(Class, MHWorld::Alloc);
}

void MHWorld::Mark(MHWorld* world) {
    rb_gc_mark(RubyCamera::GetValue(world->_camera));
}

VALUE MHWorld::Initialize(VALUE rSelf, VALUE width, VALUE height, VALUE depth, VALUE rCore) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    AssignCObjFromValue(MHCore, cCore, rCore);

    cSelf->initialize(NUM2INT(width), NUM2INT(height), NUM2INT(depth), cCore);
    CreateBindingPair(RubyCamera, cSelf->_camera);

    return rSelf;
}

VALUE MHWorld::UpdateTile(VALUE rSelf, VALUE type, VALUE x, VALUE y, VALUE z) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    cSelf->updateTile(type, NUM2INT(x), NUM2INT(y), NUM2INT(z));
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
_camera(NULL), _width(0), _height(0), _depth(0), _tiles(NULL) {}

MHWorld::~MHWorld() {
    delete[] _tiles;
    delete _scene;
}

void MHWorld::initialize(int width, int height, int depth, MHCore *core) {
    _width = width;
    _height = height;
    _depth = depth;

    _materialManager = core->getMaterialManager();
    _modelManager = core->getModelManager();

    initializeTiles();
    initializeScene();
}

MHSceneManager* MHWorld::getScene() const {
    return _scene;
}

void MHWorld::updateTile(VALUE type, int x, int y, int z) {
    _tiles[coordsToIndex(x, y, z)].type = type;
}

MHWorld::Tile* MHWorld::getTile(int x, int y, int z) {
    // Boundry checking.
    if (x < 0 || x >= _width ) { return NULL; }
    if (y < 0 || y >= _height) { return NULL; }
    if (z < 0 || z >= _depth ) { return NULL; }
    return _tiles + coordsToIndex(x, y, z);
}

MHWorld::Tile* MHWorld::getTopTile(int x, int y) {
    // Boundry checking.
    if (x < 0 || x >= _width ) { return NULL; }
    if (y < 0 || y >= _height) { return NULL; }

    // Look from top to bottom for the first filled in tile.
    for (int z = _depth - 1; z >= 0; z--) {
        Tile *t = getTile(x, y, z);
        if (t->isFilled()) {
            return t;
        }
    }

    THROW(InvalidStateError, "Bottomless pit!!!!");
    return NULL;
}

int MHWorld::coordsToIndex(int x, int y, int z) {
    return z * _width * _height + y * _width + x;
}

void MHWorld::initializeTiles() {
    _tiles = new Tile[_width * _height * _depth];
    for (int x = 0; x < _width; x++) {
        for (int y = 0; y < _height; y++) {
            for (int z = 0; z < _depth; z++) {
                Tile *t = getTile(x, y, z);
                t->parent = this;
                t->type = NULL;
                t->x = x;
                t->y = y;
                t->z = z;
            }
        }
    }
}

#include "TestSceneManager1.h"
#include "TestSceneManager2.h"
#include "TestSceneManager3.h"
void MHWorld::initializeScene() {
    _scene = new TestSceneManager2(this, _materialManager);
    _camera = _scene->createCamera("MainCamera");

	Light *l = _scene->createLight("mainLight");
    ///\todo Make this a directional light.
    l->setAmbient(0.1f, 0.1f, 0.1f);
    l->setDiffuse(0.8f, 0.8f, 0.8f);
	l->setPosition(16, 16, 32);
}

void MHWorld::populate() {
    _scene->populate();
}

int MHWorld::getWidth() { return _width; }
int MHWorld::getHeight() { return _height; }
int MHWorld::getDepth() { return _depth; }
