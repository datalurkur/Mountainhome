/*
 *  DynamicModelFace.h
 *  Mountainhome
 *
 *  Created by loch on 8/5/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _DYNAMICMODELFACE_H_
#define _DYNAMICMODELFACE_H_

class DynamicModelVertex;
class DynamicModelFace {
public:
    /*! Creates a new dynamic model face based on the three given indices and aligned
     *  along the specified plane. It also sets up the new face to join the built in
     *  linked list. */
    DynamicModelFace(
        DynamicModelVertex *one,
        DynamicModelVertex *two,
        DynamicModelVertex *three,
        int plane,
        DynamicModelFace **base);

    /*! Kills the face and patches the face list. */
    ~DynamicModelFace();

    DynamicModelVertex* getVertex(int i);

    /*! Replaces the given oldVertex with the given newVertex. If the operation collapses
     *  the face, it will orphan itself from its indices and return false, otherwise it
     *  with update oldVertex and newVertex's face list and return true.
     * \param oldVertex The vertex to replace.
     * \param newVertex The vertex to replace oldVertex with.
     * \return Whether or not the face is still valid. If not, it should be deleted. */
    bool replaceVertex(DynamicModelVertex *oldVertex, DynamicModelVertex *newVertex);

    /*! Returns whether or not this face has the given vertex. */
    bool hasVertex(DynamicModelVertex *lhs);

    int getPlane();

    DynamicModelFace* next();
    DynamicModelFace* prev();

protected:
    /*! Has the face remove itself from each of its indices. */
    void dettachFromIndices();

    /*! Reports whether or not this face has collapsed in on itself. A collapsed face
     *  contains two or more identical indices and will render as a line. */
    bool isCollapsed();

private:
    DynamicModelVertex *_indices[3];

    int _plane;

    DynamicModelFace **_base;
    DynamicModelFace *_next;
    DynamicModelFace *_prev;
};


#endif
