/*
 *  Sphere.h
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _SPHERE_H_
#define _SPHERE_H_
#include "Model.h"

class Sphere : public Model {
public:
    Sphere(Real radius);
    virtual ~Sphere();

private:
    Real _size;
};

#endif
