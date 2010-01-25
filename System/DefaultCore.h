/*
 *  DefaultCore.h
 *  Base
 *
 *  Created by Brent Wilson on 11/13/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _DEFAULTCORE_H_
#define _DEFAULTCORE_H_
#include "SimpleCore.h"

class RenderTarget;
class DefaultCore : public AbstractCore {
public:
    //\todo Load the particulars from persistent data storage.
    DefaultCore(const std::string &caption);
    virtual ~DefaultCore();

    virtual void update(int elapsed) {}  // Can't make it pure virtual again.
    virtual void display(int elapsed);
    virtual void innerLoop(int elapsed);
    virtual void teardown();

protected:
    std::list<RenderTarget*> _targets;

};

// Create a 'state' class that acts much like the simple core. The default core will act
// much like the simple core except it will have support for states and setting the
// current state. Rather than accessing the core directly through subclassing, the current
// state will have a pointer to the core and use accessors.

#endif
