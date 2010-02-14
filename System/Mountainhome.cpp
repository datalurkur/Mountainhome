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

const std::string Mountainhome::GameStateID = "GameState";

#define safe_return(x) if (!_instance.get()) { Warn("Returning "#x" as NULL."); } return _instance.get() ? GetSingleton()->x : NULL
Window *Mountainhome::GetWindow() { safe_return(_mainWindow); }
#undef safe_return

Mountainhome::Mountainhome(): DefaultCore("Mountainhome") {}

Mountainhome::~Mountainhome() {}

void Mountainhome::setup(va_list args) {
    // Setup the logger how we want it.
    LogStream::SetLogLevel(LogStream::InfoMessage);
    LogStream::SetLogTarget("Mountainhome.log");

    // Set the name of the state and register the children.
    _name = "Mountainhome";
    registerState(new GameState(), GameStateID); ///\todo Create the main window.

    // Set our active state and return.
    setActiveState(GameStateID);
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
