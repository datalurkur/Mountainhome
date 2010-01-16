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
#include <Base/Vector.h>

class Model;

///\todo Look into making a parent class for everything that needs set/get position/rotation/transformation

class Material;
class Entity {
public:
    Entity(Model *m)                                { /*!\todo implement me */ }
    virtual ~Entity()                               { /*!\todo implement me */ }
    void setPosition(const Vector3 &vec)            { /*!\todo implement me */ }
    void setPosition(Real x, Real y, Real z = 0.0f) { /*!\todo implement me */ }
    void setMaterial(const Material *m)             { /*!\todo implement me */ }
};

#endif
