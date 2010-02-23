/*
 *  Quad.cpp
 *  System
 *
 *  Created by loch on 2/23/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "Quad.h"

#include <OpenGL/gl.h>

Quad::Quad(Real width, Real height): _width(width), _height(height) {
    _boundingBox = AABB3(Vector3(0,0,0), Vector3(width / 2.0, 0, height / 2.0));
}

Quad::~Quad() {}

void Quad::render(RenderContext *context) {
    Real hw = _width/2.0;
    Real hh = _height/2.0;

    glBegin(GL_QUADS); {
        glVertex3f( hw, 0.0,  hh);
        glVertex3f(-hw, 0.0,  hh);
        glVertex3f(-hw, 0.0, -hh);
        glVertex3f( hw, 0.0, -hh);
    } glEnd();
}
