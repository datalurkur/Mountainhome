/*
 *  MHOctreeNode.h
 *  System
 *
 *  Created by loch on 3/2/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MHOCTREENODE_H_
#define _MHOCTREENODE_H_
#include <Base/AABB.h>

class MHOctreeNode {
public:
    MHOctreeNode();
    virtual ~MHOctreeNode();

    const AABB3& getBoundingBox() const;

private:
    MHOctreeNode *_children[2][2][2];
    AABB3 _boundingBox;
    bool _leaf;

};

#endif
