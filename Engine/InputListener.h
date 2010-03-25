/*
 *  InputListener.h
 *  Engine
 *
 *  Created by Brent Wilson on 5/2/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _INPUTLISTENER_H_
#define _INPUTLISTENER_H_
#include "MouseButtonListener.h"
#include "MouseMotionListener.h"
#include "KeyListener.h"

/*! A Helper interface, allowing the subclass to register for all input events. */
class InputListener : public KeyListener, public MouseButtonListener,
public MouseMotionListener {
public:
    InputListener() {}
    virtual ~InputListener() {}
};

#endif
