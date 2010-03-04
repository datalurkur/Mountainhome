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
#include "MHWorld.h"

#include "MHSceneManager.h"
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
    rb_define_method(Class, "world=", RUBY_METHOD_FUNC(GameState::SetWorld), 1);
    rb_define_method(Class, "world", RUBY_METHOD_FUNC(GameState::GetWorld), 0);
}

VALUE GameState::Initialize(VALUE self) {
    RubyStateProxy::RegisterObject(self, new GameState(self));
    return self;
}

VALUE GameState::SetWorld(VALUE self, VALUE world) {
    GameState *state = (GameState*)GetObject(self);

    if (NIL_P(world)) {
        // Delete the world if given nil.
        delete state->_world;
        state->_world = NULL;
    } else if (state->_world) {
        // This probably isn't what people someone wanted to do.
        THROW(InvalidStateError, "Memory leak!");
    } else {
        // The typical case. Set the world in the proper game state object.
        state->_world = MHWorld::GetObject(world);
    }

    return world;
}

VALUE GameState::GetWorld(VALUE self) {
    GameState *state = (GameState*)GetObject(self);
    return MHWorld::GetValue(state->_world);
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark GameState declarations
//////////////////////////////////////////////////////////////////////////////////////////
GameState::GameState(VALUE robj): RubyStateProxy(robj), _world(NULL), _yaw(0), _pitch(0) {}

GameState::~GameState() {}

void GameState::update(int elapsed) {
    // Handle camera movement.
    _world->getCamera()->moveRelative(_move * elapsed);
 
# if 1
    // Handle mouse motion using events!
    _world->getCamera()->adjustYaw(_yaw * elapsed);
    _world->getCamera()->adjustPitch(_pitch * elapsed);
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
            _world->getCamera()->rotate(Quaternion(pitch * elapsed, yaw * elapsed, 0));
        }
    }
#endif
}

void GameState::keyPressed(KeyEvent *event) {
    static Real moveSpeed = 0.01;
    
    int mod = event->modifier();
    int key = event->key();

    switch(key) {
    case Keyboard::KEY_w:
    case Keyboard::KEY_UP:    if(mod == Keyboard::MOD_LSHIFT or mod == Keyboard::MOD_RSHIFT) _move.y =  moveSpeed; else _move.z = -moveSpeed; break;
    case Keyboard::KEY_s:
    case Keyboard::KEY_DOWN:  if(mod == Keyboard::MOD_LSHIFT or mod == Keyboard::MOD_RSHIFT) _move.y = -moveSpeed; else _move.z =  moveSpeed; break;
    case Keyboard::KEY_a:
    case Keyboard::KEY_LEFT:  _move.x = -moveSpeed; break;
    case Keyboard::KEY_d:
    case Keyboard::KEY_RIGHT: _move.x =  moveSpeed; break;
    case Keyboard::KEY_SPACE:
        _world->toggleCamera();
        break;
    }
}

void GameState::keyReleased(KeyEvent *event) {
	switch(event->key()) {
    case Keyboard::KEY_w:
	case Keyboard::KEY_UP:    _move.y = 0; _move.z = 0; break;
    case Keyboard::KEY_s:
	case Keyboard::KEY_DOWN:  _move.y = 0; _move.z = 0; break;
    case Keyboard::KEY_a:
	case Keyboard::KEY_LEFT:  _move.x = 0; break;
    case Keyboard::KEY_d:
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
