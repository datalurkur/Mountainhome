/*
 *  MHActorSelectionBindings.h
 *  Mountainhome
 *
 *  Created by loch on 11/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHACTORSELECTIONBINDINGS_H_
#define _MHACTORSELECTIONBINDINGS_H_
#include "RubyBindings.h"
#include "MHSelection.h"

class MHActorSelectionBindings : public RubyBindings<MHActorSelection, false> {
public:
    MHActorSelectionBindings();
    virtual ~MHActorSelectionBindings();

    static VALUE Each(VALUE rSelf);
};

#endif
