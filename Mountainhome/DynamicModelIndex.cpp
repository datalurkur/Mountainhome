/*
 *  DynamicModelIndex.cpp
 *  Mountainhome
 *
 *  Created by loch on 8/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/Plane.h>
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
#if 0
    bool result;

    bool a = this != other;
    bool b = (this->edgeFlags() & other->edgeFlags()) == other->edgeFlags();
    bool c = this->planeFlags() == other->planeFlags();
    bool d = _edgeFlags != -1;
    bool e = !this->canAbsorbWithoutFold(other);
    result = a && b && c && d && e;

    if (result) {
        Info("Absorbing: " << _verts[other->_vIndex] << " [" << other->edgeFlags() << "] ==> " << _verts[_vIndex] << " [" << edgeFlags() << "]");
        Info("    Can absorb!!!!!!");
        Info("    " << a << " && " << b << " && " << c << " && " << d << " && " <<  e);
        Info("    " << this->edgeFlags() << " & " << other->edgeFlags() << " => " << (this->edgeFlags() & other->edgeFlags()));
        Info("    " << this->planeFlags() << " == " << other->planeFlags());
        PRINTEDGE("    lhs:", this->edgeFlags());
        PRINTEDGE("    rhs:", other->edgeFlags());
        PRINTEDGE("      &:", (this->edgeFlags() & other->edgeFlags()));
    }

    return result;
#else
    return (this != other)
        && _edgeFlags != -1
        && (this->planeFlags() == other->planeFlags())
        && (this->edgeFlags() & other->edgeFlags()) == other->edgeFlags()
        && !this->canAbsorbWithoutFold(other);
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

//            Info("    Removing face:");
//            Info("        " << _verts[(*itr)->getIndex(0)->vIndex()]);
//            Info("        " << _verts[(*itr)->getIndex(1)->vIndex()]);
//            Info("        " << _verts[(*itr)->getIndex(2)->vIndex()]);

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
    return _planeFlags;
}

int DynamicModelIndex::edgeFlags() {
    return _edgeFlags;
}

DynamicModelIndex* DynamicModelIndex::next() { return _next; }
DynamicModelIndex* DynamicModelIndex::prev() { return _prev; }

void DynamicModelIndex::calculateFlags() {
    calculatePlaneFlags();
    calculateEdgeFlags();
}

void DynamicModelIndex::calculatePlaneFlags() {
    _planeFlags = 0;
    FaceList::iterator itr = _faces.begin();
    for (; itr != _faces.end(); itr++) {
        _planeFlags |= (*itr)->plane();
    }
}

#define MATCHES(flag, bits) ((flag & (bits)) == (bits))

void DynamicModelIndex::calculateEdgeFlags() {
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

    // FIXME: DEBUG CODE REMOVE ME.
    if (_verts[_vIndex] == Vector3(1, 1 , 0)) { _edgeFlags = -1; return; }

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
}


bool DynamicModelIndex::canAbsorbWithoutFold(DynamicModelIndex *other) {
    const Vector3 &starting = _verts[other->vIndex()];
    const Vector3 &ending   = _verts[this->vIndex()];

    FaceList::iterator itr = other->_faces.begin();
    for (; itr != other->_faces.end(); itr++) {
        // Find the other two indices that share the current face.
        DynamicModelIndex *one, *two;
        one = (*itr)->getIndex(0);
        if (one == other) {
            one = (*itr)->getIndex(1);
            two = (*itr)->getIndex(2);
        } else {
            two = (*itr)->getIndex(1);
            if (two == other) {
                two = (*itr)->getIndex(2);
            }
        }

        const Vector3 &otherA = _verts[one->vIndex()];
        const Vector3 &otherB = _verts[two->vIndex()];

        // Transform everything so that otherA is at the origin (simplifies plane calculations).
        Vector3 tOtherB   = (otherB) - (otherA);
        Vector3 tStarting = (starting) - (otherA);
        Vector3 tEnding   = (ending)   - (otherA);

        // Find the normal of the plane we're comparing to.
        Vector3 line = tOtherB.getNormalized();
        Vector3 normal = tStarting - (line * line.dotProduct(tStarting));
        normal.normalize();

//            Info("From: " << *starting << ", " << *otherA << ", " << *otherB);
//            Info("To:   " << *ending   << ", " << *otherA << ", " << *otherB);
//
//
//            Info("Details");
//            LogStream::IncrementIndent();
//            Info(line);
//            Info(tStarting);
//            Info(line.dotProduct(tStarting));
//            Info(line * (line.dotProduct(tStarting)));
//            Info(tStarting - (line * (line.dotProduct(tStarting))));
//            Info(normal);
//            LogStream::DecrementIndent();

        // Check to see if the new end point will cross the plane.
        if (!Plane(normal, 0).isInFrontOrOn(tEnding)) {
            return true;
        }
    }

    return false;
}
