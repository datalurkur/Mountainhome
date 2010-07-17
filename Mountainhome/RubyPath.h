/*
 *  RubyPath.h
 *  Mountainhome
 *
 *  Created by Andrew on 7/16/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RUBYPATH_H_
#define _RUBYPATH_H_
#include "Path.h"
#include "RubyBindings.h"

class RubyPath : public RubyBindings<Path, false> {

public:
    static void SetupBindings();

    static VALUE NextStep(VALUE self);
};

#endif

