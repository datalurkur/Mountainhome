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

DynamicModelFace::DynamicModelFace(DynamicModelIndex *one, DynamicModelIndex *two, DynamicModelIndex *three, int plane, DynamicModelFace *next, DynamicModelFace **base):
_plane(plane), _base(base), _next(next), _prev(NULL) {
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

int DynamicModelFace::plane() {
    return _plane;
}

DynamicModelIndex* DynamicModelFace::getIndex(int i) {
    ASSERT_GE(i, 0);
    ASSERT_LE(i, 2);
    return _indices[i];
}

void DynamicModelFace::setIndex(int i, DynamicModelIndex* index) {
    ASSERT_GE(i, 0);
    ASSERT_LE(i, 2);
    ASSERT(index);

    _indices[i] = index;
}

DynamicModelFace* DynamicModelFace::next() { return _next; }

DynamicModelFace* DynamicModelFace::prev() { return _prev; }
