/*
 *  Tile.cpp
 *  System
 *
 *  Created by datalurkur on 2/24/10.
 *
 */

#include "Tile.h"

#include <OpenGL/gl.h>

void computeTriNormals(Vector3 *verts, Vector3 *norms, short count) {
  int c;
  Info("Computing normals");
  for(c=0; c<count; c++) {
    Vector3 A, B, C;
    A = verts[3*c+1]-verts[3*c];
    B = verts[3*c+2]-verts[3*c];
    C = A.crossProduct(B);
    C.normalize();
    Info("Computed normal" << C);
    norms[3*c] = norms[3*c+1] = norms[3*c+2] = C;
  }
}

const short CONVEX_CORNER_PRIMCOUNT = 2;
Vector3 convex_corner_vertices[CONVEX_CORNER_PRIMCOUNT*3] = {
  //Vector3(-0.5f,-0.5f,-0.5f),Vector3( 0.5f,-0.5f,-0.5f),Vector3(-0.5f, 0.5f,-0.5f),
  Vector3(-0.5f,-0.5f, 0.5f),Vector3( 0.5f,-0.5f,-0.5f),Vector3(-0.5f, 0.5f,-0.5f)
};
Vector3 convex_corner_normals[CONVEX_CORNER_PRIMCOUNT*3];

const short CONCAVE_CORNER_PRIMCOUNT = 2;
Vector3 concave_corner_vertices[CONCAVE_CORNER_PRIMCOUNT*3] = {
  Vector3(-0.5f,-0.5f,-0.5f),Vector3( 0.5f,-0.5f, 0.5f),Vector3(-0.5f, 0.5f, 0.5f),
  Vector3(-0.5f, 0.5f, 0.5f),Vector3( 0.5f,-0.5f, 0.5f),Vector3( 0.5f, 0.5f, 0.5f)
};
Vector3 concave_corner_normals[CONCAVE_CORNER_PRIMCOUNT*3];

const short PINCH_PRIMCOUNT = 2;
Vector3 pinch_vertices[PINCH_PRIMCOUNT*3] = {
  Vector3(-0.5f,-0.5f,-0.5f),Vector3( 0.5f,-0.5f, 0.5f),Vector3(-0.5f, 0.5f, 0.5f),
  Vector3(-0.5f, 0.5f, 0.5f),Vector3( 0.5f,-0.5f, 0.5f),Vector3( 0.5f, 0.5f, -0.5f)
};
Vector3 pinch_normals[PINCH_PRIMCOUNT*3];

const short RAMP_PRIMCOUNT = 2;
Vector3 ramp_vertices[RAMP_PRIMCOUNT*3] = {
  //Vector3(-0.5f,-0.5f,-0.5f),Vector3( 0.5f,-0.5f,-0.5f),Vector3(-0.5f,-0.5f, 0.5f),
  //Vector3(-0.5f, 0.5f,-0.5f),Vector3(-0.5f, 0.5f, 0.5f),Vector3( 0.5f, 0.5f,-0.5f),
  Vector3(-0.5f,-0.5f, 0.5f),Vector3( 0.5f,-0.5f,-0.5f),Vector3(-0.5f, 0.5f, 0.5f),
  Vector3( 0.5f,-0.5f,-0.5f),Vector3( 0.5f, 0.5f,-0.5f),Vector3(-0.5f, 0.5f, 0.5f)
};
Vector3 ramp_normals[RAMP_PRIMCOUNT*3];

