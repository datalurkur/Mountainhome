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

#define PRINTCORNER(prefix, edge) \
    Info(prefix << \
        " NNX: " << ((edge & NNX) ? 1 : 0) << \
        " NPX: " << ((edge & NPX) ? 1 : 0) << \
        " PNX: " << ((edge & PNX) ? 1 : 0) << \
        " PPX: " << ((edge & PPX) ? 1 : 0) << \
        " NXN: " << ((edge & NXN) ? 1 : 0) << \
        " NXP: " << ((edge & NXP) ? 1 : 0) << \
        " PXN: " << ((edge & PXN) ? 1 : 0) << \
        " PXP: " << ((edge & PXP) ? 1 : 0) << \
        " XNN: " << ((edge & XNN) ? 1 : 0) << \
        " XNP: " << ((edge & XNP) ? 1 : 0) << \
        " XPN: " << ((edge & XPN) ? 1 : 0) << \
        " XPP: " << ((edge & XPP) ? 1 : 0))


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
#if 1
    Info("Can " << _verts[_vIndex] << " [" << edgeFlags() << "] absorb " << _verts[other->_vIndex] << " [" << other->edgeFlags() << "]?");
    bool result;

    bool a = this != other;
    bool b = (this->edgeFlags() & other->edgeFlags()) == other->edgeFlags();
    bool c = this->planeFlags() == other->planeFlags();
    bool d = _edgeFlags != -1;
    result = a && b && c && d;

    Info("    " << a << " && " << b << " && " << c << " && " << d);
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

#define MATCHES(flag, bits) ((flag & (bits)) == (bits))

