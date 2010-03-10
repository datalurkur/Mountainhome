/*
 *  MHGameState.cpp
 *  System
 *
 *  Created by loch on 2/25/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "MHGameState.h"
#include "MHCore.h"
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
#pragma mark MHGameState ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHGameState::SetupBindings() {
    Class = rb_define_class("MHGameState", Class);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHGameState::Initialize), 0);
    rb_define_method(Class, "world=", RUBY_METHOD_FUNC(MHGameState::SetWorld), 1);
    rb_define_method(Class, "world", RUBY_METHOD_FUNC(MHGameState::GetWorld), 0);
}

VALUE MHGameState::Initialize(VALUE self) {
    RubyStateProxy::RegisterObject(self, new MHGameState(self));
    return self;
}

VALUE MHGameState::SetWorld(VALUE self, VALUE world) {
    MHGameState *state = (MHGameState*)GetObject(self);

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

VALUE MHGameState::GetWorld(VALUE self) {
    MHGameState *state = (MHGameState*)GetObject(self);
    return MHWorld::GetValue(state->_world);
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHGameState declarations
//////////////////////////////////////////////////////////////////////////////////////////
MHGameState::MHGameState(VALUE robj): RubyStateProxy(robj), _world(NULL), _yaw(0), _pitch(0) {}

MHGameState::~MHGameState() {}

void MHGameState::update(int elapsed) {
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
    int middleX = MHCore::GetWindow()->getWidth()  >> 1;
    int middleY = MHCore::GetWindow()->getHeight()  >> 1;
    Mouse::Get()->getMousePos(x, y);
    if ((x != middleX) || (y != middleY)) {
        Mouse::Get()->setMousePos(middleX, middleY);
        if (first) { first = false; } else {
            yaw   = Math::Radians(float((middleX - x) * rotSpeed) / float(middleX));
            pitch = Math::Radians(float((middleY - y) * rotSpeed) / float(middleY));
            _world->getCamera()->adjustYaw(_yaw * elapsed);
            _world->getCamera()->adjustPitch(_pitch * elapsed);
        }
    }
#endif
}

void MHGameState::keyPressed(KeyEvent *event) {
    static Real moveSpeed = 0.025;
    
    int mod = event->modifier();
    int key = event->key();
    bool flipped = mod == Keyboard::MOD_LSHIFT || mod == Keyboard::MOD_RSHIFT;

    switch(key) {
    case Keyboard::KEY_a:
    case Keyboard::KEY_LEFT:
        _move.x = -moveSpeed; break;
    case Keyboard::KEY_d:
    case Keyboard::KEY_RIGHT:
        _move.x =  moveSpeed; break;
    case Keyboard::KEY_w:
    case Keyboard::KEY_UP:
        if (flipped) { _move.y =  moveSpeed; } else { _move.z = -moveSpeed; }
        break;
    case Keyboard::KEY_s:
    case Keyboard::KEY_DOWN:
        if (flipped) { _move.y = -moveSpeed; } else { _move.z = moveSpeed; }
        break;
    case Keyboard::KEY_m:
        _world->toggleCameraZoom();
        break;
    case Keyboard::KEY_SPACE:
        _world->toggleCamera();
        break;
    }
}

void MHGameState::keyReleased(KeyEvent *event) {
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

void MHGameState::mouseMoved(MouseMotionEvent *event) {
    // We set the position of the mouse which causes a mouseMoved event to trigger. We
    // need to ignore this, though.
    static int count = 0;
    if (count++ < 3) { return; }

    static Real rotSpeed = -0.01;
    _yaw   = event->relX() * rotSpeed;
    _pitch = event->relY() * rotSpeed;
}
