/*
 *  MHActor.h
 *  Mountainhome
 *
 *  Created by loch on 11/13/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHACTOR_H_
#define _MHACTOR_H_
#include <Engine/Entity.h>

class MHActor : public Entity {
public:
    static const std::string TypeName;

public:
    MHActor(const std::string &name);

};

#endif
