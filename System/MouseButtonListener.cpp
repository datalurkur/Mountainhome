/*
 *  MouseButtonListener.cpp
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/5/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "MouseButtonListener.h"

MouseButtonEvent::MouseButtonEvent(int button, int x, int y): _button(button), _x(x),
_y(y) {}

MouseButtonEvent::~MouseButtonEvent() {}

int MouseButtonEvent::x() { return _x; }
int MouseButtonEvent::y() { return _y; }
int MouseButtonEvent::button() { return _button; }


