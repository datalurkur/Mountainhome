/*
 *  SimpleCore.h
 *  Base
 *
 *  Created by Brent Wilson on 11/12/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _SIMPLECORE_H_
#define _SIMPLECORE_H_
#include "AbstractCore.h"

class Window;
class EventPump;
class RenderContext;
class FrameListener;
class Camera;

class SimpleCore : public AbstractCore {
public:
    SimpleCore(int width, int height, bool fullscreen, const std::string &caption);
    //\todo Implement a consturctor that makes use of persistent settings.

    virtual ~SimpleCore();

    virtual void innerLoop(int elapsed);
    virtual void display(int elapsed) {}

protected:
    Camera *_mainCamera;

};

#endif