void DynamicModelIndex::calculateEdgeFlags() {
#if 0
#error
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
#elif 0
#error
    int counts[] = {0, 0, 0, 0, 0, 0};
    FaceList::iterator itr = _faces.begin();
    for (; itr != _faces.end(); itr++) {

        // Find the other two indices that share the current face.
        DynamicModelIndex *one, *two;
        one = (*itr)->getIndex(0);
        if (one == this) {
            one = (*itr)->getIndex(1);
            two = (*itr)->getIndex(2);
        } else {
            two = (*itr)->getIndex(1);
            if (two == this) {
                two = (*itr)->getIndex(2);
            }
        }

        // Create the test vector and increment the counts accordingly.
        Vector3 test =
            _verts[one->vIndex()] - _verts[this->vIndex()] +
            _verts[two->vIndex()] - _verts[this->vIndex()];

        if (test.x < 0) { counts[0]++; }
        if (test.x > 0) { counts[1]++; }
        if (test.y < 0) { counts[2]++; }
        if (test.y > 0) { counts[3]++; }
        if (test.z < 0) { counts[4]++; }
        if (test.z > 0) { counts[5]++; }
    }

    // Update the edge flags based on the counts. Anything with more than 
    _edgeFlags = 0;
    if (counts[0] < 2) { _edgeFlags |= NX; }
    if (counts[1] < 2) { _edgeFlags |= PX; }
    if (counts[2] < 2) { _edgeFlags |= NY; }
    if (counts[3] < 2) { _edgeFlags |= PY; }
    if (counts[4] < 2) { _edgeFlags |= NZ; }
    if (counts[5] < 2) { _edgeFlags |= PZ; }

    if (MATCHES(_edgeFlags, (NX | PX))) { _edgeFlags ^= (NX | PX); }
    if (MATCHES(_edgeFlags, (NY | PY))) { _edgeFlags ^= (NY | PY); }
    if (MATCHES(_edgeFlags, (NZ | PZ))) { _edgeFlags ^= (NZ | PZ); }
#elif 1
    int cornerFlags = 0;
    FaceList::iterator itr = _faces.begin();
    for (; itr != _faces.end(); itr++) {
        // Find the other two indices that share the current face.
        DynamicModelIndex *one, *two;
        one = (*itr)->getIndex(0);
        if (one == this) {
            one = (*itr)->getIndex(1);
            two = (*itr)->getIndex(2);
        } else {
            two = (*itr)->getIndex(1);
            if (two == this) {
                two = (*itr)->getIndex(2);
            }
        }

        // Create the test vector and increment the counts accordingly.
        Vector3 test =
            _verts[one->vIndex()] - _verts[this->vIndex()] +
            _verts[two->vIndex()] - _verts[this->vIndex()];

        if ((*itr)->plane() == DynamicModel::XY) {
            if (test.x < 0 && test.y < 0) { cornerFlags |= NNX; }
            if (test.x < 0 && test.y > 0) { cornerFlags |= NPX; }
            if (test.x > 0 && test.y < 0) { cornerFlags |= PNX; }
            if (test.x > 0 && test.y > 0) { cornerFlags |= PPX; }
        } else if ((*itr)->plane() == DynamicModel::XZ) {
            if (test.x < 0 && test.z < 0) { cornerFlags |= NXN; }
            if (test.x < 0 && test.z > 0) { cornerFlags |= NXP; }
            if (test.x > 0 && test.z < 0) { cornerFlags |= PXN; }
            if (test.x > 0 && test.z > 0) { cornerFlags |= PXP; }
        } else {
            if (test.y < 0 && test.z < 0) { cornerFlags |= XNN; }
            if (test.y < 0 && test.z > 0) { cornerFlags |= XNP; }
            if (test.y > 0 && test.z < 0) { cornerFlags |= XPN; }
            if (test.y > 0 && test.z > 0) { cornerFlags |= XPP; }
        }
    }

    const int XY_MASK = (NNX | NPX | PNX | PPX);
    const int XZ_MASK = (NXN | NXP | PXN | PXP);
    const int YZ_MASK = (XNN | XNP | XPN | XPP);
//    const int X_MASK  = (XY_MASK | XZ_MASK);
//    const int Y_MASK  = (XY_MASK | YZ_MASK);
//    const int Z_MASK  = (XZ_MASK | YZ_MASK);

    const int NX_MASK = (NNX | NPX | NXN | NXP);
    const int PX_MASK = (PNX | PPX | PXN | PXP);
    const int NY_MASK = (NNX | PNX | XNN | XNP);
    const int PY_MASK = (NPX | PPX | XPN | XPP);
//    const int NZ_MASK = (XNN | XPN | NXN | PXN);
//    const int PZ_MASK = (XNP | XPP | NXP | PXP);

#define SET_EDGE_FLAGS(cpf, nx, px, ny, py) \
    do { \
        if ((cpf & NX_MASK) == 0) { _edgeFlags |= nx; } \
        if ((cpf & PX_MASK) == 0) { _edgeFlags |= px; } \
        if ((cpf & NY_MASK) == 0) { _edgeFlags |= ny; } \
        if ((cpf & PY_MASK) == 0) { _edgeFlags |= py; } \
        if ((cpf & NNX) == 0 && MATCHES(cpf, (NPX | PNX))) { _edgeFlags |= (nx | ny); _edgeFlags = -1; } \
        if ((cpf & NPX) == 0 && MATCHES(cpf, (NNX | PPX))) { _edgeFlags |= (nx | py); _edgeFlags = -1; } \
        if ((cpf & PNX) == 0 && MATCHES(cpf, (NNX | PPX))) { _edgeFlags |= (px | ny); _edgeFlags = -1; } \
        if ((cpf & PPX) == 0 && MATCHES(cpf, (NPX | PNX))) { _edgeFlags |= (px | py); _edgeFlags = -1; } \
    } while(0)

    // Update the edge flags based on the corner flags.
    _edgeFlags = 0;

    int cornerPlaneFlags = 0;
    if (cornerPlaneFlags = (cornerFlags & XY_MASK)) {
        cornerPlaneFlags = cornerPlaneFlags >> 0;
        SET_EDGE_FLAGS(cornerPlaneFlags, NX, PX, NY, PY);
    }

    if (cornerPlaneFlags = (cornerFlags & XZ_MASK)) {
        cornerPlaneFlags = cornerPlaneFlags >> 4;
        SET_EDGE_FLAGS(cornerPlaneFlags, NX, PX, NZ, PZ);
    }

    if (cornerPlaneFlags = (cornerFlags & YZ_MASK)) {
        cornerPlaneFlags = cornerPlaneFlags >> 8;
        SET_EDGE_FLAGS(cornerPlaneFlags, NY, PY, NZ, PZ);
    }

#else
#error
    _edgeFlags = 0;
    FaceList::iterator itr = _faces.begin();
    for (; itr != _faces.end(); itr++) {
        // Find the other two indices that share the current face.
        DynamicModelIndex *one, *two;
        one = (*itr)->getIndex(0);
        if (one == this) {
            one = (*itr)->getIndex(1);
            two = (*itr)->getIndex(2);
        } else {
            two = (*itr)->getIndex(1);
            if (two == this) {
                two = (*itr)->getIndex(2);
            }
        }

        // Create the test vector and increment the counts accordingly.
        Vector3 test =
            _verts[one->vIndex()] - _verts[this->vIndex()] +
            _verts[two->vIndex()] - _verts[this->vIndex()];

        if ((*itr)->plane() == DynamicModel::XY) {
            if (test.x < 0 && test.y < 0) { _edgeFlags |= NNX; }
            if (test.x < 0 && test.y > 0) { _edgeFlags |= NPX; }
            if (test.x > 0 && test.y < 0) { _edgeFlags |= PNX; }
            if (test.x > 0 && test.y > 0) { _edgeFlags |= PPX; }
        } else if ((*itr)->plane() == DynamicModel::XY) {
            if (test.x < 0 && test.z < 0) { _edgeFlags |= NXN; }
            if (test.x < 0 && test.z > 0) { _edgeFlags |= NXP; }
            if (test.x > 0 && test.z < 0) { _edgeFlags |= PXN; }
            if (test.x > 0 && test.z > 0) { _edgeFlags |= PXP; }
        } else {
            if (test.y < 0 && test.z < 0) { _edgeFlags |= XNN; }
            if (test.y < 0 && test.z > 0) { _edgeFlags |= XNP; }
            if (test.y > 0 && test.z < 0) { _edgeFlags |= XPN; }
            if (test.y > 0 && test.z > 0) { _edgeFlags |= XPP; }
        }
    }
#endif
}
