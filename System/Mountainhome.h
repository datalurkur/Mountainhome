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
#include <Engine/SimpleCore.h>
class Mountainhome : public SimpleCore {
public:
    static const std::string GameStateID;
    static const std::string MenuStateID;

public:
    Mountainhome();
    virtual ~Mountainhome();

    virtual void update(int elapsed);
    virtual void display(int elapsed);
    virtual void keyPressed(KeyEvent *event);
};
#endif
