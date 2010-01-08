/*
 *  MenuState.h
 *  System
 *
 *  Created by loch on 12/29/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MENUSTATE_H_
#define _MENUSTATE_H_
#include <Engine/State.h>
class MenuState : public State {
public:
    MenuState() {}
    virtual ~MenuState() {}

    virtual void setup(va_list args) {}
    virtual void teardown() {}

    virtual void update(int elapsed) {}
};

#endif
