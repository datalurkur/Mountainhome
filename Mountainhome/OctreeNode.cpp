/*
 *  OctreeNode.cpp
 *  System
 *
 *  Created by loch on 3/2/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "OctreeNode.h"

OctreeNode::OctreeNode() {}

OctreeNode::~OctreeNode() {}

const AABB3& OctreeNode::getBoundingBox() const {
    return _boundingBox;
}
