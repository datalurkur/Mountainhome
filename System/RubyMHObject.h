/*
 *  RubyMHObject.h
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _RUBYMHOBJECT_H_
#define _RUBYMHOBJECT_H_
#include "RubyBindings.h"

class MHObject;
class RubyMHObject {
public:
    static void SetupBindings();

    static VALUE Initialize(VALUE self, VALUE name, VALUE world, VALUE model, VALUE mat);

    typedef std::map<VALUE, MHObject*> RubyObjectMap;

private:
    static RubyObjectMap Objects;
    static VALUE Class;

};

#endif
