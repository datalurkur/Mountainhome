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
_activeCam(NULL), _width(width), _height(height), _depth(depth), _tiles(NULL) {
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
}

void MHWorld::updateTile(VALUE type, int x, int y, int z) {
    _tiles[coordsToIndex(x, y, z)].type = type;
}

MHWorld::Tile* MHWorld::getTile(int x, int y, int z) {
    return _tiles + coordsToIndex(x, y, z);
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

void MHWorld::initializeScene() {
    _scene = new MHSceneManager(this);
	Light *l = _scene->createLight("mainLight");
    ///\todo Make this a directional light.
    l->setAmbient(0.1f, 0.1f, 0.1f);
    l->setDiffuse(0.8f, 0.8f, 0.8f);
	l->setPosition(16, 16, 32);

	// Setup the camera
    _lCam = _scene->createCamera("leftCamera");
    _lCam->setFixedYawAxis(true, Vector3(0, 1, 0));
	_lCam->setPosition(Vector3(0, 0, 20));
	_lCam->setDirection(Vector3(0, 0, -1));

	_rCam = _scene->createCamera("rightCamera");
    _rCam->setFixedYawAxis(true, Vector3(0, 0, 1));
    _rCam->setPosition(Vector3(0, -10, 10));
    _rCam->setDirection(Vector3(0, 1, 0));

    // Set the active camera.
    _activeCam = _lCam;

	// Connect the camera to the window
	Mountainhome::GetWindow()->setBGColor(Color4(.4,.6,.8,1));
	Mountainhome::GetWindow()->addViewport(_scene->getCamera("leftCamera"), 0, 0.0f, 0.0f, 0.5f, 1.0f);
	Mountainhome::GetWindow()->addViewport(_scene->getCamera("rightCamera"), 1, 0.5f, 0.0f, 0.5f, 1.0f);
}

void MHWorld::populate() {
    _scene->populate();
}

int MHWorld::getWidth() { return _width; }
int MHWorld::getHeight() { return _height; }
int MHWorld::getDepth() { return _depth; }
