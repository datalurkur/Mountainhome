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

//void Camera::setViewByMouse() {
//    int x, y;
//    int middleX = _viewport->getTarget()->getWidth()  >> 1;
//    int middleY = _viewport->getTarget()->getHeight()  >> 1;
//    float deltaX, deltaY;
//
//    Mouse::Get()->getMousePos(x, y);
//
//    if ((x == middleX) && (y == middleY)) { return; }
//        
//    Mouse::Get()->setMousePos(middleX, middleY);
//
//    deltaX = Math::Radians(float((middleX - x) * MOUSE_SPEED) / float(middleX));
//    deltaY = Math::Radians(float((middleY - y) * MOUSE_SPEED) / float(middleY));
//
//    standardViewByMouse(deltaX, deltaY);
//}
