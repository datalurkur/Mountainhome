/*
 *  DynamicModelFace.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "DynamicModelIndex.h"
#include "DynamicModelFace.h"

DynamicModelFace::DynamicModelFace(
    DynamicModelIndex *one,
    DynamicModelIndex *two,
    DynamicModelIndex *three,
    int plane,
    DynamicModelFace **base
):
    _plane(plane),
    _base(base),
    _next(*base),
    _prev(NULL)
{
    if (_next) { _next->_prev = this; }

    ASSERT(one);
    ASSERT(two);
    ASSERT(three);
    ASSERT_GE(_plane, 0);
    ASSERT_LE(_plane, 4);

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

bool DynamicModelFace::hasIndex(DynamicModelIndex *lhs) {
    for (int i = 0; i < 3; i++) {
        if (_indices[i] == lhs) { return true; }
    }

    return false;
}

int DynamicModelFace::plane() {
    return _plane;
}

DynamicModelIndex* DynamicModelFace::getIndex(int i) {
    return _indices[i];
}

bool DynamicModelFace::replaceIndex(DynamicModelIndex *oldIndex, DynamicModelIndex *newIndex) {
    // Replace oldIndex with newIndex.
    for (int i = 0; i < 3; i++) {
        if (_indices[i] == oldIndex) {
            _indices[i] = newIndex;
            break;
        }
    }

    // If the face has collapsed, orphan it from its indices and notify the caller that
    // the face is no longer visible.
    if (isCollapsed()) {
        for (int i = 0; i < 3; i++) {
            ///\todo This will call removeFace on the same index twice!
            _indices[i]->removeFace(this);
            _indices[i] = NULL;
        }

        return false;
    }

    // Otherwise, update and new index's face list and notify the caller that this face is
    // still valid. Don't bother updating the oldIndex as it will just be deleted.
    newIndex->addFace(this);
    return true;
}

DynamicModelFace* DynamicModelFace::next() { return _next; }

DynamicModelFace* DynamicModelFace::prev() { return _prev; }
