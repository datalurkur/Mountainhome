/*
 *  DemoCore.h
 *  System
 *
 *  Created by loch on 11/12/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DEMOCORE_H_
#define _DEMOCORE_H_
#include "SimpleCore.h"

class DemoCore : public SimpleCore {
public:
    DemoCore(int width, int height, bool fullscreen, const std::string &caption);
    ~DemoCore();

    void setSpeed(Real unitPerMillisecond);

    virtual void update(int elapsed);
    virtual void keyPressed(KeyEvent *event);
    virtual void keyReleased(KeyEvent *event);

protected:
    enum Direction {
        None     = 0,
        Left     = 1 << 0,
        Right    = 1 << 1,
        Forward  = 1 << 2,
        Backward = 1 << 3, 
    };

protected:
    void printUsage();

    Real _speed;
    int _current;

};

#endif
