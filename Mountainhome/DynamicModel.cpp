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

#include "Terrain.h"
#include "TranslationMatrix.h"
#include "DynamicModelVertex.h"
#include "DynamicModelFace.h"
#include "DynamicModel.h"

DynamicModel::DynamicModel():
    _matrix(NULL),
    _baseVertex(NULL),
    _baseFace(NULL),
    _vertexCount(0),
    _indexCount(0),
    _renderOp(NULL),
    _vertexArray(NULL),
    _indexBuffer(NULL)
{
    _vertexArray = new VertexArray();
    _vertexArray->setPositionBuffer(new PositionBuffer(GL_DYNAMIC_DRAW, GL_FLOAT, 3));
    _vertexArray->setNormalBuffer(new NormalBuffer(GL_DYNAMIC_DRAW, GL_FLOAT));
    _vertexArray->setTexCoordBuffer(0, new TexCoordBuffer(GL_DYNAMIC_DRAW, GL_FLOAT, 2));

    _indexBuffer = new IndexBuffer(GL_DYNAMIC_DRAW, GL_UNSIGNED_INT);

    _renderOp = new RenderOperation(TRIANGLES, _vertexArray, _indexBuffer);
}

DynamicModel::~DynamicModel() {
    // Do NOT delete these. They are all handled by the Renderable deletion.
    // XXXBMW FIXME: This is a little strange. Should these be passed into the DynamicModel, instead?
    _vertexArray = NULL;
    _indexBuffer = NULL;
    _renderOp = NULL;

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
        // Allocate enough space for a chunk. +1 to handle normals at the edges.
        _matrix = new TranslationMatrix(Terrain::ChunkSize + 1, Terrain::ChunkSize + 1);
    }

    DynamicModelVertex *vertex = _matrix->getVertex(x, y, z, normal);

    if (!vertex) {
        vertex = new DynamicModelVertex(_vertsArray.size(), _vertsArray, normal, &_baseVertex);

        _matrix->setVertex(x, y, z, normal, vertex);

#if 0
        // Add in a little random variation, for flavor.
        // XXXBMW: This ONLY works if poly reduction is turned off :/
        // XXXBMW: P.S. LOOKS REAL BAD, CAPTAIN.
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

RenderOperation * DynamicModel::getRenderOp() {
    return _renderOp;
}

int DynamicModel::getVertexCount() {
    return _vertexCount;
}

int DynamicModel::getIndexCount() {
    return _indexCount;
}

void DynamicModel::updateRenderOp(bool doPolyReduction) {
    if (getVertexCount() == 0) {
        // No vertices to render, so zero out the index buffer.
        _indexBuffer->resize(0, false);
        return;
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
        switch (currentVert->getNormal()) {
        case XY_POS:
        case XY_NEG: texCoords[count] = Vector2(vert.x, vert.y); break;
        case YZ_POS:
        case YZ_NEG: texCoords[count] = Vector2(vert.y, vert.z); break;
        case XZ_POS:
        case XZ_NEG: texCoords[count] = Vector2(vert.x, vert.z); break;
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

    // If we've grown past what our buffers can hold, reserve extra space (more than we
    // need, to prevent unnecessary reallocations).
    if (_vertexArray->getElementCapacity() < getVertexCount()) {
        _vertexArray->reserve((int)(getVertexCount() * 1.5), false);
    }

    if (_indexBuffer->getElementCapacity() < getIndexCount()) {
        _indexBuffer->reserve((int)(getIndexCount() * 1.5), false);
    }

    _vertexArray->getPositionBuffer()->setData(positions, getVertexCount());
    _vertexArray->getTexCoordBuffer(0)->setData(texCoords, getVertexCount());
    _vertexArray->getNormalBuffer()->setData(normals, getVertexCount());
    _indexBuffer->setData(indices, getIndexCount());

    delete indices;
    delete positions;
    delete texCoords;
    delete normals;

    // This object is invalid. Clean everything up.
    clearModel();
}
