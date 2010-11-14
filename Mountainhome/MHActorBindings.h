/*
 *  MHActorBindings.h
 *  Mountainhome
 *
 *  Created by loch on 11/13/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHACTORBINDINGS_H_
#define _MHACTORBINDINGS_H_
#include "RubyBindings.h"
#include "MHActor.h"

class MHActorBindings : public RubyBindings<MHActor, false> {
public:
    MHActorBindings();
    virtual ~MHActorBindings() {}

};

// A specialization that doesn't try to delete the Camera, which wouldn't compile.
template <> void RubyBindings<MHActor, false>::Free(MHActor* cobj);


#endif
