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

Sphere::Sphere(Real size, unsigned int strips, unsigned int panels): _size(size) {
    buildSphere(strips, panels);

    // And do the basic model setup.
    findBounds();
    generateVBOs();
}

Sphere::~Sphere() {}

void Sphere::buildSphere(unsigned int strips, unsigned int panels) {
    Radian panelStep(Math::PI * 2.0 / panels);
    Radian stripStep(Math::PI / strips);

    unsigned int vIndex = 1;
    unsigned int iIndex = 0;

    ASSERT(strips > 1); // Needs at least two strips to form a sphere.
    strips = Math::Max(strips, 2);
    
    ASSERT(panels > 3); // Needs at least three panels to form a sphere.
    panels = Math::Max(panels, 3);
    
    // One vert for each strip/panel and the two capping verts.
    _count = (strips * panels) + 2;

    _verts = new Vector3[_count];
    _norms = new Vector3[_count];
    _texCoords = NULL;

    // Two 3-vert prims per panel on the middle strips. One 3-vert prim per panel on the capping strips.
    _indexCount = ((strips - 2) * panels * 3 * 2) + (panels * 3 * 2);
    _indices = new unsigned int[_indexCount];

    // Cap the sphere.
    _verts[0] = Vector3(0, _size, 0);
    _verts[_count - 1] = Vector3(0, -_size, 0);

    for (int strip = 0; strip < strips; strip++)
    {
        Real scale = _size * Math::Sin(stripStep * (strip + 1));
        Real yPos = _size * Math::Cos(stripStep * (strip + 1));
        unsigned int lastVIndex = (strip + 1) * panels;

        for (int panel = 0; panel < panels; panel++)
        {
            // Add the new vertex.
            Real xPos = scale * Math::Cos(panelStep * panel);
            Real zPos = scale * Math::Sin(panelStep * panel);
            _verts[vIndex] = Vector3(xPos, yPos, zPos);
            _norms[vIndex] = _verts[vIndex].getNormalized();

            // Add the new primitives.
            if (strip == 0)
            {
                _indices[iIndex++] = 0;
                _indices[iIndex++] = lastVIndex;
                _indices[iIndex++] = vIndex;
            }
            else if (strip == strips - 1)
            {
                _indices[iIndex++] = _count - 1;
                _indices[iIndex++] = vIndex - panels;
                _indices[iIndex++] = lastVIndex - panels;
            }
            else
            {
                // Bottom right triangle.
                _indices[iIndex++] = (short)(lastVIndex - panels);
                _indices[iIndex++] = lastVIndex;
                _indices[iIndex++] = vIndex;

                // Top left triangle.
                _indices[iIndex++] = (short)(lastVIndex - panels);
                _indices[iIndex++] = vIndex;
                _indices[iIndex++] = (short)(vIndex - panels);
            }

            lastVIndex = vIndex++;
        }
    }
}
