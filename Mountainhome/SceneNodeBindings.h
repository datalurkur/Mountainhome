/*
 *  SceneNodeBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHSCENENODEBINDINGS_H_
#define _MHSCENENODEBINDINGS_H_
#include "RubyBindings.h"
#include "SceneNode.h"

class SceneNodeBindings : public RubyBindings<SceneNode, false> {
public:

public:
    SceneNodeBindings();
    virtual ~SceneNodeBindings();

    SceneNode *getPointer(VALUE rObj);

};

#endif
