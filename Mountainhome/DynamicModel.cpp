/*
 *  DynamicModel.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/4/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "TranslationMatrix.h"
#include "DynamicModelIndex.h"
#include "DynamicModelFace.h"
#include "DynamicModel.h"

DynamicModel::DynamicModel(int width, int height, int xOffset, int yOffset, int zOffset):
_matrix(NULL), _baseIndex(NULL), _baseFace(NULL), _xOffset(xOffset), _yOffset(yOffset),
_zOffset(zOffset)
{
    _matrix = new TranslationMatrix(width, height);
}

DynamicModel::~DynamicModel() {
    // Delete all of the remainign indices.
    DynamicModelIndex *currentIndex = _baseIndex;
    DynamicModelIndex *nextIndex;

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
    bool keepMerging = true;
    while (keepMerging) {
        keepMerging = false;
        DynamicModelIndex *current = _baseIndex;
        while (current) {
            if (current->absorbNeighbors()) {
                keepMerging = true;
            }

            current = current->next();
        }
    }
}

void DynamicModel::addFace(
    Real x1, Real y1, Real z1,
    Real x2, Real y2, Real z2,
    Real x3, Real y3, Real z3,
    int plane)
{
    DynamicModelIndex *one   = addVertex(x1, y1, z1);
    DynamicModelIndex *two   = addVertex(x2, y2, z2);
    DynamicModelIndex *three = addVertex(x3, y3, z3);

    _baseFace = new DynamicModelFace(one, two, three, plane, _baseFace, &_baseFace);
}

DynamicModelIndex *DynamicModel::addVertex(Real x, Real y, Real z) {
    DynamicModelIndex *index = _matrix->getIndex(x - _xOffset, y - _yOffset, z - _zOffset);

    if (!index) {
        _baseIndex = index = new DynamicModelIndex(_vertsArray.size(), _vertsArray, _baseIndex, &_baseIndex);

        _matrix->setIndex(x - _xOffset, y - _yOffset, z - _zOffset, _baseIndex);

        _vertsArray.push_back(Vector3(x, y, z));
    }

    return index;
}

int DynamicModel::getVertexCount() {
    return _vertsArray.size();
}

int DynamicModel::getIndexCount() {
    DynamicModelFace *current = _baseFace;
    int count = 0;

    while (current) {
        current = current->next();
        count++;
    }
    
    return count * 3;
}

Vector3 *DynamicModel::buildStaticVertexArray()   { return vector_to_array(_vertsArray); }

unsigned int *DynamicModel::buildStaticIndexArray() {
    unsigned int *indices = new unsigned int[getIndexCount()];
    DynamicModelFace *current = _baseFace;
    int count = 0;

    while (current) {
        for (int i = 0; i < 3; i++) {
            indices[count + i] = current->getIndex(i)->vIndex();
        }

        current = current->next();
        count += 3;
    }

    return indices;
}
