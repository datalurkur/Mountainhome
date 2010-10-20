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
    Sphere(Real radius, unsigned int strips = 15, unsigned int panels = 15);
    virtual ~Sphere();

private:
    void buildSphere(unsigned int strips, unsigned int panels);

private:
    Real _size;

};

#endif
