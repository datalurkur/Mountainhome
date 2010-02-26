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
#include <Engine/Keyboard.h>

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

    // Set the active camera.
    _activeCam = _lCam;

	// Connect the camera to the window
	Mountainhome::GetWindow()->setBGColor(Color4(.4,.6,.8,1));
	Mountainhome::GetWindow()->addViewport(_scene->getCamera("leftCamera"), 0, 0.0f, 0.0f, 0.5f, 1.0f);
	Mountainhome::GetWindow()->addViewport(_scene->getCamera("rightCamera"), 1, 0.5f, 0.0f, 0.5f, 1.0f);

    RubyStateProxy::setup(args);
}

void GameState::update(int elapsed) {
    _activeCam->moveRelative(_move * elapsed);
}

void GameState::teardown() {
    Mountainhome::GetWindow()->removeAllViewports();
}

void GameState::keyPressed(KeyEvent *event) {
    static Real moveSpeed = 0.01;
    
    int mod = event->modifier();
    int key = event->key();

    switch(key) {
    case Keyboard::KEY_UP:    if(mod == Keyboard::MOD_LCTRL or mod == Keyboard::MOD_RCTRL) _move.z = -moveSpeed; else _move.y =  moveSpeed; break;
    case Keyboard::KEY_DOWN:  if(mod == Keyboard::MOD_LCTRL or mod == Keyboard::MOD_RCTRL) _move.z =  moveSpeed; else _move.y = -moveSpeed; break;
    case Keyboard::KEY_LEFT:  _move.x = -moveSpeed; break;
    case Keyboard::KEY_RIGHT: _move.x =  moveSpeed; break;
    case Keyboard::KEY_SPACE:
        _activeCam = (_activeCam == _lCam) ? _rCam : _lCam;
        break;
    }
}

void GameState::keyReleased(KeyEvent *event) {
	switch(event->key()) {
	case Keyboard::KEY_UP:    _move.y = 0; _move.z = 0; break;
	case Keyboard::KEY_DOWN:  _move.y = 0; _move.z = 0; break;
	case Keyboard::KEY_LEFT:  _move.x = 0; break;
	case Keyboard::KEY_RIGHT: _move.x = 0; break;
	} 
}
