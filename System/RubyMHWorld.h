/*
 *  RubyMHWorld.h
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RUBYMHWORLD_H_
#define _RUBYMHWORLD_H_
#include "RubyBindings.h"

class MHWorld;
class RubyMHWorld {
public:
    static void SetupBindings();
    static MHWorld* GetWorld(VALUE robj);

    static VALUE Initialize(VALUE self);

    typedef std::map<VALUE, MHWorld*> RubyObjectMap;

private:
    static RubyObjectMap Objects;
    static VALUE Class;

};

#endif
