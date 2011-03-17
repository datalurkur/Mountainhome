/*
 *  DynamicModel.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/4/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Render/RenderOperation.h>
#include <Render/VertexArray.h>
#include <Render/Buffer.h>

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
    _zOffset(zOffset),
    _width(width),
    _height(height)
{}

DynamicModel::~DynamicModel() {
    clearModel();
}

void DynamicModel::clearModel() {
    // Delete all of the remainign indices.
    DynamicModelVertex *nextIndex, *currentIndex = _baseVertex;
    while (currentIndex) {
        nextIndex = currentIndex->next();
        delete currentIndex;
        currentIndex = nextIndex;
    }

    _baseVertex = NULL;


    // Delete all remaining faces.
    DynamicModelFace *nextFace, *currentFace = _baseFace;
    while (currentFace) {
        nextFace = currentFace->next();
        delete currentFace;
        currentFace = nextFace;
    }

    _baseFace = NULL;

    // Delete the translation matrix.
    delete _matrix;
    _matrix = NULL;

    // Clear the counts.
    _vertexCount = 0;
    _indexCount = 0;
}

void DynamicModel::cullModelPrims() {
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
    if (!_matrix) {
        _matrix = new TranslationMatrix(_width, _height);
    }

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

RenderOperation * DynamicModel::generateRenderOp(bool doPolyReduction) {
    if (getVertexCount() == 0) {
        return NULL;
    }

    if (doPolyReduction) {
        cullModelPrims();
    }

    unsigned int *indices   = new unsigned int[getIndexCount()];
    Vector3      *positions = new Vector3[getVertexCount()];
    Vector2      *texCoords = new Vector2[getVertexCount()];
    Vector3      *normals   = new Vector3[getVertexCount()];

    DynamicModelVertex *currentVert;
    DynamicModelFace *currentFace;
    int count;

    // We loop over the DynamicModelVertex list rather than using the _vertsArray vector
    // to make sure we skip any unused vertices (many of which may have been removed in
    // the poly reduction code.
    for (count = 0, currentVert = _baseVertex; currentVert; count++, currentVert = currentVert->next()) {
        const Vector3 &vert = _vertsArray[currentVert->getIndex()];

        // Set the position.
        positions[count] = vert;

        // Set the tex coord.
        float factor = 5.0;
        switch (currentVert->getNormal()) {
        case XY_POS:
        case XY_NEG: texCoords[count] = Vector2(vert.x / factor, vert.y / factor); break;
        case YZ_POS:
        case YZ_NEG: texCoords[count] = Vector2(vert.y / factor, vert.z / factor); break;
        case XZ_POS:
        case XZ_NEG: texCoords[count] = Vector2(vert.x / factor, vert.z / factor); break;
        default: THROW(ItemNotFoundError, "Unknown normal value: " << currentVert->getNormal());
        }

        // Set the normal.
        switch (currentVert->getNormal()) {
        case XY_POS: normals[count] = Vector3( 0,  0,  1); break;
        case XY_NEG: normals[count] = Vector3( 0,  0, -1); break;
        case YZ_POS: normals[count] = Vector3( 1,  0,  0); break;
        case YZ_NEG: normals[count] = Vector3(-1,  0,  0); break;
        case XZ_POS: normals[count] = Vector3( 0,  1,  0); break;
        case XZ_NEG: normals[count] = Vector3( 0, -1,  0); break;
        default: THROW(ItemNotFoundError, "Unknown normal value: " << currentVert->getNormal());
        }

        // Update the index of the current DynamicModelVertex from the index into
        // _vertsArray to the index into our new static arrays. This allows us to generate
        // proper indices in the next step, but will completely invalidate this object for
        // future use.
        currentVert->setIndex(count);
    }

    // Generate the index list from our faces.
    for (count = 0, currentFace = _baseFace; currentFace; count += 3, currentFace = currentFace->next()) {
        for (int i = 0; i < 3; i++) {
            indices[count + i] = currentFace->getVertex(i)->getIndex();
        }
    }

    VertexArray *vertexBuffer = new VertexArray();
    vertexBuffer->setPositionBuffer(new PositionBuffer(
        GL_STATIC_DRAW, GL_FLOAT, 3, getVertexCount(), positions));
    vertexBuffer->setNormalBuffer(new NormalBuffer(
      GL_STATIC_DRAW, GL_FLOAT, getVertexCount(), normals));
    vertexBuffer->setTexCoordBuffer(0, new TexCoordBuffer(
        GL_STATIC_DRAW, GL_FLOAT, 2, getVertexCount(), texCoords));

    IndexBuffer *indexBuffer = new IndexBuffer(GL_STATIC_DRAW, GL_UNSIGNED_INT, getIndexCount(), indices);

    RenderOperation *retVal = new RenderOperation(TRIANGLES, vertexBuffer, indexBuffer);

    delete indices;
    delete positions;
    delete texCoords;
    delete normals;

    // This object is invalid. Clean everything up.
    clearModel();

    return retVal;
}
