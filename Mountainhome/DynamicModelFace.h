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
    DynamicModelFace(DynamicModelIndex *one, DynamicModelIndex *two, DynamicModelIndex *three, int plane, DynamicModelFace *next, DynamicModelFace **base);

    /*! Kills the face and patches the face list. */
    ~DynamicModelFace();

    /*! Reports whether or not this face has collapsed in on itself. A collapsed face
     *  contains two or more identical indices and will render as a line. */
    bool isCollapsed();

    DynamicModelIndex* getIndex(int i);
    void setIndex(int i, DynamicModelIndex *index);

    int plane();

    DynamicModelFace* next();
    DynamicModelFace* prev();

private:
    int _plane;
    DynamicModelIndex *_indices[3];

    DynamicModelFace **_base;
    DynamicModelFace *_next;
    DynamicModelFace *_prev;
};


#endif
