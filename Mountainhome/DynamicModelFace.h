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

class DynamicModelIndex;
class DynamicModelFace {
public:
    /*! Creates a new dynamic model face based on the three given indices and aligned
     *  along the specified plane. It also sets up the new face to join the built in
     *  linked list. */
    DynamicModelFace(
        DynamicModelIndex *one,
        DynamicModelIndex *two,
        DynamicModelIndex *three,
        int plane,
        DynamicModelFace **base);

    /*! Kills the face and patches the face list. */
    ~DynamicModelFace();

    DynamicModelIndex* getIndex(int i);

    /*! Replaces the given oldIndex with the given newIndex. If the operation collapses
     *  the face, it will orphan itself from its indices and return false, otherwise it
     *  with update oldIndex and newIndex's face list and return true.
     * \param oldIndex The index to replace.
     * \param newIndex The index to replace oldIndex with.
     * \return Whether or not the face is still valid. If not, it should be deleted. */
    bool replaceIndex(DynamicModelIndex *oldIndex, DynamicModelIndex *newIndex);

    /*! Returns whether or not this face has the given index. */
    bool hasIndex(DynamicModelIndex *lhs);

    int plane();

    DynamicModelFace* next();
    DynamicModelFace* prev();

protected:
    /*! Has the face remove itself from each of its indices. */
    void dettachFromIndices();

    /*! Reports whether or not this face has collapsed in on itself. A collapsed face
     *  contains two or more identical indices and will render as a line. */
    bool isCollapsed();

private:
    int _plane;
    DynamicModelIndex *_indices[3];

    DynamicModelFace **_base;
    DynamicModelFace *_next;
    DynamicModelFace *_prev;
};


#endif
