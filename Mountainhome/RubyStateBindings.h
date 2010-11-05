/*
 *  RubyStateBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RUBYSTATEBINDINGS_H_
#define _RUBYSTATEBINDINGS_H_
#include "RubyBindings.h"
#include "RubyState.h"

class RubyStateBindings : public RubyBindings<RubyState, false> {
public:
    /*! The allocator function for RubyStates. */
    static VALUE Alloc(VALUE klass);

public:
    RubyStateBindings();
    virtual ~RubyStateBindings() {}

};

#endif
