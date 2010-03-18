/*
 *  MHCore.cpp
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

#include <Engine/Keyboard.h>

#include "MHCore.h"
#include "MHGameState.h"

#include "RubyStateProxy.h"
#include "RubyLogger.h"
#include "RubyKeyboard.h"

#include "MHConceptState.h"
#include "MHObject.h"
#include "MHWorld.h"

#include "MHUIElement.h"
#include "MHUIManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHCore ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
VALUE MHCore::Class = NULL;
VALUE MHCore::Object = NULL;

void MHCore::SetupBindings() {
    Class = rb_define_class("MHCore", rb_cObject);
    rb_define_method(Class, "register_state", RUBY_METHOD_FUNC(MHCore::RegisterState), 2);
    rb_define_method(Class, "state=", RUBY_METHOD_FUNC(MHCore::SetState), 1);
    rb_define_method(Class, "exit", RUBY_METHOD_FUNC(MHCore::StopMainLoop), 0);
#if 0
    rb_include_module(Class, rb_intern("Singleton"));
    Object = rb_funcall(Class, rb_intern("instance"), 0);
#else
    Object = rb_class_new_instance(NULL, 0, Class);
#endif
#if 0
    rb_define_variable("$mhcore", Object);
#else
    rb_gv_set("$mhcore", Object);
#endif
}

VALUE MHCore::StopMainLoop(VALUE self) {
    MHCore::Get()->stopMainLoop();
    return self;
}

VALUE MHCore::SetState(VALUE self, VALUE name) {
    std::string strName = rb_string_value_cstr(&name);
    MHCore::Get()->setActiveState(strName);
    return name;
}

VALUE MHCore::RegisterState(VALUE self, VALUE state, VALUE name) {
    std::string strName = rb_string_value_cstr(&name);
    Info("Registering state " << RubyStateProxy::GetObject(state) << " under '" << strName << "'.");
    MHCore::Get()->registerState(RubyStateProxy::GetObject(state), strName);
    return state;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHCore static declarations
//////////////////////////////////////////////////////////////////////////////////////////
#define safe_return(x) if (!_instance.get()) { Warn("Returning "#x" as NULL."); } return _instance.get() ? Get()->x : NULL
Window *MHCore::GetWindow() { safe_return(_mainWindow); }
#undef safe_return

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHCore declarations
//////////////////////////////////////////////////////////////////////////////////////////
MHCore::MHCore(): DefaultCore("Mountainhome") {}

MHCore::~MHCore() {}

MHConceptState *state = NULL;
void MHCore::setup(va_list args) {
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
	
	Material *blue = new Material();
	blue->setColor(0.0f,0.0f,1.0f,1.0f);
	blue->setAmbient(0.0f,1.0f,0.0f);
	blue->setDiffuse(0.0f,1.0f,0.0f,1.0f);

	MaterialManager::Get()->registerResource("white", white);
	MaterialManager::Get()->registerResource("red", red);
	MaterialManager::Get()->registerResource("blue", blue);

    // And setup our ruby bindings before calling down into our main ruby setup script.
    RubyStateProxy::SetupBindings();
    RubyLogger::SetupBindings();
    RubyKeyboard::SetupBindings();

    MHCore::SetupBindings();
    MHGameState::SetupBindings();
    MHObject::SetupBindings();
    MHWorld::SetupBindings();

    MHUIElement::SetupBindings();
    MHUIManager::SetupBindings();

#if 1
    rb_require("Mountainhome");
#else
    registerState(new MHConceptState(), "MHConceptState");
    setActiveState("MHConceptState");
#endif
}

void MHCore::keyPressed(KeyEvent *event) {
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
