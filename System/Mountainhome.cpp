/*
 *  Mountainhome.cpp
 *  Base
 *
 *  Created by Brent Wilson on 12/21/09.
 *  Copyright 2009 NVIDIA. All rights reserved.
 *
 */

#include <Base/Logger.h>
#include <Engine/Keyboard.h>

#include "Mountainhome.h"
#include "GameState.h"
#include "MenuState.h"

const std::string Mountainhome::GameStateID = "GameState";
const std::string Mountainhome::MenuStateID = "MenuState";

Mountainhome::Mountainhome(): SimpleCore(800, 600, false, "Mountainhome") {
    // Setup the logger how we want it.
    LogStream::SetLogLevel(LogStream::InfoMessage);
    LogStream::SetLogTarget("Mountainhome.log");

    // Set the name of the state and register the children.
    _name = "Mountainhome";
    registerState(new GameState(NULL), GameStateID); ///\todo Create the main window.
    registerState(new MenuState(), MenuStateID);

    // Set our active state and return.
    setActiveState(GameStateID);
}

Mountainhome::~Mountainhome() {}

void Mountainhome::update(int elapsed) {
    //Info("Updating: " << elapsed);
}

void Mountainhome::display(int elapsed) {
    //_mainCamera->activate(_renderContext);
    //Info("Displaying: " << elapsed);
}

void Mountainhome::keyPressed(KeyEvent *event) {
    switch(event->key()) {
    case Keyboard::KEY_q:
        Info("Exiting!");
        stopMainLoop();
        break;
    }
}
