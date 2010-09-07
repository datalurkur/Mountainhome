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

const unsigned int numSphereVerts = 6;
Vector3 sphereVerts[] = {
    Vector3( 0.0f,  0.0f, -0.5f), // 0
    Vector3( 0.5f,  0.0f,  0.0f), // 1
    Vector3(-0.5f,  0.0f,  0.0f), // 2
    Vector3( 0.0f,  0.5f,  0.0f), // 3
    Vector3( 0.0f, -0.5f,  0.0f), // 4
    Vector3( 0.0f,  0.0f,  0.5f)  // 5
};

Vector3 sphereNorms[] = {
    Vector3( 0.0f,  0.0f, -1.0f), // 0
    Vector3( 1.0f,  0.0f,  0.0f), // 1
    Vector3(-1.0f,  0.0f,  0.0f), // 2
    Vector3( 0.0f,  1.0f,  0.0f), // 3
    Vector3( 0.0f, -1.0f,  0.0f), // 4
    Vector3( 0.0f,  0.0f,  1.0f)  // 5
};

const unsigned int numSphereIndices = 24;
unsigned int sphereIndices[] = {
    0,1,4,
    0,4,2,
    0,2,3,
    0,3,1,
    5,1,3,
    5,3,2,
    5,2,4,
    5,4,1
};

Sphere::Sphere(Real size): _size(size) {
    _indices = sphereIndices;
    _indexCount = numSphereIndices;

    _verts = sphereVerts;
    _count = numSphereVerts;
    _norms = sphereNorms;

    _texCoords = NULL;

    // And do the basic model setup.
    findBounds();
    generateVBOs();
}

Sphere::~Sphere() {}
