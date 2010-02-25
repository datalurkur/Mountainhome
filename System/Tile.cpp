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
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);

      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);
      } glEnd();
    break;

    case ONEUP:
      glBegin(GL_TRIANGLES); {
      // Top of ramp faces
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);

      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);

      // Side of ramp faces
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);

      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f,  0.5f);
      } glEnd();
    break;

    case TWOADJUP:
      glBegin(GL_TRIANGLES); {
      // Top face
      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f( 0.5f,  0.5f, 0.5f);
      glVertex3f(-0.5f,  0.5f, 0.5f);

      // Side faces
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.0f, -0.5f, -0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);

      glVertex3f(0.5f, 0.5f, -0.5f);
      glVertex3f(0.5f, 0.5f,  0.5f);
      glVertex3f(0.5f, 0.0f, -0.5f);

      // Floor face
      glVertex3f(0.0f, -0.5f, -0.5f);
      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.0f, -0.5f);

      // Ramp faces
      glVertex3f( 0.0f, -0.5f, -0.5f);
      glVertex3f( 0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);

      glVertex3f(0.0f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.0f, -0.5f);
      glVertex3f(0.5f,  0.5f,  0.5f);
      } glEnd();
    break;

    case TWOOPPUP:
      glBegin(GL_TRIANGLES); {
      // Top faces
      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f( 0.5f, -0.5f, 0.5f);
      glVertex3f( 0.0f,  0.0f, 0.0f);

      glVertex3f( 0.0f, 0.0f, 0.0f);
      glVertex3f( 0.5f, 0.5f, 0.5f);
      glVertex3f(-0.5f, 0.5f, 0.5f);

      // Left faces
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f( 0.0f,  0.0f,  0.0f);

      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.0f,  0.0f,  0.0f);
      glVertex3f(-0.5f,  0.5f, -0.5f);

      glVertex3f(-0.5f, 0.5f, -0.5f);
      glVertex3f( 0.0f, 0.0f,  0.0f);
      glVertex3f(-0.5f, 0.5f,  0.5f);

      // Right faces
      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.0f,  0.0f,  0.0f);
      glVertex3f(0.5f, -0.5f,  0.5f);

      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f, -0.5f);
      glVertex3f(0.0f,  0.0f,  0.0f);

      glVertex3f(0.5f, 0.5f, -0.5f);
      glVertex3f(0.5f, 0.5f,  0.5f);
      glVertex3f(0.0f, 0.0f,  0.0f);
      } glEnd();
    break;

    case THREEUP:
      glBegin(GL_TRIANGLES); {
      // Topmost face
      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f( 0.5f, -0.5f, 0.5f);
      glVertex3f( 0.0f,  0.0f, 0.0f);

      // Bottommost face
      glVertex3f(-0.5f, 0.5f, -0.5f);
      glVertex3f( 0.0f, 0.0f,  0.0f);
      glVertex3f( 0.5f, 0.5f, -0.5f);

      // Left faces
      glVertex3f(-0.5f, 0.5f, -0.5f);
      glVertex3f(-0.5f, 0.5f,  0.5f);
      glVertex3f( 0.0f, 0.0f,  0.0f);

      glVertex3f(-0.5f,  0.5f, 0.5f);
      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f( 0.0f,  0.0f, 0.0f);

      // Right faces
      glVertex3f(0.5f, 0.5f, -0.5f);
      glVertex3f(0.0f, 0.0f,  0.0f);
      glVertex3f(0.5f, 0.5f,  0.5f);

      glVertex3f(0.5f,  0.5f, 0.5f);
      glVertex3f(0.0f,  0.0f, 0.0f);
      glVertex3f(0.5f, -0.5f, 0.5f);
      } glEnd();
    break;

    case ALLUP:
      glBegin(GL_TRIANGLES); {
      // Bottom plane
      glVertex3f(-0.2f, -0.2f, -0.5f);
      glVertex3f( 0.2f, -0.2f, -0.5f);
      glVertex3f(-0.2f,  0.2f, -0.5f);

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
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);

      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f( 0.5f, -0.5f,  0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);

      // Rear face
      glVertex3f(-0.5f, 0.5f, -0.5f);
      glVertex3f( 0.5f, 0.5f,  0.5f);
      glVertex3f( 0.5f, 0.5f, -0.5f);

      glVertex3f(-0.5f, 0.5f, -0.5f);
      glVertex3f(-0.5f, 0.5f,  0.5f);
      glVertex3f( 0.5f, 0.5f,  0.5f);

      // Left face
      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f, -0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);

      glVertex3f(-0.5f, -0.5f, -0.5f);
      glVertex3f(-0.5f,  0.5f,  0.5f);
      glVertex3f(-0.5f,  0.5f, -0.5f);

      // Right face
      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f,  0.5f);
      glVertex3f(0.5f, -0.5f,  0.5f);

      glVertex3f(0.5f, -0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f, -0.5f);
      glVertex3f(0.5f,  0.5f,  0.5f);

      // Top face
      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f( 0.5f, -0.5f, 0.5f);
      glVertex3f( 0.5f,  0.5f, 0.5f);

      glVertex3f(-0.5f, -0.5f, 0.5f);
      glVertex3f( 0.5f,  0.5f, 0.5f);
      glVertex3f(-0.5f,  0.5f, 0.5f);
      }glEnd();
    break;
    }
}
