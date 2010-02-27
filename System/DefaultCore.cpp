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
    _mainWindow = new Window(800, 600, false, caption);
    _renderContext = new RenderContext();

    _renderContext->clearBuffers(Color4(0.0, 0.0, 0.0, 1.0));
    _mainWindow->swapBuffers();

    _targets.push_back(_mainWindow);
}

DefaultCore::~DefaultCore() {}

void DefaultCore::display(int elapsed) {
    _renderContext->resetGeometryCount();
    std::list<RenderTarget*>::iterator itr;
    for (itr = _targets.begin(); itr != _targets.end(); itr++) {
        (*itr)->render(_renderContext);
    }

    _mainWindow->swapBuffers();
}

void DefaultCore::innerLoop(int elapsed) {
    update(elapsed);
    display(elapsed);
}

void DefaultCore::teardown() {
    // Ensure everything is torndown before we start deleting stuff!
    clearStates();
}
