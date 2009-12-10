/*
 *  DefaultCore.h
 *  System
 *
 *  Created by loch on 11/13/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _DEFAULTCORE_H_
#define _DEFAULTCORE_H_
#include "SimpleCore.h"

class DefaultCore : public SimpleCore {

};

// Create a 'state' class that acts much like the simple core. The default core will act
// much like the simple core except it will have support for states and setting the
// current state. Rather than accessing the core directly through subclassing, the current
// state will have a pointer to the core and use accessors.

#endif
