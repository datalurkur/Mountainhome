/*
 *  Keyboard.cpp
 *  System
 *
 *  Created by loch on 11/11/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "Keyboard.h"
#include "SDL_Helper.h"

Keyboard::Keyboard() {}
Keyboard::~Keyboard() {}

int Keyboard::getModifiers() {
    return SDL_GetModState();
}

bool Keyboard::isKeyDown(int key) {
    int size = 0;
    Uint8 *keys = SDL_GetKeyState(&size);
    return keys[key] == 1;
}

void Keyboard::pressKey(int pressed, int modifiers) {
    SDL_Event *event = new SDL_Event;
    SDL_KeyboardEvent *key = &event->key;
    key->type = SDL_KEYDOWN;
    key->state = SDL_PRESSED;
    key->keysym.sym = (SDLKey)pressed;
    key->keysym.mod = (SDLMod)modifiers;
    key->keysym.unicode = 0;
    SDL_PushEvent(event);

    key->type = SDL_KEYUP;
    key->state = SDL_RELEASED;
    SDL_PushEvent(event);

    delete event;

}
