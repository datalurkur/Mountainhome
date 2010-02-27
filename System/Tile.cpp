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

const short NONEUP_PRIMCOUNT = 10;
Vector3 noneup_vertices[NONEUP_PRIMCOUNT*3] = {
  Vector3(-0.2f,-0.2f, 0.5f),Vector3( 0.2f,-0.2f, 0.5f),Vector3(-0.2f, 0.2f, 0.5f),
  Vector3(-0.2f, 0.2f, 0.5f),Vector3( 0.2f,-0.2f, 0.5f),Vector3( 0.2f, 0.2f, 0.5f),
  Vector3(-0.5f,-0.5f,-0.5f),Vector3( 0.5f,-0.5f,-0.5f),Vector3( 0.2f,-0.2f, 0.5f),
  Vector3(-0.5f,-0.5f,-0.5f),Vector3( 0.2f,-0.2f, 0.5f),Vector3(-0.2f,-0.2f, 0.5f),
  Vector3( 0.5f,-0.5f,-0.5f),Vector3( 0.5f, 0.5f,-0.5f),Vector3( 0.2f, 0.2f, 0.5f),
  Vector3( 0.5f,-0.5f,-0.5f),Vector3( 0.2f, 0.2f, 0.5f),Vector3( 0.2f,-0.2f, 0.5f),
  Vector3( 0.5f, 0.5f,-0.5f),Vector3(-0.5f, 0.5f,-0.5f),Vector3(-0.2f, 0.2f, 0.5f),
  Vector3( 0.5f, 0.5f,-0.5f),Vector3(-0.2f, 0.2f, 0.5f),Vector3( 0.2f, 0.2f, 0.5f),
  Vector3(-0.5f,-0.5f,-0.5f),Vector3(-0.2f,-0.2f, 0.5f),Vector3(-0.5f, 0.5f,-0.5f),
  Vector3(-0.5f, 0.5f,-0.5f),Vector3(-0.2f,-0.2f, 0.5f),Vector3(-0.2f, 0.2f, 0.5f)
};
Vector3 noneup_normals[NONEUP_PRIMCOUNT*3];

const short ONEUP_PRIMCOUNT = 4;
Vector3 oneup_vertices[ONEUP_PRIMCOUNT*3] = {
  Vector3(-0.5f,-0.5f,-0.5f),Vector3( 0.5f,-0.5f,-0.5f),Vector3(-0.5f, 0.5f, 0.5f),
  Vector3( 0.5f,-0.5f,-0.5f),Vector3( 0.5f, 0.5f, 0.5f),Vector3(-0.5f, 0.5f, 0.5f),
  Vector3(-0.5f,-0.5f,-0.5f),Vector3(-0.5f, 0.5f, 0.5f),Vector3(-0.5f, 0.5f,-0.5f),
  Vector3( 0.5f,-0.5f,-0.5f),Vector3( 0.5f, 0.5f,-0.5f),Vector3( 0.5f, 0.5f, 0.5f)
};
Vector3 oneup_normals[ONEUP_PRIMCOUNT*3];

const short TWOADJUP_PRIMCOUNT = 6;
Vector3 twoadjup_vertices[TWOADJUP_PRIMCOUNT*3] = {
  Vector3(-0.5f,-0.5f, 0.5f),Vector3( 0.5f, 0.5f, 0.5f),Vector3(-0.5f, 0.5f, 0.5f),
  Vector3(-0.5f,-0.5f,-0.5f),Vector3( 0.0f,-0.5f,-0.5f),Vector3(-0.5f,-0.5f, 0.5f),
  Vector3( 0.5f, 0.5f,-0.5f),Vector3( 0.5f, 0.5f, 0.5f),Vector3( 0.5f, 0.0f,-0.5f),
  Vector3( 0.0f,-0.5f,-0.5f),Vector3( 0.5f,-0.5f,-0.5f),Vector3( 0.5f, 0.0f,-0.5f),
  Vector3( 0.0f,-0.5f,-0.5f),Vector3( 0.5f, 0.5f, 0.5f),Vector3(-0.5f,-0.5f, 0.5f),
  Vector3( 0.0f,-0.5f,-0.5f),Vector3( 0.5f, 0.0f,-0.5f),Vector3( 0.5f, 0.5f, 0.5f)
};
Vector3 twoadjup_normals[TWOADJUP_PRIMCOUNT*3];

const short TWOOPPUP_PRIMCOUNT = 8;
Vector3 twooppup_vertices[TWOOPPUP_PRIMCOUNT*3] = {
  Vector3(-0.5f,-0.5f, 0.5f),Vector3( 0.5f,-0.5f, 0.5f),Vector3( 0.0f, 0.0f, 0.0f),
  Vector3( 0.0f, 0.0f, 0.0f),Vector3( 0.5f, 0.5f, 0.5f),Vector3(-0.5f, 0.5f, 0.5f),
  Vector3(-0.5f,-0.5f,-0.5f),Vector3(-0.5f,-0.5f, 0.5f),Vector3( 0.0f, 0.0f, 0.0f),
  Vector3(-0.5f,-0.5f,-0.5f),Vector3( 0.0f, 0.0f, 0.0f),Vector3(-0.5f, 0.5f,-0.5f),
  Vector3(-0.5f, 0.5f,-0.5f),Vector3( 0.0f, 0.0f, 0.0f),Vector3(-0.5f, 0.5f, 0.5f),
  Vector3( 0.5f,-0.5f,-0.5f),Vector3( 0.0f, 0.0f, 0.0f),Vector3( 0.5f,-0.5f, 0.5f),
  Vector3( 0.5f,-0.5f,-0.5f),Vector3( 0.5f, 0.5f,-0.5f),Vector3( 0.0f, 0.0f, 0.0f),
  Vector3( 0.5f, 0.5f,-0.5f),Vector3( 0.5f, 0.5f, 0.5f),Vector3( 0.0f, 0.0f, 0.0f),
};
Vector3 twooppup_normals[TWOOPPUP_PRIMCOUNT*3];

