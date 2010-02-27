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
#include <Engine/Mouse.h>

#include <Render/MaterialManager.h>
#include <Render/ModelManager.h>
#include <Render/Sphere.h>
#include <Render/Entity.h>
#include <Render/Light.h>
#include <Render/Node.h>

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
GameState::GameState(VALUE robj): RubyStateProxy(robj), _scene(NULL), _activeCam(NULL),
_lCam(NULL), _rCam(NULL), _yaw(0), _pitch(0) {
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
    _lCam->lookAt(Vector3(0, 0, 0));

    Info("[brent1] Position: " << _lCam->getPosition());
    Info("[brent1] Direction: " << _lCam->getDirection());

	_rCam = _scene->createCamera("rightCamera");
	_rCam->setPosition(Vector3(0, -10, 0));
	_rCam->lookAt(Vector3(0,0,0));

    Info("[brent1] Position: " << _rCam->getPosition());
    Info("[brent1] Direction: " << _rCam->getDirection());

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
 
# if 1
    // Handle mouse motion using events!
    _activeCam->rotate(Quaternion(_pitch * elapsed, _yaw * elapsed, 0));
    _yaw = _pitch = 0; // Clear the rotation data so we don't spin forever.
#else
    // Mouse motion with resetting the position.
    static const Real rotSpeed = 7;
    static bool first = true;

    Radian pitch, yaw; int x, y;
    int middleX = Mountainhome::GetWindow()->getWidth()  >> 1;
    int middleY = Mountainhome::GetWindow()->getHeight()  >> 1;
    Mouse::Get()->getMousePos(x, y);
    if ((x != middleX) || (y != middleY)) {
        Mouse::Get()->setMousePos(middleX, middleY);
        if (first) { first = false; } else {
            yaw   = Math::Radians(float((middleX - x) * rotSpeed) / float(middleX));
            pitch = Math::Radians(float((middleY - y) * rotSpeed) / float(middleY));
            _activeCam->rotate(Quaternion(pitch * elapsed, yaw * elapsed, 0));
        }
    }
#endif
}

void GameState::teardown() {
    Mountainhome::GetWindow()->removeAllViewports();
}

void GameState::keyPressed(KeyEvent *event) {
    static Real moveSpeed = 0.01;
    
    int mod = event->modifier();
    int key = event->key();

    switch(key) {
    case Keyboard::KEY_UP:    if(mod == Keyboard::MOD_LSHIFT or mod == Keyboard::MOD_RSHIFT) _move.y =  moveSpeed; else _move.z = -moveSpeed; break;
    case Keyboard::KEY_DOWN:  if(mod == Keyboard::MOD_LSHIFT or mod == Keyboard::MOD_RSHIFT) _move.y = -moveSpeed; else _move.z =  moveSpeed; break;
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

void GameState::mouseMoved(MouseMotionEvent *event) {
    // We set the position of the mouse which causes a mouseMoved event to trigger. We
    // need to ignore this, though.
    static int count = 0;
    if (count++ < 3) { return; }

    static Real rotSpeed = -0.01;
    _yaw   = event->relX() * rotSpeed;
    _pitch = event->relY() * rotSpeed;
}
