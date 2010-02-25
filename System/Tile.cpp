/*
 *  Tile.cpp
 *  System
 *
 *  Created by datalurkur on 2/24/10.
 *
 */

#include "Tile.h"

#include <OpenGL/gl.h>

Tile::Tile(char type): _type(type) {
    _boundingBox = AABB3(Vector3(0,0,0), Vector3(0.5, 0.5, 0.5));
}

Tile::~Tile() {}

void Tile::render(RenderContext *context) {
    switch(_type)
    {
	case NONEUP:
	glBegin(GL_TRIANGLES); {
	    // Just a pyramid
	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(0.5, 0.5, 0.5);

	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(0.5, 0.5, 0.5);

	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(0.5, 0.5, 0.5);

	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(0.5, 0.5, 0.5);
	} glEnd();
	break;

	case ONEUP:
	glBegin(GL_TRIANGLES); {
	    // Top of ramp faces
	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(0.0, 1.0, 1.0);

	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(0.0, 1.0, 1.0);

	    // Side of ramp faces
	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(0.0, 1.0, 0.0);

	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(1.0, 1.0, 1.0);
	} glEnd();
	break;

	case TWOADJUP:
	glBegin(GL_TRIANGLES); {
	    // Top face
	    glVertex3f(0.0, 0.0, 1.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(0.0, 1.0, 1.0);

	    // Side faces
	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(0.5, 0.0, 0.0);
	    glVertex3f(0.0, 0.0, 1.0);

	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(1.0, 0.5, 0.0);

	    // Floor face
	    glVertex3f(0.5, 0.0, 0.0);
	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(1.0, 0.5, 0.0);

	    // Ramp faces
	    glVertex3f(0.5, 0.0, 0.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(0.0, 0.0, 1.0);

	    glVertex3f(0.5, 0.0, 0.0);
	    glVertex3f(1.0, 0.5, 0.0);
	    glVertex3f(1.0, 1.0, 1.0);
	} glEnd();
	break;

	case TWOOPPUP:
	glBegin(GL_TRIANGLES); {
	    // Top faces
	    glVertex3f(0.0, 0.0, 1.0);
	    glVertex3f(0.5, 0.5, 0.5);
	    glVertex3f(0.0, 1.0, 1.0);

	    glVertex3f(1.0, 0.0, 1.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(0.5, 0.5, 0.5);

	    // Front faces
	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(0.5, 0.5, 0.5);
	    glVertex3f(0.0, 0.0, 1.0);

	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(0.5, 0.5, 0.5);

	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(1.0, 0.0, 1.0);
	    glVertex3f(0.5, 0.5, 0.5);

	    // Back faces
	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(0.5, 0.5, 0.5);

	    glVertex3f(0.0, 1.0, 0.0);
	    glVertex3f(0.5, 0.5, 0.5);
	    glVertex3f(1.0, 1.0, 0.0);

	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(0.5, 0.5, 0.5);
	    glVertex3f(1.0, 1.0, 1.0);
	} glEnd();
	break;

	case THREEUP:
	glBegin(GL_TRIANGLES); {
	    // Topmost face
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(0.5, 0.5, 0.5);
	    glVertex3f(1.0, 1.0, 1.0);

	    // Bottommost face
	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(0.5, 0.5, 0.5);

	    // Left faces
	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(0.5, 0.5, 0.5);
	    glVertex3f(0.0, 0.0, 1.0);

	    glVertex3f(0.0, 0.0, 1.0);
	    glVertex3f(0.5, 0.5, 0.5);
	    glVertex3f(0.0, 1.0, 1.0);

	    // Right faces
	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(1.0, 0.0, 1.0);
	    glVertex3f(0.5, 0.5, 0.5);

	    glVertex3f(1.0, 0.0, 1.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(0.5, 0.5, 0.5);
	} glEnd();
	break;

	default: // ALLUP
        glBegin(GL_TRIANGLES); {
	    // Front face
	    glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(1.0, 0.0, 0.0);
            glVertex3f(1.0, 0.0, 1.0);

            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(1.0, 0.0, 1.0);
            glVertex3f(0.0, 0.0, 1.0);

	    // Rear face
	    glVertex3f(0.0, 1.0, 0.0);
            glVertex3f(1.0, 1.0, 1.0);
            glVertex3f(1.0, 1.0, 0.0);

            glVertex3f(0.0, 1.0, 0.0);
            glVertex3f(0.0, 1.0, 1.0);
            glVertex3f(1.0, 1.0, 1.0);

	    // Left face
	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(0.0, 0.0, 1.0);
	    glVertex3f(0.0, 1.0, 1.0);

	    glVertex3f(0.0, 0.0, 0.0);
	    glVertex3f(0.0, 1.0, 1.0);
	    glVertex3f(0.0, 1.0, 0.0);

	    // Right face
	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(1.0, 0.0, 1.0);

	    glVertex3f(1.0, 0.0, 0.0);
	    glVertex3f(1.0, 1.0, 0.0);
	    glVertex3f(1.0, 1.0, 1.0);

	    // Top face
	    glVertex3f(0.0, 0.0, 1.0);
	    glVertex3f(1.0, 0.0, 1.0);
	    glVertex3f(1.0, 1.0, 1.0);

	    glVertex3f(0.0, 0.0, 1.0);
	    glVertex3f(1.0, 1.0, 1.0);
	    glVertex3f(0.0, 1.0, 1.0);
        }glEnd();
	break;
    }
}
