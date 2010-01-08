/*
 *  SimpleCore.cpp
 *  Base
 *
 *  Created by Brent Wilson on 11/12/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "SimpleCore.h"
#include "EventPump.h"
#include "RenderContext.h"
#include "FrameListener.h"
#include "RenderTarget.h"
#include "Window.h"
#include "Camera.h"

SimpleCore::SimpleCore(int width, int height, bool fullscreen, const std::string &name)
:AbstractCore(width, height, fullscreen, name), _mainCamera(NULL) {
    _mainCamera = new Camera();
    _mainWindow->addViewport(_mainCamera);
}

SimpleCore::~SimpleCore() {
    delete _mainCamera;
}

void SimpleCore::innerLoop(int elapsed) {
    update(elapsed);

    _renderContext->resetGeometryCount();
    _mainWindow->render(_renderContext);
    display(elapsed);

    setPostText();
    _mainWindow->swapBuffers();
}