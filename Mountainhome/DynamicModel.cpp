/*
 *  DynamicModel.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/4/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "TranslationMatrix.h"
#include "DynamicModelVertex.h"
#include "DynamicModelFace.h"
#include "DynamicModel.h"

DynamicModel::DynamicModel(
    int width, int height, int xOffset, int yOffset, int zOffset
):
    _matrix(NULL),
    _baseVertex(NULL),
    _baseFace(NULL),
    _vertexCount(0),
    _indexCount(0),
    _xOffset(xOffset),
    _yOffset(yOffset),
    _zOffset(zOffset)
{
    _matrix = new TranslationMatrix(width, height);
}

DynamicModel::~DynamicModel() {
    // Delete all of the remainign indices.
    DynamicModelVertex *currentIndex = _baseVertex;
    DynamicModelVertex *nextIndex;

    while (currentIndex) {
        nextIndex = currentIndex->next();
        delete currentIndex;
        currentIndex = nextIndex;
    }

    // Delete all remaining faces.
    DynamicModelFace *nextFace, *currentFace = _baseFace;
    while (currentFace) {
        nextFace = currentFace->next();
        delete currentFace;
        currentFace = nextFace;
    }

    delete _matrix;
}

void DynamicModel::doPolyReduction() {
    // Update the flags.
    DynamicModelVertex *currentVertex = _baseVertex;
    while (currentVertex) {
        currentVertex->calculateEdgeFlags();
        currentVertex = currentVertex->next();
    }

    // Do the reduction.
    bool keepMerging = true;
    while (keepMerging) {
        keepMerging = false;
        currentVertex = _baseVertex;
        while (currentVertex) {
            if (currentVertex->absorbNeighbors()) {
                keepMerging = true;
            }

            currentVertex = currentVertex->next();
        }
    }

    // Update vertex count.
    currentVertex = _baseVertex;
    _vertexCount = 0;

    while (currentVertex) {
        currentVertex = currentVertex->next();
        _vertexCount++;
    }

    // Update the index count.
    DynamicModelFace *currentFace = _baseFace;
    _indexCount = 0;

    while (currentFace) {
        currentFace = currentFace->next();
        _indexCount += 3;
    }
}

void DynamicModel::addFace(
    Real x1, Real y1, Real z1,
    Real x2, Real y2, Real z2,
    Real x3, Real y3, Real z3,
    WorldNormal normal)
{
    DynamicModelVertex *one   = addVertex(x1, y1, z1, normal);
    DynamicModelVertex *two   = addVertex(x2, y2, z2, normal);
    DynamicModelVertex *three = addVertex(x3, y3, z3, normal);

    _baseFace = new DynamicModelFace(one, two, three, normal, &_baseFace);

    _indexCount += 3;
}

DynamicModelVertex *DynamicModel::addVertex(
    Real x, Real y, Real z,
    WorldNormal normal)
{
    DynamicModelVertex *vertex = _matrix->getVertex(
        x - _xOffset,
        y - _yOffset,
        z - _zOffset,
        normal);

    if (!vertex) {
        vertex = new DynamicModelVertex(_vertsArray.size(), _vertsArray, normal, &_baseVertex);

        _matrix->setVertex(x - _xOffset, y - _yOffset, z - _zOffset, normal, vertex);

#if 0
        // XXXBMW: This ONLY works if poly reduction is turned off :/
        // Add in a little random variation, for flavor.
        srand(x + y + z);
        float spread = 0.1 / RAND_MAX;
        _vertsArray.push_back(Vector3(
            rand() * spread + x,
            rand() * spread + y,
            rand() * spread + z));
#else
        _vertsArray.push_back(Vector3(x, y, z));
#endif
    }

    _vertexCount++;
    return vertex;
}

int DynamicModel::getVertexCount() {
    return _vertexCount;
}

int DynamicModel::getIndexCount() {
    return _indexCount;
}

Vector3 * DynamicModel::buildStaticVertexArray() {
    Vector3 *verts = new Vector3[getVertexCount()];
    DynamicModelVertex *current = _baseVertex;

    for (int count = 0; current; count++, current = current->next()) {
        verts[count] = _vertsArray[current->getIndex()];
        current->setIndex(count);
    }

    return verts;
}

Vector2 * DynamicModel::buildStaticTexCoordArray() {
    Vector2 *texcoords = new Vector2[getVertexCount()];
    DynamicModelVertex *current = _baseVertex;

    float factor = 5.0;
    for (int count = 0; current; count++, current = current->next()) {
        const Vector3 &vert = _vertsArray[current->getIndex()];
        switch (current->getNormal()) {
        case XY_POS:
        case XY_NEG: texcoords[count] = Vector2(vert.x / factor, vert.y / factor); break;
        case YZ_POS:
        case YZ_NEG: texcoords[count] = Vector2(vert.y / factor, vert.z / factor); break;
        case XZ_POS:
        case XZ_NEG: texcoords[count] = Vector2(vert.x / factor, vert.z / factor); break;
        default: THROW(ItemNotFoundError, "Unknown normal value: " << current->getNormal());
        }
    }

    return texcoords;
}

Vector3 * DynamicModel::buildStaticNormalArray() {
    Vector3 *normals = new Vector3[getVertexCount()];
    DynamicModelVertex *current = _baseVertex;

    for (int count = 0; current; count++, current = current->next()) {
        switch (current->getNormal()) {
        case XY_POS: normals[count] = Vector3( 0,  0,  1); break;
        case XY_NEG: normals[count] = Vector3( 0,  0, -1); break;
        case YZ_POS: normals[count] = Vector3( 1,  0,  0); break;
        case YZ_NEG: normals[count] = Vector3(-1,  0,  0); break;
        case XZ_POS: normals[count] = Vector3( 0,  1,  0); break;
        case XZ_NEG: normals[count] = Vector3( 0, -1,  0); break;
        default: THROW(ItemNotFoundError, "Unknown normal value: " << current->getNormal());
        }
    }

    return normals;
}

unsigned int * DynamicModel::buildStaticIndexArray() {
    unsigned int *indices = new unsigned int[getIndexCount()];
    DynamicModelFace *current = _baseFace;
    int count = 0;

    while (current) {
        for (int i = 0; i < 3; i++) {
            indices[count + i] = current->getVertex(i)->getIndex();
        }

        current = current->next();
        count += 3;
    }

    return indices;
}
