/*
 *  SimpleCore.cpp
 *  Base
 *
 *  Created by Brent Wilson on 11/12/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include <Render/Camera.h>
#include <Render/RenderTarget.h>
#include <Render/RenderContext.h>
#include <Render/Viewport.h>

#include "SimpleCore.h"
#include "EventPump.h"
#include "FrameListener.h"
#include "Window.h"


SimpleCore::SimpleCore(int width, int height, const std::string &name)
:AbstractCore(width, height, name), _mainCamera(NULL) {
    _mainCamera = new Camera();
    Viewport *v = _mainWindow->addViewport();
    v->addSource(_mainCamera, 0);
}

SimpleCore::~SimpleCore() {
    delete _mainCamera;
}

void SimpleCore::innerLoop(int elapsed) {
    update(elapsed);

    _renderContext->resetMetrics();
    _mainWindow->render(_renderContext);
    display(elapsed);

    setPostText();
    _mainWindow->swapBuffers();
}
