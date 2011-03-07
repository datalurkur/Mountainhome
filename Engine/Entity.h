/*
 *  Entity.h
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _ENTITY_H_
#define _ENTITY_H_
#include <Base/AABB.h>
#include "SceneNode.h"

class Model;
class SceneManager;

class Entity : public SceneNode {
public:
    static const std::string TypeName;

public:
    Entity(const std::string &name);

    virtual ~Entity();

    void expandLocalAABB(const AABB3 &aabb);

    void addModel(Model *model, Material *mat = NULL);

    virtual void updateImplementationValues();

protected:
    Entity(const std::string &name, const std::string &typeName);

private:
    AABB3 _localAABB;
    bool _hasLocalAABB;

};

#endif
