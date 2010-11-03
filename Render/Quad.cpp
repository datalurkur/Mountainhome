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
    buildQuad();

    // And do the basic model setup.
    findBounds();
    generateVBOs();
}

Quad::~Quad() {}

void Quad::buildQuad() {
    Real hw = _width/2.0;
    Real hh = _height/2.0;

    _count = 4;

    _verts = new Vector3[_count];
    _norms = new Vector3[_count];
    _texCoords = NULL;

    // vertex look like:
    // 1 0
    // 2 3
    //<-- 0,0

    _verts[0] = Vector3( hw,  hh, 0.0);
    _verts[1] = Vector3(-hw,  hh, 0.0);
    _verts[2] = Vector3(-hw, -hh, 0.0);
    _verts[3] = Vector3( hw, -hh, 0.0);

    for(int ii = 0; ii < _count; ii++)
        _norms[ii] = _verts[ii].getNormalized();

    _indexCount = 6;
    _indices = new unsigned int[_indexCount];

    unsigned int iIndex = 0;

    // Top left triangle
    _indices[iIndex++] = 0;
    _indices[iIndex++] = 2;
    _indices[iIndex++] = 1;

    // Bottom right triangle.
    _indices[iIndex++] = 0;
    _indices[iIndex++] = 3;
    _indices[iIndex++] = 2;
}
