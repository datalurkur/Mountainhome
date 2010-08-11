/*
 *  DynamicModelIndex.h
 *  Mountainhome
 *
 *  Created by loch on 8/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _DYNAMICMODELINDEX_H_
#define _DYNAMICMODELINDEX_H_
#include <Base/Vector.h>
#include <list>

class DynamicModel;
class DynamicModelFace;
class DynamicModelIndex {
public:
    DynamicModelIndex(unsigned int vIndex, const std::vector<Vector3> &verts, DynamicModelIndex *next, DynamicModelIndex **base);
    ~DynamicModelIndex();

    bool canAbsorb(DynamicModelIndex *other);

    /*! Absorbs the given index, removing collapsed faces and the old index. */
    void absorb(DynamicModelIndex *other);

    bool absorbNeighbors();

    void addFace(DynamicModelFace* face);

    void removeFace(DynamicModelFace* face);

    unsigned int vIndex();

    int planeFlags();

    int edgeFlags();

    DynamicModelIndex* next();

    DynamicModelIndex* prev();

private:
    void calculatePlaneFlags();
    void calculateEdgeFlags();

private:
    typedef std::list<DynamicModelFace*> FaceList;

    enum CornerFlags {
        NNX = 1 << 0,
        NPX = 1 << 1,
        PNX = 1 << 2,
        PPX = 1 << 3,

        NXN = 1 << 4,
        NXP = 1 << 5,
        PXN = 1 << 6,
        PXP = 1 << 7,

        XNN = 1 << 8,
        XNP = 1 << 9,
        XPN = 1 << 10,
        XPP = 1 << 11
    };

    enum EdgeFlags {
        NX = 1 << 0,
        PX = 1 << 1,
        NY = 1 << 2,
        PY = 1 << 3,
        NZ = 1 << 4,
        PZ = 1 << 5,
    };

private:
    const std::vector<Vector3> &_verts;

    FaceList _faces;

    unsigned int _vIndex;
    int _planeFlags;
    int _edgeFlags;

    DynamicModelIndex **_base;
    DynamicModelIndex *_next;
    DynamicModelIndex *_prev;

};

#endif
