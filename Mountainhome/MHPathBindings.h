/*
 *  MHPathBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHPATHBINDINGS_H_
#define _MHPATHBINDINGS_H_
#include "RubyBindings.h"
#include "MHPath.h"

class MHPathBindings : public RubyBindings<MHPath, true> {
public:
    static VALUE Initialize(VALUE rSelf, VALUE rWorld, VALUE rSX, VALUE rSY, VALUE rSZ, VALUE rDX, VALUE rDY, VALUE rDZ);
    static VALUE NextStep(VALUE self);
    static VALUE Blocked(VALUE self);
    static VALUE EndOfPath(VALUE self);

public:
    MHPathBindings();
    virtual ~MHPathBindings() {}

};

#endif
