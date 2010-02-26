/*
 *  Mountainhome.cpp
 *  Mountainhome
 *
 *  Created by Brent Wilson on 12/21/09.
 *  Copyright 2009 NVIDIA. All rights reserved.
 *
 */

#include <Base/Assertion.h>
#include <Base/Logger.h>

#include <Render/MaterialManager.h>
#include <Render/ModelManager.h>
#include <Render/Quad.h>
#include "Tile.h"

#include <Engine/Keyboard.h>

#include "Mountainhome.h"
#include "GameState.h"

#include "RubyStateProxy.h"
#include "RubyLogger.h"
#include "GameState.h"
#include "MHObject.h"
#include "MHWorld.h"


//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Mountainhome ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
VALUE Mountainhome::Class = NULL;
VALUE Mountainhome::Object = NULL;

void Mountainhome::SetupBindings() {
    Class = rb_define_class("Mountainhome", rb_cObject);
    rb_define_method(Class, "register_state", RUBY_METHOD_FUNC(Mountainhome::RegisterState), 2);
    rb_define_method(Class, "state=", RUBY_METHOD_FUNC(Mountainhome::SetState), 1);
    rb_define_method(Class, "exit", RUBY_METHOD_FUNC(Mountainhome::StopMainLoop), 0);
#if 0
    rb_include_module(Class, rb_intern("Singleton"));
    Object = rb_funcall(Class, rb_intern("instance"), 0);
#else
    Object = rb_class_new_instance(NULL, 0, Class);
#endif
#if 0
    rb_define_variable("$mountainhome", Object);
#else
    rb_gv_set("$mountainhome", Object);
#endif
}

VALUE Mountainhome::StopMainLoop(VALUE self) {
    Mountainhome::Get()->stopMainLoop();
    return self;
}

VALUE Mountainhome::SetState(VALUE self, VALUE name) {
    std::string strName = rb_string_value_cstr(&name);
    Mountainhome::Get()->setActiveState(strName);
    return name;
}

VALUE Mountainhome::RegisterState(VALUE self, VALUE state, VALUE name) {
    std::string strName = rb_string_value_cstr(&name);
    Info("Registering state " << RubyStateProxy::GetObject(state) << " under '" << strName << "'.");
    Mountainhome::Get()->registerState(RubyStateProxy::GetObject(state), strName);
    return state;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Mountainhome static declarations
//////////////////////////////////////////////////////////////////////////////////////////
const std::string Mountainhome::GameStateID = "GameState";

#define safe_return(x) if (!_instance.get()) { Warn("Returning "#x" as NULL."); } return _instance.get() ? Get()->x : NULL
Window *Mountainhome::GetWindow() { safe_return(_mainWindow); }
#undef safe_return

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Mountainhome declarations
//////////////////////////////////////////////////////////////////////////////////////////
Mountainhome::Mountainhome(): DefaultCore("Mountainhome") {}

Mountainhome::~Mountainhome() {}

void Mountainhome::setup(va_list args) {
    // Setup the logger how we want it.
    LogStream::SetLogLevel(LogStream::InfoMessage);
    LogStream::SetLogTarget("Mountainhome.log");

    // Set the name of the state.
    _name = "Mountainhome";

    // Register some default resources.
    // Some very basic materials to work with.
    Material *white = new Material();
	white->setColor(1.0f, 1.0f, 1.0f, 1.0f);
    white->setAmbient(1.0f, 1.0f, 1.0f);
    white->setDiffuse(1.0, 1.0, 1.0, 1.0);
	
	Material *red = new Material();
	red->setColor(1.0f, 0.0f, 0.0f, 1.0f);
    red->setAmbient(1.0f, 0.0f, 0.0f);
    red->setDiffuse(1.0, 0.0, 0.0, 1.0);

    //ModelManager::Get()->registerResource("tile", new Quad(1, 1));
    ModelManager::Get()->registerResource("tile_allup", new Tile(ALLUP));
	ModelManager::Get()->registerResource("tile_oneup", new Tile(ONEUP));
	ModelManager::Get()->registerResource("tile_twoadjup", new Tile(TWOADJUP));
	ModelManager::Get()->registerResource("tile_twooppup", new Tile(TWOOPPUP));
	ModelManager::Get()->registerResource("tile_threeup", new Tile(THREEUP));
	ModelManager::Get()->registerResource("tile_noneup", new Tile(NONEUP));
	ModelManager::Get()->registerResource("tile_wall", new Tile(WALL));
    
	MaterialManager::Get()->registerResource("white", white);
	MaterialManager::Get()->registerResource("red", red);

    // And setup our ruby bindings before calling down into our main ruby setup script.
    RubyStateProxy::SetupBindings();
    RubyLogger::SetupBindings();

    Mountainhome::SetupBindings();
    GameState::SetupBindings();
    MHObject::SetupBindings();
    MHWorld::SetupBindings();

    int state = 0;
    rb_protect(require_setup_wrapper, 0, &state);
    translate_ruby_exception(state);
}

void Mountainhome::keyPressed(KeyEvent *event) {
    switch(event->key()) {
    case Keyboard::KEY_q:
        stopMainLoop();
        break;
	case Keyboard::KEY_h:
		
		break;
	case Keyboard::KEY_j:
	
		break;
	case Keyboard::KEY_k:
	
		break;
	case Keyboard::KEY_l:
	
		break;
	default:
        ParentState::keyPressed(event);
    }
}
