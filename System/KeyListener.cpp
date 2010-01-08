/*
 *  KeyListener.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/5/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "KeyListener.h"

KeyEvent::KeyEvent(int k, int m): _key(k), _mod(m) {}
KeyEvent::~KeyEvent() {}

int KeyEvent::key() { return _key; }
int KeyEvent::modifier() { return _mod; }
