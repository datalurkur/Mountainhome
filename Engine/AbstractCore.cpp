/*
 *  AbstractCore.cpp
 *  Base
 *
 *  Created by Brent Wilson on 11/13/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include <Render/GL_Helper.h>
#include <Render/RenderContext.h>
#include <Render/RenderTarget.h>
#include <Render/SDL_Helper.h>
#include <Render/Camera.h>

#include "AbstractCore.h"
#include "EventPump.h"
#include "FrameListener.h"
#include "Window.h"

AbstractCore::AbstractCore(): _running(true), _framerate(1337), _mainWindow(NULL),
_renderContext(NULL), _eventPump(NULL) {}

AbstractCore::AbstractCore(int width, int height, const std::string &name)
: _running(true), _framerate(1337), _mainWindow(NULL), 
_renderContext(NULL), _eventPump(NULL) {
    _mainWindow = new Window(width, height, name);
    _renderContext = new RenderContext();

    _eventPump = new EventPump(_mainWindow);
    _eventPump->addWindowListener(this);
    _eventPump->addMouseButtonListener(this);
    _eventPump->addMouseMotionListener(this);
    _eventPump->addKeyListener(this);
}

AbstractCore::~AbstractCore() {
    delete _eventPump;     _eventPump     = NULL;
    delete _renderContext; _renderContext = NULL;
    delete _mainWindow;    _mainWindow    = NULL;
}

void AbstractCore::setPostText() {
    char buffer [64];
    snprintf(buffer, 64, "FPS: %i Geo: %i", (int)_framerate, _renderContext->getPrimitiveCount());
    // snprintf(buffer, 64, "FPS: %i", (int)_framerate);
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

void AbstractCore::startMainLoop() {
    Info("Starting main loop.");

    int lastTime = getTime();
    int elapsedTime;

    _running = true;

    va_list args;
    setup(args);
    while(_running) {
        //Info("-------------------------------------------------------------------------");

        int currentTime = getTime();
        elapsedTime = currentTime - lastTime;
        calculateFramerate(elapsedTime);

        getEventPump()->processEvents();
        broadcastFrameEvent(elapsedTime);

        innerLoop(elapsedTime);

        lastTime = currentTime;
        CheckGLErrors();

        //Info("Framerate: " << _framerate);
    }
    teardown();

    Info("Main loop finished.");
}

void AbstractCore::stopMainLoop() {
    Info("Halt of main loop requested.");
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
