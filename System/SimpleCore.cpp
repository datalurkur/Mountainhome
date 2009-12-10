/*
 *  SimpleCore.cpp
 *  System
 *
 *  Created by loch on 11/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "SimpleCore.h"
#include "EventPump.h"
#include "RenderContext.h"
#include "FrameListener.h"
#include "RenderTarget.h"
#include "Window.h"
#include "Camera.h"

#include "SDL_Helper.h"

SimpleCore::SimpleCore(int width, int height, bool fullscreen, const std::string &name)
:AbstractCore(width, height, fullscreen, name), _mainCamera(NULL) {
    _mainCamera = new Camera();
    _mainWindow->addRenderSource(_mainCamera);

}

SimpleCore::~SimpleCore() {
    delete _mainCamera;
}

void SimpleCore::startMainLoop() {
    int lastTime = getTime();
    _running = true;

    while(_running) {
        int currentTime = getTime();
        _elapsedTime = currentTime - lastTime;
        calculateFramerate(_elapsedTime);

        getEventPump()->processEvents();
        if (broadcastFrameEvent(_elapsedTime)) {
            stopMainLoop();
        }
        
        update(_elapsedTime);

        _renderContext->resetGeometryCount();
        _mainCamera->activate(_renderContext);
        display(_elapsedTime);

        setPostText();
        _mainWindow->swapBuffers();
        lastTime = currentTime;
    }
}

