/*
 *  EventPump.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/4/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */
 
#include <Render/Render.h>
#include <Render/SDL_Helper.h>

#include "EventPump.h"
#include "WindowListener.h"
#include "MouseButtonListener.h"
#include "MouseMotionListener.h"
#include "KeyListener.h"
#include "Window.h"

EventPump::EventPump() {}
EventPump::~EventPump() {}

void EventPump::clearListeners() {
    _windowListeners.clear();
    _mouseButtonListeners.clear();
    _mouseMotionListeners.clear();
    _keyListeners.clear();
}

void EventPump::addWindowListener(WindowListener *listener) {
    _windowListeners.push_back(listener);
}

void EventPump::addMouseButtonListener(MouseButtonListener *listener) {
    _mouseButtonListeners.push_back(listener);
}

void EventPump::addMouseMotionListener(MouseMotionListener *listener) {
    _mouseMotionListeners.push_back(listener);
}

void EventPump::addKeyListener(KeyListener *listener) {
    _keyListeners.push_back(listener);
}

void EventPump::processEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_VIDEORESIZE: {
                std::list<WindowListener*>::iterator itr;
                for (itr = _windowListeners.begin();
                     itr != _windowListeners.end(); itr++) {
                    (*itr)->windowResized(event.resize.w, event.resize.h);
                }
            } break;
            case SDL_QUIT: {
                std::list<WindowListener*>::iterator itr;
                for (itr = _windowListeners.begin();
                     itr != _windowListeners.end(); itr++) {
                    (*itr)->windowClosing();
                }
            } break;
            case SDL_KEYDOWN: {
                KeyEvent keydown(event.key.keysym.sym, event.key.keysym.mod);
                std::list<KeyListener*>::iterator itr;
                for (itr = _keyListeners.begin(); itr != _keyListeners.end(); itr++) {
                    (*itr)->keyPressed(&keydown);
                }
            } break;
            case SDL_KEYUP: {
                KeyEvent keyup(event.key.keysym.sym, event.key.keysym.mod);
                std::list<KeyListener*>::iterator itr;
                for (itr = _keyListeners.begin(); itr != _keyListeners.end(); itr++) {
                    (*itr)->keyReleased(&keyup);
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                MouseButtonEvent e(event.button.button, event.button.x,
                                   GUI_HEIGHT - 1 - event.button.y);

                std::list<MouseButtonListener*>::iterator itr;
                for (itr = _mouseButtonListeners.begin();
                     itr != _mouseButtonListeners.end(); itr++) {
                    (*itr)->mousePressed(&e);
                }
            } break;

            case SDL_MOUSEBUTTONUP: {
                MouseButtonEvent e(event.button.button, event.button.x,
                                   GUI_HEIGHT - 1 - event.button.y);

                std::list<MouseButtonListener*>::iterator itr;
                for (itr = _mouseButtonListeners.begin();
                     itr != _mouseButtonListeners.end(); itr++) {
                    (*itr)->mouseReleased(&e);
                }
            } break;

            case SDL_MOUSEMOTION: {
                MouseMotionEvent e(event.motion.x, GUI_HEIGHT - 1 - event.motion.y,
                                   event.motion.xrel, event.motion.yrel);

                std::list<MouseMotionListener*>::iterator itr;
                for (itr = _mouseMotionListeners.begin();
                     itr != _mouseMotionListeners.end(); itr++) {
                    (*itr)->mouseMoved(&e);
                }
            } break;

            default:
                break;
        }
    }
}
