/*
 *  EntityBindings.h
 *  Mountainhome
 *
 *  Created by Paul on 6/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _ENTITYBINDINGS_H_
#define _ENTITYBINDINGS_H_
#include <Engine/Entity.h>
#include "RubyBindings.h"

class EntityBindings : public RubyBindings<Entity, false> {
public:
    static VALUE SetVisibility(VALUE rSelf, VALUE rState);

public:
    EntityBindings();
    virtual ~EntityBindings() {}

    Entity *getPointer(VALUE rObj);

};

// A specialization that doesn't try to delete the Camera, which wouldn't compile.
template <> void RubyBindings<Entity, false>::Free(Entity* cobj);

#endif