const short THREEUP_PRIMCOUNT = 6;
Vector3 threeup_vertices[THREEUP_PRIMCOUNT*3] = {
  Vector3(-0.5f,-0.5f, 0.5f),Vector3( 0.5f,-0.5f, 0.5f),Vector3( 0.0f, 0.0f, 0.0f),
  Vector3(-0.5f, 0.5f,-0.5f),Vector3( 0.0f, 0.0f, 0.0f),Vector3( 0.5f, 0.5f,-0.5f),
  Vector3(-0.5f, 0.5f,-0.5f),Vector3(-0.5f, 0.5f, 0.5f),Vector3( 0.0f, 0.0f, 0.0f),
  Vector3(-0.5f, 0.5f, 0.5f),Vector3(-0.5f,-0.5f, 0.5f),Vector3( 0.0f, 0.0f, 0.0f),
  Vector3( 0.5f, 0.5f,-0.5f),Vector3( 0.0f, 0.0f, 0.0f),Vector3( 0.5f, 0.5f, 0.5f),
  Vector3( 0.5f, 0.5f, 0.5f),Vector3( 0.0f, 0.0f, 0.0f),Vector3( 0.5f,-0.5f, 0.5f)
};
Vector3 threeup_normals[THREEUP_PRIMCOUNT*3];

const short ALLUP_PRIMCOUNT = 2;
Vector3 allup_vertices[ALLUP_PRIMCOUNT*3] = {
  Vector3(-0.5f,-0.5f, 0.5f),Vector3( 0.5f,-0.5f, 0.5f),Vector3(-0.5f, 0.5f, 0.5f),
  Vector3( 0.5f,-0.5f, 0.5f),Vector3( 0.5f, 0.5f, 0.5f),Vector3(-0.5f, 0.5f, 0.5f)
};
Vector3 allup_normals[ALLUP_PRIMCOUNT*3];

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
  static bool computed=false;
  if(!computed) {
    computeTriNormals(noneup_vertices, noneup_normals, NONEUP_PRIMCOUNT);
    computeTriNormals(oneup_vertices, oneup_normals, ONEUP_PRIMCOUNT);
    computeTriNormals(twoadjup_vertices, twoadjup_normals, TWOADJUP_PRIMCOUNT);
    computeTriNormals(twooppup_vertices, twooppup_normals, TWOOPPUP_PRIMCOUNT);
    computeTriNormals(threeup_vertices, threeup_normals, THREEUP_PRIMCOUNT);
    computeTriNormals(allup_vertices, allup_normals, ALLUP_PRIMCOUNT);
    computeTriNormals(wall_vertices, wall_normals, WALL_PRIMCOUNT);
    computed=true;
  }

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
    switch(_type)
    {
    case NONEUP:
      glVertexPointer(3,GL_FLOAT,0,noneup_vertices);
      glNormalPointer(GL_FLOAT,0,noneup_normals);
      glDrawArrays(GL_TRIANGLES,0,NONEUP_PRIMCOUNT*3);
    break;
    case ONEUP:
      glVertexPointer(3,GL_FLOAT,0,oneup_vertices);
      glNormalPointer(GL_FLOAT,0,oneup_normals);
      glDrawArrays(GL_TRIANGLES,0,ONEUP_PRIMCOUNT*3);
    break;
    case TWOADJUP:
      glVertexPointer(3,GL_FLOAT,0,twoadjup_vertices);
      glNormalPointer(GL_FLOAT,0,twoadjup_normals);
      glDrawArrays(GL_TRIANGLES,0,TWOADJUP_PRIMCOUNT*3);
    break;
    case TWOOPPUP:
      glVertexPointer(3,GL_FLOAT,0,twooppup_vertices);
      glNormalPointer(GL_FLOAT,0,twooppup_normals);
      glDrawArrays(GL_TRIANGLES,0,TWOOPPUP_PRIMCOUNT*3);
    break;
    case THREEUP:
      glVertexPointer(3,GL_FLOAT,0,threeup_vertices);
      glNormalPointer(GL_FLOAT,0,threeup_normals);
      glDrawArrays(GL_TRIANGLES,0,THREEUP_PRIMCOUNT*3);
    break;
    case ALLUP:
      glVertexPointer(3,GL_FLOAT,0,allup_vertices);
      glNormalPointer(GL_FLOAT,0,allup_normals);
      glDrawArrays(GL_TRIANGLES,0,ALLUP_PRIMCOUNT*3);
    break;
    default: // WALL
      glVertexPointer(3,GL_FLOAT,0,wall_vertices);
      glNormalPointer(GL_FLOAT,0,wall_normals);
      glDrawArrays(GL_TRIANGLES,0,WALL_PRIMCOUNT*3);
    break;
    }
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
}
