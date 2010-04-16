/*
 *  DemoCore.h
 *  Base
 *
 *  Created by Brent Wilson on 11/12/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _DEMOCORE_H_
#define _DEMOCORE_H_
#include "SimpleCore.h"

class DemoCore : public SimpleCore {
public:
    DemoCore(int width, int height, const std::string &caption);
    virtual ~DemoCore();

    void setSpeed(Real unitPerMillisecond);

    virtual void update(int elapsed);
    virtual void keyPressed(KeyEvent *event);
    virtual void keyReleased(KeyEvent *event);
    virtual void mouseMoved(MouseMotionEvent *event);

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
