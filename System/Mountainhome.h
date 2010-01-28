/*
 *  Mountainhome.h
 *  Base
 *
 *  Created by Brent Wilson on 12/21/09.
 *  Copyright 2009 NVIDIA. All rights reserved.
 *
 */

#ifndef _MOUNTAINHOME_H_
#define _MOUNTAINHOME_H_
#include <Engine/DefaultCore.h>

class Mountainhome : public DefaultCore {
public:
    static const std::string GameStateID;

    static Mountainhome *Instance;

public:
    static Window *window();

public:
    Mountainhome();
    virtual ~Mountainhome();

    virtual void keyPressed(KeyEvent *event);
};
#endif
