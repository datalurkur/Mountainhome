/*
 *  DynamicModelIndex.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/AABB.h>

#include "DynamicModelIndex.h"
#include "DynamicModelFace.h"
#include "DynamicModel.h"

#define PRINTEDGE(prefix, edge) \
    Info(prefix << \
        " NX: " << ((edge & NX) ? 1 : 0) << \
        " PX: " << ((edge & PX) ? 1 : 0) << \
        " NY: " << ((edge & NY) ? 1 : 0) << \
        " PY: " << ((edge & PY) ? 1 : 0) << \
        " NZ: " << ((edge & NZ) ? 1 : 0) << \
        " PZ: " << ((edge & PZ) ? 1 : 0))       

DynamicModelIndex::DynamicModelIndex(unsigned int vIndex, const std::vector<Vector3> &verts, DynamicModelIndex *next, DynamicModelIndex **base):
_verts(verts), _vIndex(vIndex), _planeFlags(-1), _edgeFlags(-1), _base(base), _next(next), _prev(NULL) {
    if (_next) { _next->_prev = this; }
}

DynamicModelIndex::~DynamicModelIndex() {
    if (this == *_base) {
        ASSERT(!_prev);
        *_base = _next;
    }

    if (_next) { _next->_prev = _prev; }
    if (_prev) { _prev->_next = _next; }
}

bool DynamicModelIndex::canAbsorb(DynamicModelIndex *other) {
#if 0
    Info("Can " << _verts[_vIndex] << " [" << edgeFlags() << "] absorb " << _verts[other->_vIndex] << " [" << other->edgeFlags() << "]?");
    bool result;

    bool a = this != other;
    bool b = (this->edgeFlags() & other->edgeFlags()) == other->edgeFlags();
    bool c = this->planeFlags() == other->planeFlags();
    result = a && b && c;

    Info("    " << a << " && " << b << " && " << c);
    Info("    " << this->edgeFlags() << " & " << other->edgeFlags() << " => " << (this->edgeFlags() & other->edgeFlags()));
    Info("    " << this->planeFlags() << " == " << other->planeFlags());
    PRINTEDGE("    lhs:", this->edgeFlags());
    PRINTEDGE("    rhs:", other->edgeFlags());
    PRINTEDGE("      &:", (this->edgeFlags() & other->edgeFlags()));

    if (result) {
        Info("    Can absorb!!!!!!");
    }

    return result;
#else
    return (this != other)
        && (this->edgeFlags() & other->edgeFlags()) == other->edgeFlags()
        && (this->planeFlags() == other->planeFlags());
#endif
}

/*! Absorbs the given index, removing collapsed faces and the old index. */
void DynamicModelIndex::absorb(DynamicModelIndex *other) {
//    Info("Absorbing " << _verts[other->vIndex()] << " into " << _verts[vIndex()]);

    // Loop over the faces of the other index for the current plane.
    FaceList::iterator itr = other->_faces.begin();
    for (; itr != other->_faces.end(); itr++) {
        // Update the face with the new index value.
        for (int j = 0; j < 3; j++) {
            if ((*itr)->getIndex(j) == other) {
                (*itr)->setIndex(j, this);
            }
        }

        // If the face has collapsed, kill it. Otherwise absorb it.
        if ((*itr)->isCollapsed()) {
            for (int j = 0; j < 3; j++) {
                (*itr)->getIndex(j)->removeFace(*itr);
            }

            delete (*itr);
        } else {
            addFace(*itr);
        }
    }

    // Kill the absorbed index.
    delete other;
}

bool DynamicModelIndex::absorbNeighbors() {
    bool keepMerging = true;
    bool merged = false;

    while (keepMerging) {
        keepMerging = false;

        FaceList::iterator itr = _faces.begin();
        for (; itr != _faces.end() && !keepMerging; itr++) {
            for (int j = 0; j < 3 && !keepMerging; j++) {
                if (canAbsorb((*itr)->getIndex(j))) {
                    absorb((*itr)->getIndex(j));
                    keepMerging = true;
                    merged = true;
                }
            }
        }
    }

    return merged;
}

void DynamicModelIndex::addFace(DynamicModelFace* face) {
    _faces.push_back(face);
}

void DynamicModelIndex::removeFace(DynamicModelFace* face) {
    _faces.remove(face);
}

unsigned int DynamicModelIndex::vIndex() {
    return _vIndex;
}

int DynamicModelIndex::planeFlags() {
    if (_planeFlags == -1) {
        calculatePlaneFlags();
    }

    return _planeFlags;
}

int DynamicModelIndex::edgeFlags() {
    if (_edgeFlags == -1) {
        calculateEdgeFlags();
    }

    return _edgeFlags;
}

DynamicModelIndex* DynamicModelIndex::next() { return _next; }
DynamicModelIndex* DynamicModelIndex::prev() { return _prev; }

void DynamicModelIndex::calculatePlaneFlags() {
    _planeFlags = 0;
    FaceList::iterator itr = _faces.begin();
    for (; itr != _faces.end(); itr++) {
        _planeFlags |= (*itr)->plane();
    }
}

void DynamicModelIndex::calculateEdgeFlags() {
    const Vector3 &base = _verts[_vIndex];
    AABB3 aabb(base, Vector3(0, 0, 0));

    FaceList::iterator itr = _faces.begin();
    for (; itr != _faces.end(); itr++) {
        for (int j = 0; j < 3; j++) {
            if ((*itr)->getIndex(j) == this) {
                continue;
            }
            
            aabb.encompass(_verts[(*itr)->getIndex(j)->vIndex()]);
        }
    }

    _edgeFlags = 0;
    Vector3 min = aabb.getMin();
    Vector3 max = aabb.getMax();
    if (Math::eq(base.x, min.x)) { _edgeFlags |= NX; }
    if (Math::eq(base.x, max.x)) { _edgeFlags |= PX; }
    if (Math::eq(base.y, min.y)) { _edgeFlags |= NY; }
    if (Math::eq(base.y, max.y)) { _edgeFlags |= PY; }
    if (Math::eq(base.z, min.z)) { _edgeFlags |= NZ; }
    if (Math::eq(base.z, max.z)) { _edgeFlags |= PZ; }

    if ((_edgeFlags & (NX | PX)) == (NX | PX)) { _edgeFlags ^= (NX | PX); }
    if ((_edgeFlags & (NY | PY)) == (NY | PY)) { _edgeFlags ^= (NY | PY); }
    if ((_edgeFlags & (NZ | PZ)) == (NZ | PZ)) { _edgeFlags ^= (NZ | PZ); }
}
