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
    WorldPlane plane)
{
    DynamicModelVertex *one   = addVertex(x1, y1, z1, plane);
    DynamicModelVertex *two   = addVertex(x2, y2, z2, plane);
    DynamicModelVertex *three = addVertex(x3, y3, z3, plane);

    _baseFace = new DynamicModelFace(one, two, three, plane, &_baseFace);

    _indexCount += 3;
}

DynamicModelVertex *DynamicModel::addVertex(
    Real x, Real y, Real z,
    WorldPlane plane)
{
    DynamicModelVertex *vertex = _matrix->getVertex(
        x - _xOffset,
        y - _yOffset,
        z - _zOffset,
        plane);

    if (!vertex) {
        vertex = new DynamicModelVertex(_vertsArray.size(), _vertsArray, plane, &_baseVertex);

        _matrix->setVertex(x - _xOffset, y - _yOffset, z - _zOffset, plane, vertex);

        _vertsArray.push_back(Vector3(x, y, z));
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

Vector3 *DynamicModel::buildStaticVertexArray() {
    Vector3 *verts = new Vector3[getVertexCount()];
    DynamicModelVertex *current = _baseVertex;
    int count = 0;

    while (current) {
        verts[count] = _vertsArray[current->getIndex()];
        current->setIndex(count);
        current = current->next();
        count++;
    }

    return verts;
}

Vector2 *DynamicModel::buildStaticTexCoordArray() {
    Vector2 *texcoords = new Vector2[getVertexCount()];
    DynamicModelVertex *current = _baseVertex;

    int count = 0;
    float factor = 5.0;
    while (current) {
        const Vector3 &vert = _vertsArray[current->getIndex()];
        switch (current->getPlane()) {
        case XY: texcoords[count] = Vector2(vert.x / factor, vert.y / factor); break;
        case YZ: texcoords[count] = Vector2(vert.y / factor, vert.z / factor); break;
        case XZ: texcoords[count] = Vector2(vert.x / factor, vert.z / factor); break;
        default: THROW(ItemNotFoundError, "Unknown plane value: " << current->getPlane());
        }

        current = current->next();
        count++;
    }

    return texcoords;
}

unsigned int *DynamicModel::buildStaticIndexArray() {
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
