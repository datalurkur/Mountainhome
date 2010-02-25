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

#include <Render/OctreeScene.h>
#include <Render/Light.h>
#include <Render/Camera.h>
#include "Window.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHWorld::SetupBindings() {
    Class = rb_define_class("MHWorld", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHWorld::Initialize), 0);
	rb_define_method(Class, "teardown", RUBY_METHOD_FUNC(MHWorld::Teardown), 0);
	rb_define_method(Class, "setup", RUBY_METHOD_FUNC(MHWorld::Setup), 0);
}

VALUE MHWorld::Initialize(VALUE self) {
    MHWorld::RegisterObject(self, new MHWorld());
    return self;
}

VALUE MHWorld::Teardown(VALUE self) {
	GetObject(self)->teardown();
	return self;
}

VALUE MHWorld::Setup(VALUE self) {
	GetObject(self)->setup();
	return self;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld implementation
//////////////////////////////////////////////////////////////////////////////////////////
MHWorld::MHWorld(): _scene(NULL) {
    _scene = new OctreeScene();
}

MHWorld::~MHWorld() {}

Scene* MHWorld::getScene() {
    return _scene;
}

void MHWorld::setup() {
	Light *l = _scene->createLight("mainLight");
    ///\todo Make this a directional light.
    l->setAmbient(0.2f, 0.2f, 0.2f);
    l->setDiffuse(0.8f, 0.8f, 0.8f);
	l->setPosition(16, 16, 32);
	// Setup the camera
    Camera *lCam = _scene->createCamera("leftCamera");
    lCam->setPosition(Vector3(5, 5, 10));
    lCam->lookAt(Vector3(5, 5, 5));
	Camera *rCam = _scene->createCamera("rightCamera");
	rCam->setPosition(Vector3(5, 0, 5));
	rCam->lookAt(Vector3(5,5,5));
	
	// Connect the camera to the window
	Mountainhome::GetWindow()->setBGColor(Color4(.4,.6,.8,1));
	Mountainhome::GetWindow()->addViewport(_scene->getCamera("leftCamera"), 0, 0.0f, 0.0f, 0.5f, 1.0f);
	Mountainhome::GetWindow()->addViewport(_scene->getCamera("rightCamera"), 1, 0.5f, 0.0f, 0.5f, 1.0f);
}

void MHWorld::teardown() {
    Mountainhome::GetWindow()->removeAllViewports();
}
