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
#include "GL_Helper.h"


Sphere::Sphere(Real size): _size(size) {
    _boundingBox = AABB3(Vector3(0,0,0), Vector3(size, size, size));
}

Sphere::~Sphere() {}

void Sphere::render(RenderContext *context) {
    glUseProgram(0);
    glutSolidSphere(_size, 30, 30);
}
