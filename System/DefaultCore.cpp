/*
 *  DefaultCore.cpp
 *  Base
 *
 *  Created by Brent Wilson on 11/13/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include <Render/RenderContext.h>

#include "DefaultCore.h"
#include "Window.h"

DefaultCore::DefaultCore(const std::string &caption) {
    // \fixme Load from some sort of persistent settings.
    _mainWindow = new Window(800, 600, true, caption);
}

DefaultCore::~DefaultCore() {}

void DefaultCore::display(int elapsed) {
    _renderContext->resetGeometryCount();
    std::list<RenderTarget*>::iterator itr;
    for (itr = _targets.begin(); itr != _targets.end(); itr++) {
        
    }
}

void DefaultCore::innerLoop(int elapsed) {
    update(elapsed);
    display(elapsed);
}