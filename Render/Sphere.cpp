/*
 *  Sphere.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "Sphere.h"

#include <GLUT/glut.h>
#include <OpenGL/gl.h>

Sphere::Sphere(Real size): _size(size) {
    _boundingBox = AABB3(Vector3(0,0,0), Vector3(size/2, size/2, size/2));
}

Sphere::~Sphere() {}

void Sphere::render(RenderContext *context) {
    glutSolidSphere(_size, 30, 30);
}