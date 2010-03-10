/*
 *  MHOctreeNode.cpp
 *  System
 *
 *  Created by loch on 3/2/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "MHOctreeNode.h"

MHOctreeNode::MHOctreeNode() {

}

MHOctreeNode::~MHOctreeNode() {}

const AABB3& MHOctreeNode::getBoundingBox() const {
    return _boundingBox;
}

