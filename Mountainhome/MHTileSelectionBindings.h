/*
 *  MHTileSelectionBindings.h
 *  Mountainhome
 *
 *  Created by datalurkur on 11/25/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHTILESELECTIONBINDINGS_H_
#define _MHTILESELECTIONBINDINGS_H_
#include "RubyBindings.h"
#include "MHSelection.h"

class MHTileSelectionBindings : public RubyBindings<MHTileSelection, false> {
public:
    MHTileSelectionBindings();
    virtual ~MHTileSelectionBindings();

    static VALUE Each(VALUE rSelf);
};

#endif
