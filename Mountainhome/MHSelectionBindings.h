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

};

#endif
