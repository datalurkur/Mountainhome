/*
 *  TestKeyboard.cpp
 *  System
 *
 *  Created by loch on 11/11/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "TestEventSystem.h"
#include <Keyboard.h>
#include <SDL_Helper.h>
#include <EventPump.h>
#include <KeyListener.h>

struct TempListener : public KeyListener {
    int lastKey;
    int lastMod;
    int pressed;
    int typed;
    int released;

    TempListener(): lastKey(0), lastMod(0), pressed(0), typed(0), released(0) {}

    void dumpInfo() {
        Info(lastKey << lastMod << pressed << released << typed);
    }

    virtual void keyTyped(KeyEvent *event) {
        lastKey = event->key();
        lastMod = event->modifier();
        typed++;
    }

    virtual void keyPressed(KeyEvent *event) {
        lastKey = event->key();
        lastMod = event->modifier();
        pressed++;
    }

    virtual void keyReleased(KeyEvent *event) {
        lastKey = event->key();
        lastMod = event->modifier();
        released++;
    }

};

EventPump *TestEventSystem::_pump = NULL;
TempListener *TestEventSystem::_listener = NULL;

void TestEventSystem::RunTests() {
    SDL_Init(SDL_INIT_VIDEO);
    _listener = new TempListener();
    _pump = new EventPump();
    _pump->addKeyListener(_listener);

    TestKeyboardEvents();

    _pump->clearListeners();
    delete _listener;
    delete _pump;
    SDL_Quit();
}

void TestEventSystem::TestKeyboardEvents() {
    Keyboard *board = Keyboard::GetSingleton();
    board->pressKey(Keyboard::KEY_s, Keyboard::MOD_LSHIFT);
    _pump->processEvents();

    TASSERT_EQ(_listener->typed, 0);
    TASSERT_EQ(_listener->pressed, 1);
    TASSERT_EQ(_listener->released, 1);
    TASSERT_EQ(_listener->lastKey, Keyboard::KEY_s);
    TASSERT_EQ(_listener->lastMod, Keyboard::MOD_LSHIFT);

    board->pressKey(Keyboard::KEY_F1, Keyboard::MOD_LSHIFT | Keyboard::MOD_RALT);
    _pump->processEvents();

    TASSERT_EQ(_listener->typed, 0);
    TASSERT_EQ(_listener->pressed, 2);
    TASSERT_EQ(_listener->released, 2);
    TASSERT_EQ(_listener->lastKey, Keyboard::KEY_F1);
    TASSERT_EQ(_listener->lastMod, Keyboard::MOD_LSHIFT | Keyboard::MOD_RALT);
}
