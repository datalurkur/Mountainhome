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
	rb_define_method(Class, "teardown", RUBY_METHOD_FUNC(MHGameState::Teardown), 0);
    rb_define_method(Class, "world=", RUBY_METHOD_FUNC(MHGameState::SetWorld), 1);
    rb_define_method(Class, "world", RUBY_METHOD_FUNC(MHGameState::GetWorld), 0);
	rb_define_method(Class, "manager=", RUBY_METHOD_FUNC(MHGameState::SetManager), 1);
	rb_define_method(Class, "manager", RUBY_METHOD_FUNC(MHGameState::GetManager), 0);
}

VALUE MHGameState::Initialize(VALUE self) {
    RubyStateProxy::RegisterObject(self, new MHGameState(self));
    return self;
}

VALUE MHGameState::Teardown(VALUE self) {
    Info("Tearing down GameState");
	MHCore::Get()->stopMainLoop();
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

    return rb_iv_set(self, "@world", world);
}

VALUE MHGameState::GetWorld(VALUE self) {
    MHGameState *state = (MHGameState*)GetObject(self);
    return MHWorld::GetValue(state->_world);
}

VALUE MHGameState::SetManager(VALUE self, VALUE manager) {
    MHGameState *state = (MHGameState*)GetObject(self);

    if (NIL_P(manager)) {
        // Delete the manager if given nil.
        delete state->_manager;
        state->_manager = NULL;
    } else if (state->_manager) {
        // This probably isn't what people someone wanted to do.
        THROW(InvalidStateError, "Memory leak!");
    } else {
        // The typical case. Set the world in the proper game state object.
        state->_manager = MHUIManager::GetObject(manager);
    }

    return rb_iv_set(self, "@manager", manager);
}

VALUE MHGameState::GetManager(VALUE self) {
    MHGameState *state = (MHGameState*)GetObject(self);
    return MHUIManager::GetValue(state->_manager);
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHGameState declarations
//////////////////////////////////////////////////////////////////////////////////////////
MHGameState::MHGameState(VALUE robj): RubyStateProxy(robj), _world(NULL), _manager(NULL) {}

MHGameState::~MHGameState() {}
