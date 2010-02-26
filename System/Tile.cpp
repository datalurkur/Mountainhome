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
      glNormal3f(0.0f, 0.0f, 1.0f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);

      glNormal3f(0.0f, 0.0f, 1.0f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);
      } glEnd();
    break;

    case ONEUP:
      glBegin(GL_TRIANGLES); {
      // Top of ramp faces
      glNormal3f(0.0f, -0.7071f, 0.7071f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);

      glNormal3f(0.0f, -0.7071f, 0.7071f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);

      // Side of ramp faces
      glNormal3f(-1.0f, 0.0f, 0.0f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);

      glNormal3f(1.0f, 0.0f, 0.0f);
      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f,  0.5f);
      } glEnd();
    break;

    case TWOADJUP:
      glBegin(GL_TRIANGLES); {
      // Top face
      glNormal3f(0.0f, 0.0f, 1.0f);
      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f( 0.5f,  0.5f, 0.5f);
      glVertex3f(-0.5f,  0.5f, 0.5f);

      // Side faces
      glNormal3f(0.0f, -1.0f, 0.0f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.0f, -0.5f, -0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);

      glNormal3f(0.0f, 1.0f, 0.0f);
      glVertex3f(0.5f, 0.5f, -0.5f);
      glVertex3f(0.5f, 0.5f,  0.5f);
      glVertex3f(0.5f, 0.0f, -0.5f);

      // Floor face
      glNormal3f(0.0f, 0.0f, 1.0f);
      glVertex3f(0.0f, -0.5f, -0.5f);
      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.0f, -0.5f);

      // Ramp faces
      glNormal3f(-0.4444f, -0.4444f, 0.2222f);
      glVertex3f( 0.0f, -0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);

      glNormal3f(-0.4444f, -0.4444f, 0.2222f);
      glVertex3f(0.0f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.0f, -0.5f);
      glVertex3f(0.5f,  0.5f,  0.5f);
      } glEnd();
    break;

    case TWOOPPUP:
      glBegin(GL_TRIANGLES); {
      // Top faces
      glNormal3f(0.0f, 0.7071f, 0.7071f);
      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f( 0.5f, -0.5f, 0.5f);
      glVertex3f( 0.0f,  0.0f, 0.0f);

      glNormal3f(0.0f, -0.7071f, 0.7071f);
      glVertex3f( 0.0f, 0.0f, 0.0f);
      glVertex3f( 0.5f, 0.5f, 0.5f);
      glVertex3f(-0.5f, 0.5f, 0.5f);

      // Left faces
      glNormal3f(-0.7071f, 0.7071f, 0.0f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f( 0.0f,  0.0f,  0.0f);

      glNormal3f(-0.7071f, 0.0f, 0.7071f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.0f,  0.0f,  0.0f);
      glVertex3f(-0.5f,  0.5f, -0.5f);

      glNormal3f(-0.7071f, -0.7071f, 0.0f);
      glVertex3f(-0.5f, 0.5f, -0.5f);
      glVertex3f( 0.0f, 0.0f,  0.0f);
      glVertex3f(-0.5f, 0.5f,  0.5f);

      // Right faces
      glNormal3f(0.7071f, 0.7071f, 0.0f);
      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.0f,  0.0f,  0.0f);
      glVertex3f(0.5f, -0.5f,  0.5f);

      glNormal3f(0.7071f, 0.0f, 0.7071f);
      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f, -0.5f);
      glVertex3f(0.0f,  0.0f,  0.0f);

      glNormal3f(0.7071f, -0.7071f, 0.0f);
      glVertex3f(0.5f, 0.5f, -0.5f);
      glVertex3f(0.5f, 0.5f,  0.5f);
      glVertex3f(0.0f, 0.0f,  0.0f);
      } glEnd();
    break;

    case THREEUP:
      glBegin(GL_TRIANGLES); {
      // Topmost face
      glNormal3f(0.0f, 0.7071f, 0.7071f);
      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f( 0.5f, -0.5f, 0.5f);
      glVertex3f( 0.0f,  0.0f, 0.0f);

      // Bottommost face
      glNormal3f(0.0f, 0.7071f, 0.7071f);
      glVertex3f(-0.5f, 0.5f, -0.5f);
      glVertex3f( 0.0f, 0.0f,  0.0f);
      glVertex3f( 0.5f, 0.5f, -0.5f);

      // Left faces
      glNormal3f(0.7071f, 0.7071f, 0.0f);
      glVertex3f(-0.5f, 0.5f, -0.5f);
      glVertex3f(-0.5f, 0.5f,  0.5f);
      glVertex3f( 0.0f, 0.0f,  0.0f);

      glNormal3f(0.7071f, 0.0f, 0.7071f);
      glVertex3f(-0.5f,  0.5f, 0.5f);
      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f( 0.0f,  0.0f, 0.0f);

      // Right faces
      glNormal3f(-0.7071f, 0.7071f, 0.0f);
      glVertex3f(0.5f, 0.5f, -0.5f);
      glVertex3f(0.0f, 0.0f,  0.0f);
      glVertex3f(0.5f, 0.5f,  0.5f);

      glNormal3f(-0.7071f, 0.0f, 0.7071f);
      glVertex3f(0.5f,  0.5f, 0.5f);
      glVertex3f(0.0f,  0.0f, 0.0f);
      glVertex3f(0.5f, -0.5f, 0.5f);
      } glEnd();
    break;

    case ALLUP:
      glBegin(GL_TRIANGLES); {
      // Bottom plane
      glNormal3f(0.0f, 0.0f, 1.0f);
      glVertex3f(-0.2f, -0.2f, -0.5f);
      glVertex3f( 0.2f, -0.2f, -0.5f);
      glVertex3f(-0.2f,  0.2f, -0.5f);

      glNormal3f(0.0f, 0.0f, 1.0f);
      glVertex3f(-0.2f,  0.2f, -0.5f);
      glVertex3f( 0.2f, -0.2f, -0.5f);
      glVertex3f( 0.2f,  0.2f, -0.5f);

      // Near ramp
      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f( 0.2f, -0.2f, -0.5f);

      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f( 0.2f, -0.2f, -0.5f);
      glVertex3f(-0.2f, -0.2f, -0.5f);

      // Right ramp
      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f( 0.2f,  0.2f, -0.5f);

      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f( 0.2f,  0.2f, -0.5f);
      glVertex3f( 0.2f, -0.2f, -0.5f);

      // Far ramp
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f(-0.2f,  0.2f, -0.5f);

      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f(-0.2f,  0.2f, -0.5f);
      glVertex3f( 0.2f,  0.2f, -0.5f);

      // Left ramp
      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f(-0.2f, -0.2f, -0.5f);

      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f(-0.2f, -0.2f, -0.5f);
      glVertex3f(-0.2f,  0.2f, -0.5f);
      } glEnd();
    break;

    default: // WALL
      glBegin(GL_TRIANGLES); {
      // Front face
      glNormal3f(0.0f, -1.0f, 0.0f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);

      glNormal3f(0.0f, -1.0f, 0.0f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);

      // Rear face
      glNormal3f(0.0f, 1.0f, 0.0f);
      glVertex3f(-0.5f, 0.5f, -0.5f);
      glVertex3f( 0.5f, 0.5f,  0.5f);
      glVertex3f( 0.5f, 0.5f, -0.5f);

      glNormal3f(0.0f, 1.0f, 0.0f);
      glVertex3f(-0.5f, 0.5f, -0.5f);
      glVertex3f(-0.5f, 0.5f,  0.5f);
      glVertex3f( 0.5f, 0.5f,  0.5f);

      // Left face
      glNormal3f(-1.0f, 0.0f, 0.0f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);

      glNormal3f(-1.0f, 0.0f, 0.0f);
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);

      // Right face
      glNormal3f(1.0f, 0.0f, 0.0f);
      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f,  0.5f);
      glVertex3f(0.5f, -0.5f,  0.5f);

      glNormal3f(1.0f, 0.0f, 0.0f);
      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f,  0.5f);

      // Top face
      glNormal3f(0.0f, 0.0f, 1.0f);
      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f( 0.5f, -0.5f, 0.5f);
      glVertex3f( 0.5f,  0.5f, 0.5f);

      glNormal3f(0.0f, 0.0f, 1.0f);
      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f( 0.5f,  0.5f, 0.5f);
      glVertex3f(-0.5f,  0.5f, 0.5f);
      }glEnd();
    break;
    }
}
