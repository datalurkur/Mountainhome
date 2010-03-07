/*
 *  MHWorld.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "MHWorld.h"
#include "Mountainhome.h"
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
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHWorld::Initialize), 3);
    rb_define_method(Class, "update_tile", RUBY_METHOD_FUNC(MHWorld::UpdateTile), 4);
    rb_define_method(Class, "populate", RUBY_METHOD_FUNC(MHWorld::Populate), 0);
}

VALUE MHWorld::Initialize(VALUE self, VALUE width, VALUE height, VALUE depth) {
    RegisterObject(self, new MHWorld(NUM2INT(width), NUM2INT(height), NUM2INT(depth)));
    return self;
}

VALUE MHWorld::UpdateTile(VALUE self, VALUE type, VALUE x, VALUE y, VALUE z) {
    GetObject(self)->updateTile(type, NUM2INT(x), NUM2INT(y), NUM2INT(z));
    return self;
}

VALUE MHWorld::Populate(VALUE self) {
    GetObject(self)->populate();
    return self;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld implementation
//////////////////////////////////////////////////////////////////////////////////////////
MHWorld::MHWorld(int width, int height, int depth): _scene(NULL), _lCam(NULL), _rCam(NULL),
_activeCam(NULL), _width(width), _height(height), _depth(depth), _tiles(NULL), _split(true) {
    initializeTiles();
    initializeScene();
}

MHWorld::~MHWorld() {}

MHSceneManager* MHWorld::getScene() const {
    return _scene;
}

Camera* MHWorld::getCamera() const {
    return _activeCam;
}

void MHWorld::toggleCamera() {
    _activeCam = (_activeCam == _lCam) ? _rCam : _lCam;
    updateViewports();
}

void MHWorld::toggleCameraZoom() {
    _split = !_split;
    updateViewports();
}

void MHWorld::updateViewports() {
    if (_split) {
        Mountainhome::GetWindow()->removeAllViewports();
        Mountainhome::GetWindow()->addViewport(_lCam, 0, 0.0f, 0.0f, 0.5f, 1.0f);
        Mountainhome::GetWindow()->addViewport(_rCam, 1, 0.5f, 0.0f, 0.5f, 1.0f);
    } else {
        Mountainhome::GetWindow()->removeAllViewports();
        Mountainhome::GetWindow()->addViewport(_activeCam, 0, 0.0f, 0.0f, 1.0f, 1.0f);
    }
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
    _scene = new TestSceneManager2(this);
	Light *l = _scene->createLight("mainLight");
    ///\todo Make this a directional light.
    l->setAmbient(0.1f, 0.1f, 0.1f);
    l->setDiffuse(0.8f, 0.8f, 0.8f);
	l->setPosition(16, 16, 32);

	// Setup the camera
    _lCam = _scene->createCamera("leftCamera");
    _lCam->setFixedYawAxis(true, Vector3(0, 0, 1));
    _lCam->setPosition(Vector3(_width * 0.25, _height * 0.25, _depth));
    _lCam->lookAt(Vector3(_width * 0.5, _height * 0.5, 0));

	_rCam = _scene->createCamera("rightCamera");
    _rCam->setFixedYawAxis(true, Vector3(0, 1, 0));
	_rCam->setPosition(Vector3(_width / 2.0, _height / 2.0, _depth * 2));
	_rCam->setDirection(Vector3(0, 0, -1));

    // Set the active camera.
    _activeCam = _lCam;

	// Connect the camera to the window
	Mountainhome::GetWindow()->setBGColor(Color4(.4,.6,.8,1));
    updateViewports();
}

void MHWorld::populate() {
    _scene->populate();
}

int MHWorld::getWidth() { return _width; }
int MHWorld::getHeight() { return _height; }
int MHWorld::getDepth() { return _depth; }
