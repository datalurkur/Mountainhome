/*
 *  AbstractCore.cpp
 *  System
 *
 *  Created by loch on 11/13/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "AbstractCore.h"
#include "EventPump.h"
#include "RenderContext.h"
#include "FrameListener.h"
#include "RenderTarget.h"
#include "Window.h"
#include "Camera.h"

#include "SDL_Helper.h"

AbstractCore::AbstractCore(int width, int height, bool fullscreen, const std::string &name)
: _running(true), _elapsedTime(0), _framerate(1337), _mainWindow(NULL), 
_renderContext(NULL), _eventPump(NULL) {
    _mainWindow = new Window(width, height, fullscreen, name);
    _renderContext = new RenderContext();

    _eventPump = new EventPump();
    _eventPump->addWindowListener(this);
    _eventPump->addMouseButtonListener(this);
    _eventPump->addMouseMotionListener(this);
    _eventPump->addKeyListener(this);
}

AbstractCore::~AbstractCore() {
    delete _eventPump;
    delete _renderContext;
    delete _mainWindow;
}

void AbstractCore::setPostText() {
    char buffer [64];
    //snprintf(buffer, 64, "FPS: %i Geo: %i", _framerate, _renderContext->getGeometryCount());
    snprintf(buffer, 64, "FPS: %i", (int)_framerate);
    _mainWindow->setPostCaption(buffer);
}

void AbstractCore::calculateFramerate(int elapsed) {
    static int summedTime = 0;
    static int frames = 0;
    
    summedTime += elapsed;
    if (summedTime > 1000u) {
        _framerate = static_cast<Real>(frames) * 1000.0f;
        _framerate /= static_cast<Real>(summedTime);    
        summedTime = 0;
        frames = 0;
    }
    frames++;
}

int AbstractCore::getTime() {
    return SDL_GetTicks();
}

Window* AbstractCore::getMainWindow() {
    return _mainWindow;
}

EventPump* AbstractCore::getEventPump() {
    return _eventPump;
}

void AbstractCore::stopMainLoop() {
    _running = false;
}

void AbstractCore::windowClosing() {
    stopMainLoop();
}

void AbstractCore::windowResized(int width, int height) {
    _mainWindow->resize(width, height);
}

void AbstractCore::addFrameListener(FrameListener *listener) {
    _frameListeners.push_back(listener);
}

bool AbstractCore::broadcastFrameEvent(int elapsed) {
    bool returnValue = false;
    std::list<FrameListener*>::iterator itr;
    for (itr = _frameListeners.begin(); itr != _frameListeners.end(); itr++) {
        if ((*itr)->frameStarted(elapsed)) {
            returnValue = true;
        }
    }

    return returnValue;
}

RenderContext* AbstractCore::getRenderContext() {
    return _renderContext;
}