const short WALL_PRIMCOUNT = 10;
Vector3 wall_vertices[WALL_PRIMCOUNT*3] = {
  Vector3(-0.5f,-0.5f,-0.5f),Vector3( 0.5f,-0.5f,-0.5f),Vector3( 0.5f,-0.5f, 0.5f),
  Vector3(-0.5f,-0.5f,-0.5f),Vector3( 0.5f,-0.5f, 0.5f),Vector3(-0.5f,-0.5f, 0.5f),
  Vector3(-0.5f, 0.5f,-0.5f),Vector3( 0.5f, 0.5f, 0.5f),Vector3( 0.5f, 0.5f,-0.5f),
  Vector3(-0.5f, 0.5f,-0.5f),Vector3(-0.5f, 0.5f, 0.5f),Vector3( 0.5f, 0.5f, 0.5f),
  Vector3(-0.5f,-0.5f,-0.5f),Vector3(-0.5f,-0.5f, 0.5f),Vector3(-0.5f, 0.5f, 0.5f),
  Vector3(-0.5f,-0.5f,-0.5f),Vector3(-0.5f, 0.5f, 0.5f),Vector3(-0.5f, 0.5f,-0.5f),
  Vector3( 0.5f,-0.5f,-0.5f),Vector3( 0.5f, 0.5f, 0.5f),Vector3( 0.5f,-0.5f, 0.5f),
  Vector3( 0.5f,-0.5f,-0.5f),Vector3( 0.5f, 0.5f,-0.5f),Vector3( 0.5f, 0.5f, 0.5f),
  Vector3(-0.5f,-0.5f, 0.5f),Vector3( 0.5f,-0.5f, 0.5f),Vector3( 0.5f, 0.5f, 0.5f),
  Vector3(-0.5f,-0.5f, 0.5f),Vector3( 0.5f, 0.5f, 0.5f),Vector3(-0.5f, 0.5f, 0.5f)
};
Vector3 wall_normals[WALL_PRIMCOUNT*3];

Tile::Tile(char type): _type(type) {
    _boundingBox = AABB3(Vector3(0,0,0), Vector3(0.5, 0.5, 0.5));
}

Tile::~Tile() {}

void Tile::render(RenderContext *context) {
  static bool computed = false;
  if(!computed) {
    computeTriNormals(convex_corner_vertices, convex_corner_normals, CONVEX_CORNER_PRIMCOUNT);
    computeTriNormals(concave_corner_vertices, concave_corner_normals, CONCAVE_CORNER_PRIMCOUNT);
    computeTriNormals(pinch_vertices,  pinch_normals,  PINCH_PRIMCOUNT);
    computeTriNormals(ramp_vertices,   ramp_normals,   RAMP_PRIMCOUNT);
    computeTriNormals(wall_vertices,   wall_normals,   WALL_PRIMCOUNT);
    computed = true;
  }

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
    switch(_type)
    {
    case TILE_CONVEX_CORNER:
      glVertexPointer(3,GL_FLOAT,0,convex_corner_vertices);
      glNormalPointer(GL_FLOAT,0,convex_corner_normals);
      glDrawArrays(GL_TRIANGLES,0,CONVEX_CORNER_PRIMCOUNT*3);
    break;
    case TILE_PINCH:
      glVertexPointer(3,GL_FLOAT,0,pinch_vertices);
      glNormalPointer(GL_FLOAT,0,pinch_normals);
      glDrawArrays(GL_TRIANGLES,0,PINCH_PRIMCOUNT*3);
    break;
    case TILE_RAMP:
      glVertexPointer(3,GL_FLOAT,0,ramp_vertices);
      glNormalPointer(GL_FLOAT,0,ramp_normals);
      glDrawArrays(GL_TRIANGLES,0,RAMP_PRIMCOUNT*3);
    break;
	  case TILE_CONCAVE_CORNER:
      glVertexPointer(3,GL_FLOAT,0,concave_corner_vertices);
      glNormalPointer(GL_FLOAT,0,concave_corner_normals);
      glDrawArrays(GL_TRIANGLES,0,CONCAVE_CORNER_PRIMCOUNT*3);
    break;
    default: // WALL
      glVertexPointer(3,GL_FLOAT,0,wall_vertices);
      glNormalPointer(GL_FLOAT,0,wall_normals);
      glDrawArrays(GL_TRIANGLES,0,WALL_PRIMCOUNT*3);
    break;
    }
//  glDisableClientState(GL_VERTEX_ARRAY);
//  glDisableClientState(GL_NORMAL_ARRAY);
}
