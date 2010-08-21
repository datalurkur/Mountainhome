/*
 *  DynamicModelVertex.h
 *  Mountainhome
 *
 *  Created by loch on 8/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _DYNAMICMODELVERTEX_H_
#define _DYNAMICMODELVERTEX_H_
#include <Base/Vector.h>
#include <list>

class DynamicModel;
class DynamicModelFace;
class DynamicModelVertex {
public:
    DynamicModelVertex(unsigned int index, const std::vector<Vector3> &verts, DynamicModelVertex **base);
    ~DynamicModelVertex();

    bool absorbNeighbors();

    void addFace(DynamicModelFace* face);

    void removeFace(DynamicModelFace* face);

    DynamicModelVertex* next();

    DynamicModelVertex* prev();

    void calculateFlags();

    unsigned int index();

private:

    int planeFlags();

    int edgeFlags();

    bool canAbsorb(DynamicModelVertex *other);

    /*! Absorbs the given vertex, removing collapsed faces and the old vertex. */
    void absorb(DynamicModelVertex *other);

    bool canAbsorbWithoutFold(DynamicModelVertex *other);

    void calculatePlaneFlags();
    void calculateEdgeFlags();

private:
    typedef std::list<DynamicModelVertex*> VertexList;
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

    // VertexList _neighbors;
    FaceList _faces;

    unsigned int _index;
    int _planeFlags;
    int _edgeFlags;

    DynamicModelVertex **_base;
    DynamicModelVertex *_next;
    DynamicModelVertex *_prev;

};

#endif
