/*
    PathFinder.h
    Mountainhome

    Created by: Andrew Jean
    Created on: July 13, 2010
    Last Modified: July 14, 2010
    Copyright 2010 Mountainhome Project. All rights reserved.
*/

#ifndef _PATHFINDER_H_
#define _PATHFINDER_H_

#include <Base/Vector.h>
#include "MHTerrain.h"
#include <stack>

struct PathNode {
    Vector3 _pos;
    float _cost;
    float _dist;
    PathNode *_parent;

    PathNode() {}
    PathNode(Vector3 pos, float cost, float dist, PathNode *parent): _pos(pos), _cost(cost), _dist(dist), _parent(parent) {}

    float score() { return _cost + _dist; }
};

extern bool findPath(Vector3 source, Vector3 dest, std::stack <Vector3> *path, MHTerrain *terrain);

#endif

