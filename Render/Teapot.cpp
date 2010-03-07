/*
 *  Teapot.cpp
 *  System
 *
 *  Created by loch on 1/29/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "Teapot.h"

#include <GLUT/glut.h>
#include <OpenGL/gl.h>

Teapot::Teapot(Real size): _size(size) {
    _boundingBox = AABB3(Vector3(0,0,0), Vector3(size/2, size/2, size/2));
}

Teapot::~Teapot() {}

void Teapot::render(RenderContext *context) {
    glutSolidTeapot(_size);
}
