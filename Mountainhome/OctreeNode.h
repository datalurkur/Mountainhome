/*
 *  OctreeNode.h
 *  System
 *
 *  Created by loch on 3/2/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _OCTREENODE_H_
#define _OCTREENODE_H_
#include <Base/AABB.h>

class OctreeNode {
public:
    OctreeNode();
    virtual ~OctreeNode();

    const AABB3& getBoundingBox() const;

private:
    OctreeNode *_children[2][2][2];
    AABB3 _boundingBox;
    bool _leaf;

};

#endif
