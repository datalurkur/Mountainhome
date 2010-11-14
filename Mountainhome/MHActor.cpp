/*
 *  MHActor.cpp
 *  Mountainhome
 *
 *  Created by loch on 11/13/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHActor.h"

const std::string MHActor::TypeName = "MHActor";

MHActor::MHActor(const std::string &name):
    Entity(name, TypeName) {}
