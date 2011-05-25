/*
 *  MHActorBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 11/13/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHActorBindings.h"
#include "EntityBindings.h"

MHActorBindings::MHActorBindings()
: RubyBindings<MHActor, false>(
    rb_define_class("MHActor", EntityBindings::Get()->getClass()),
    "MHActorBindings")
{

}
