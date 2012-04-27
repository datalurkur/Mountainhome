/*
 *  MHSelectionBindings.h
 *  Mountainhome
 *
 *  Created by loch on 11/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHSELECTIONBINDINGS_H_
#define _MHSELECTIONBINDINGS_H_
#include "RubyBindings.h"
#include "MHSelection.h"

class MHSelectionBindings : public RubyBindings<MHSelection, false> {
public:
    MHSelectionBindings();
    virtual ~MHSelectionBindings();

    static VALUE Each(VALUE rSelf);

    static VALUE NumActors(VALUE rSelf);
    static VALUE EachActor(VALUE rSelf);

    static VALUE NumVoxels(VALUE rSelf);
    static VALUE EachVoxel(VALUE rSelf);

    static VALUE RemoveVoxel(VALUE rSelf, VALUE x, VALUE y, VALUE z);
};

#endif
