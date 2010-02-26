/*
 *  GameState.cpp
 *  System
 *
 *  Created by loch on 2/25/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "GameState.h"
#include "Mountainhome.h"

#include <Render/OctreeScene.h>
#include <Render/Light.h>
#include <Render/Camera.h>
#include <Engine/Window.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark GameState ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void GameState::SetupBindings() {
    Class = rb_define_class("GameState", Class);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(GameState::Initialize), 0);
    rb_define_method(Class, "scene", RUBY_METHOD_FUNC(GameState::GetScene), 0);
}

VALUE GameState::Initialize(VALUE self) {
    RubyStateProxy::RegisterObject(self, new GameState(self));
    return self;
}

VALUE GameState::GetScene(VALUE self) {
    GameState *state = (GameState*)RubyStateProxy::GetObject(self);
    return SIZET2NUM((size_t)(state->_scene));
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark GameState declarations
//////////////////////////////////////////////////////////////////////////////////////////
GameState::GameState(VALUE robj): RubyStateProxy(robj), _scene(NULL), _lCam(NULL), _rCam(NULL) {
    _scene = new OctreeScene();
}

GameState::~GameState() {}

void GameState::setup(va_list args) {
	Light *l = _scene->createLight("mainLight");
    ///\todo Make this a directional light.
    l->setAmbient(0.1f, 0.1f, 0.1f);
    l->setDiffuse(0.8f, 0.8f, 0.8f);
	l->setPosition(16, 16, 32);

	// Setup the camera
    _lCam = _scene->createCamera("leftCamera");
    _lCam->setPosition(Vector3(5, 5, 10));
    _lCam->lookAt(Vector3(5, 5, 5));

	_rCam = _scene->createCamera("rightCamera");
	_rCam->setPosition(Vector3(5, 1, 5));
	_rCam->lookAt(Vector3(5,5,5));
	
	// Connect the camera to the window
	Mountainhome::GetWindow()->setBGColor(Color4(.4,.6,.8,1));
	Mountainhome::GetWindow()->addViewport(_scene->getCamera("leftCamera"), 0, 0.0f, 0.0f, 0.5f, 1.0f);
	Mountainhome::GetWindow()->addViewport(_scene->getCamera("rightCamera"), 1, 0.5f, 0.0f, 0.5f, 1.0f);

    RubyStateProxy::setup(args);
}

void GameState::teardown() {
    Mountainhome::GetWindow()->removeAllViewports();
}

void GameState::keyPressed(KeyEvent *event) {}
