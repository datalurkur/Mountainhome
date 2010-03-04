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
}

VALUE MHWorld::Initialize(VALUE self, VALUE width, VALUE height, VALUE depth) {
    Vector3 dims(NUM2INT(width), NUM2INT(height), NUM2INT(depth));
    RegisterObject(self, new MHWorld(dims));
    return self;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld implementation
//////////////////////////////////////////////////////////////////////////////////////////
MHWorld::MHWorld(const Vector3 &dimensions): _scene(NULL), _lCam(NULL), _rCam(NULL),
_activeCam(NULL), _dimensions(dimensions) {
    _scene = new MHSceneManager();

	Light *l = _scene->createLight("mainLight");
    ///\todo Make this a directional light.
    l->setAmbient(0.1f, 0.1f, 0.1f);
    l->setDiffuse(0.8f, 0.8f, 0.8f);
	l->setPosition(16, 16, 32);

	// Setup the camera
    _lCam = _scene->createCamera("leftCamera");
    _lCam->setFixedYawAxis(true, Vector3(0, 0, 1));
	_lCam->setPosition(Vector3(20, 0, 15));
	_lCam->setDirection(Vector3(0, 1, 0));

	_rCam = _scene->createCamera("rightCamera");
    _rCam->setFixedYawAxis(true, Vector3(0, 1, 0));
    _rCam->setPosition(Vector3(20, 20, 20));
    _rCam->setDirection(Vector3(0, 0, -1));

    // Set the active camera.
    _activeCam = _lCam;

	// Connect the camera to the window
	Mountainhome::GetWindow()->setBGColor(Color4(.4,.6,.8,1));
	Mountainhome::GetWindow()->addViewport(_scene->getCamera("leftCamera"), 0, 0.0f, 0.0f, 0.5f, 1.0f);
	Mountainhome::GetWindow()->addViewport(_scene->getCamera("rightCamera"), 1, 0.5f, 0.0f, 0.5f, 1.0f);
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
