/*
 *  InputListener.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 5/2/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _INPUTLISTENER_H_
#define _INPUTLISTENER_H_
#include "MouseButtonListener.h"
#include "MouseMotionListener.h"
#include "KeyListener.h"

class InputListener : public KeyListener, public MouseButtonListener,
public MouseMotionListener {};

#endif
