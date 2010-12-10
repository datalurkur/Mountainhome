/*
 *  DynamicModelFace.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "DynamicModelVertex.h"
#include "DynamicModelFace.h"

DynamicModelFace::DynamicModelFace(
    DynamicModelVertex *one,
    DynamicModelVertex *two,
    DynamicModelVertex *three,
    int normal,
    DynamicModelFace **base
):
    _normal(normal),
    _base(base),
    _next(*base),
    _prev(NULL)
{
    if (_next) { _next->_prev = this; }

    ASSERT(one);
    ASSERT(two);
    ASSERT(three);
    ASSERT_GE(_normal, 0);
    ASSERT_LE(_normal, 5);

    _indices[0] = one;
    _indices[1] = two;
    _indices[2] = three;

    for (int i = 0; i < 3; i++) {
        _indices[i]->addFace(this);
    }
}

DynamicModelFace::~DynamicModelFace() {
    if (this == *_base) {
        ASSERT(!_prev);
        *_base = _next;
    }

    if (_next) { _next->_prev = _prev; }
    if (_prev) { _prev->_next = _next; }
}

bool DynamicModelFace::isCollapsed() {
    return _indices[0] == _indices[1] ||
           _indices[1] == _indices[2] ||
           _indices[2] == _indices[0];
}

bool DynamicModelFace::hasVertex(DynamicModelVertex *lhs) {
    for (int i = 0; i < 3; i++) {
        if (_indices[i] == lhs) { return true; }
    }

    return false;
}

int DynamicModelFace::getNormal() {
    return _normal;
}

DynamicModelVertex* DynamicModelFace::getVertex(int i) {
    return _indices[i];
}

bool DynamicModelFace::replaceVertex(DynamicModelVertex *oldVertex, DynamicModelVertex *newVertex) {
    // Replace oldVertex with newVertex.
    for (int i = 0; i < 3; i++) {
        if (_indices[i] == oldVertex) {
            _indices[i] = newVertex;
            break;
        }
    }

    // If the face has collapsed, orphan it from its indices and notify the caller that
    // the face is no longer visible.
    if (isCollapsed()) {
        for (int i = 0; i < 3; i++) {
            ///\todo This will call removeFace on the same vertex twice!
            _indices[i]->removeFace(this);
            _indices[i] = NULL;
        }

        return false;
    }

    // Otherwise, update and new vertex's face list and notify the caller that this face is
    // still valid. Don't bother updating the oldVertex as it will just be deleted.
    newVertex->addFace(this);
    return true;
}

DynamicModelFace* DynamicModelFace::next() { return _next; }

DynamicModelFace* DynamicModelFace::prev() { return _prev; }
