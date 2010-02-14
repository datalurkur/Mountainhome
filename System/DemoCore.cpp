/*
 *  DemoCore.cpp
 *  Base
 *
 *  Created by Brent Wilson on 11/12/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Window.h"
#include "DemoCore.h"
#include "Camera.h"

#include "Keyboard.h"
#include "GL_Helper.h"

DemoCore::DemoCore(int width, int height, bool fs, const std::string &caption)
:SimpleCore(width, height, fs, caption), _speed(.005), _current(None) {
    printUsage();
}

DemoCore::~DemoCore() {}

void DemoCore::printUsage() {
    Info("Input Usage");
    Info(" esc/q: quit");
    Info(" mouse: look around");
    Info("     w: move forward");
    Info("     a: strafe left");
    Info("     s: move backward");
    Info("     d: strafe right");
    Info(" space: screen shot");
    Info("\n");
}

void DemoCore::setSpeed(Real newSpeed) {
    _speed = newSpeed;
}

void DemoCore::update(int elapsed) {
    CheckGLErrors();
    if (_current & Left    ) { _mainCamera->strafeLeft  (elapsed * _speed); }
    if (_current & Right   ) { _mainCamera->strafeRight (elapsed * _speed); }
    if (_current & Forward ) { _mainCamera->moveForward (elapsed * _speed); }
    if (_current & Backward) { _mainCamera->moveBackward(elapsed * _speed); }
}

void DemoCore::mouseMoved(MouseMotionEvent *event) {
//\TODO Update this to use the MouseMotionEvent.
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
}

void DemoCore::keyPressed(KeyEvent *event) {
    switch(event->key()) {
        case Keyboard::KEY_SPACE:
            // Take a screenshot?
        case 'q':
        case Keyboard::KEY_ESCAPE:
            stopMainLoop();
            break;
        case 'w':
            _current |= Forward;
            break;
        case 'a':
            _current |= Left;
            break;
        case 's':
            _current |= Backward;
            break;
        case 'd':
            _current |= Right;
            break;
    }
}

void DemoCore::keyReleased(KeyEvent *event) {
    switch(event->key()) {
        case 'w':
            _current &= ~Forward;
            break;
        case 'a':
            _current &= ~Left;
            break;
        case 's':
            _current &= ~Backward;
            break;
        case 'd':
            _current &= ~Right;
            break;
    }
}
