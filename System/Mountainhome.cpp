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

#include <Engine/Keyboard.h>

#include "Mountainhome.h"
#include "GameState.h"

#include "Bindings.h"

const std::string Mountainhome::GameStateID = "GameState";

#define safe_return(x) if (!_instance.get()) { Warn("Returning "#x" as NULL."); } return _instance.get() ? Get()->x : NULL
Window *Mountainhome::GetWindow() { safe_return(_mainWindow); }
#undef safe_return

Mountainhome::Mountainhome(): DefaultCore("Mountainhome") {}

Mountainhome::~Mountainhome() {}

void Mountainhome::setup(va_list args) {
    // Setup the logger how we want it.
    LogStream::SetLogLevel(LogStream::InfoMessage);
    LogStream::SetLogTarget("Mountainhome.log");

    // Set the name of the state.
    _name = "Mountainhome";

    // And setup our ruby bindings before calling down into our main ruby setup script.
    MountainhomeBinding::SetupBindings();
    StateBinding::SetupBindings();
    rb_require("setup.rb");
}

void Mountainhome::keyPressed(KeyEvent *event) {
    switch(event->key()) {
    case Keyboard::KEY_q:
        stopMainLoop();
        break;
    default:
        ParentState::keyPressed(event);
    }
}
