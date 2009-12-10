/*
 *  SimpleCore.h
 *  System
 *
 *  Created by loch on 11/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SIMPLECORE_H_
#define _SIMPLECORE_H_
#include "AbstractCore.h"
#include "InputListener.h"
#include "WindowListener.h"
#include <Math3D.h>
#include <list>

class Window;
class EventPump;
class RenderContext;
class FrameListener;
class Camera;

class SimpleCore : public AbstractCore {
public:
    SimpleCore(int width, int height, bool fullscreen, const std::string &caption);
    ~SimpleCore();

    virtual void startMainLoop();
    virtual void update(int elapsed) = 0;
    virtual void display(int elapsed) = 0;

protected:
    Camera *_mainCamera;

};

#endif
