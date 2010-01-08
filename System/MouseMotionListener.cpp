/*
 *  MouseMotionListener.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 5/1/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "MouseMotionListener.h"

MouseMotionEvent::MouseMotionEvent(int absX, int absY, int relX, int relY): _absX(absX),
_absY(absY), _relX(relX), _relY(relY) {}
MouseMotionEvent::~MouseMotionEvent() {}

int MouseMotionEvent::absX() { return _absX; }
int MouseMotionEvent::absY() { return _absY; }
int MouseMotionEvent::relX() { return _relX; }
int MouseMotionEvent::relY() { return _relY; }
