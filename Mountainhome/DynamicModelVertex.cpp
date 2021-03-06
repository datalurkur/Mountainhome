/*
 *  DynamicModelVertex.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

//#include <Base/Normal.h>
#include <Base/AABB.h>

#include "DynamicModelVertex.h"
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


DynamicModelVertex::DynamicModelVertex(
    unsigned int index,
    const std::vector<Vector3> &verts,
    int normal,
    DynamicModelVertex **base
):
    _verts(verts),
    _index(index),
    _edgeFlags(-1),
    _normal(normal),
    _base(base),
    _next(*base),
    _prev(NULL)
{
    if (_next) { _next->_prev = this; }
    *_base = this;
}

DynamicModelVertex::~DynamicModelVertex() {
    if (this == *_base) {
        ASSERT(!_prev);
        *_base = _next;
    }

    if (_next) { _next->_prev = _prev; }
    if (_prev) { _prev->_next = _next; }
}

bool DynamicModelVertex::canAbsorb(DynamicModelVertex *other) {
#if 0
    bool result;

    bool a = this != other;
    bool b = (this->edgeFlags() & other->edgeFlags()) == other->edgeFlags();
    bool c = _edgeFlags != -1;
    bool d = this->canAbsorbWithoutFold(other);
    result = a && b && c && d;

    if (result) {
        Info("Absorbing: " << _verts[other->_index] << " [" << other->edgeFlags() << "] ==> " << _verts[_index] << " [" << edgeFlags() << "]");
        Info("    " << a << " && " << b << " && " << c << " && " << d);
        Info("    " << this->edgeFlags() << " & " << other->edgeFlags() << " => " << (this->edgeFlags() & other->edgeFlags()));
        PRINTEDGE("    lhs:", this->edgeFlags());
        PRINTEDGE("    rhs:", other->edgeFlags());
        PRINTEDGE("      &:", (this->edgeFlags() & other->edgeFlags()));
    }

    return result;
#else
    return (this != other)
        && _edgeFlags != -1
        && (this->getEdgeFlags() & other->getEdgeFlags()) == other->getEdgeFlags()
        && this->canAbsorbWithoutFold(other);
#endif
}

void DynamicModelVertex::absorb(DynamicModelVertex *other) {
    // Info("Absorbing " << _verts[other->getIndex()] << " into " << _verts[getIndex()]);

    // Loop over the faces of the other index for the current normal.
    FaceList::iterator itr = other->_faces.begin();
    for (; itr != other->_faces.end(); itr++) {
        if (!(*itr)->replaceVertex(other, this)) {
            delete (*itr);
        }
    }

    // Kill the absorbed index.
    delete other;
}

bool DynamicModelVertex::absorbNeighbors() {
    bool keepMerging = true;
    bool merged = false;

    while (keepMerging) {
        keepMerging = false;

        FaceList::iterator itr = _faces.begin();
        for (; itr != _faces.end() && !keepMerging; itr++) {
            for (int j = 0; j < 3 && !keepMerging; j++) {
                if (canAbsorb((*itr)->getVertex(j))) {
                    absorb((*itr)->getVertex(j));
                    keepMerging = true;
                    merged = true;
                }
            }
        }
    }

    return merged;
}

void DynamicModelVertex::addFace(DynamicModelFace* face) {
    _faces.push_back(face);
}

void DynamicModelVertex::removeFace(DynamicModelFace* face) {
    _faces.remove(face);
}

unsigned int DynamicModelVertex::getIndex() {
    return _index;
}

void DynamicModelVertex::setIndex(unsigned int newIndex) {
    _index = newIndex;
}

int DynamicModelVertex::getNormal() {
    return _normal;
}

int DynamicModelVertex::getEdgeFlags() {
    return _edgeFlags;
}

int DynamicModelVertex::getFaceCount() {
    return _faces.size();
}

DynamicModelVertex* DynamicModelVertex::next() { return _next; }
DynamicModelVertex* DynamicModelVertex::prev() { return _prev; }

#define MATCHES(flag, bits) ((flag & (bits)) == (bits))

void DynamicModelVertex::calculateEdgeFlags() {
    int cornerFlags = 0;
    FaceList::iterator itr = _faces.begin();
    for (; itr != _faces.end(); itr++) {
        // Find the other two indices that share the current face.
        DynamicModelVertex *one, *two;
        one = (*itr)->getVertex(0);
        if (one == this) {
            one = (*itr)->getVertex(1);
            two = (*itr)->getVertex(2);
        } else {
            two = (*itr)->getVertex(1);
            if (two == this) {
                two = (*itr)->getVertex(2);
            }
        }

        // Create the test vector and increment the counts accordingly.
        Vector3 test =
            _verts[one->getIndex()] - _verts[this->getIndex()] +
            _verts[two->getIndex()] - _verts[this->getIndex()];

        if ((*itr)->getNormal() == DynamicModel::XY_POS ||
            (*itr)->getNormal() == DynamicModel::XY_NEG) {
            if (test.x < 0 && test.y < 0) { cornerFlags |= NNX; }
            if (test.x < 0 && test.y > 0) { cornerFlags |= NPX; }
            if (test.x > 0 && test.y < 0) { cornerFlags |= PNX; }
            if (test.x > 0 && test.y > 0) { cornerFlags |= PPX; }
        } else if ((*itr)->getNormal() == DynamicModel::XZ_POS ||
                   (*itr)->getNormal() == DynamicModel::XZ_NEG) {
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

    const int NX_MASK = (NNX | NPX | NXN | NXP);
    const int PX_MASK = (PNX | PPX | PXN | PXP);
    const int NY_MASK = (NNX | PNX | XNN | XNP);
    const int PY_MASK = (NPX | PPX | XPN | XPP);

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

    int cornerNormalFlags = 0;
    if ((cornerNormalFlags = (cornerFlags & XY_MASK))) {
        cornerNormalFlags = cornerNormalFlags >> 0;
        SET_EDGE_FLAGS(cornerNormalFlags, NX, PX, NY, PY);
    }

    if ((cornerNormalFlags = (cornerFlags & XZ_MASK))) {
        cornerNormalFlags = cornerNormalFlags >> 4;
        SET_EDGE_FLAGS(cornerNormalFlags, NX, PX, NZ, PZ);
    }

    if ((cornerNormalFlags = (cornerFlags & YZ_MASK))) {
        cornerNormalFlags = cornerNormalFlags >> 8;
        SET_EDGE_FLAGS(cornerNormalFlags, NY, PY, NZ, PZ);
    }
}

bool DynamicModelVertex::canAbsorbWithoutFold(DynamicModelVertex *other) {
    const Vector3 &starting = _verts[other->getIndex()];
    const Vector3 &ending   = _verts[this->getIndex()];

    FaceList::iterator itr = other->_faces.begin();
    for (; itr != other->_faces.end(); itr++) {
        // Only consider faces that won't be be collapsed if 'this' absorbs 'other' (they
        // will always fail this test).
        if ((*itr)->hasVertex(this)) { continue; }

        // Find the other two indices that share the current face.
        DynamicModelVertex *one, *two;
        one = (*itr)->getVertex(0);
        if (one == other) {
            one = (*itr)->getVertex(1);
            two = (*itr)->getVertex(2);
        } else {
            two = (*itr)->getVertex(1);
            if (two == other) {
                two = (*itr)->getVertex(2);
            }
        }

        // A and B are the other vertex locations in the face.
        const Vector3 &otherA = _verts[one->getIndex()];
        const Vector3 &otherB = _verts[two->getIndex()];

        // Transform everything so that otherA is at the origin (simplifies normal calculations).
        Vector3 toOtherB   = (otherB) - (otherA);
        Vector3 toStarting = (starting) - (otherA);
        Vector3 toEnding   = (ending)   - (otherA);

        // Find the normal of the normal defined by the points A and B. The ambiguity of
        // the 2 point normal definition is clear, not by having 'starting' be in the
        // normal, but rather having the normal point directly at it.
        Vector3 line = toOtherB.getNormalized();
        Vector3 normal = toStarting - (line * line.dotProduct(toStarting));
        normal.normalize();

        // Check to see if the new end point will cross or land on the A/B normal. Both of
        // these cases are invalid. Remember, the ONLY time a face should EVER collapse is
        // if v absorbs u and u and v are both vertices in the collapsed face. Any other
        // time will break the fold prevention algorithm.
        if (Math::le(toEnding.dotProduct(normal), 0.0)) {
            return false;
        }
    }

    return true;
}
