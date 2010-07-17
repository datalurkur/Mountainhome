/*
    Path.h
    Mountainhome

    Created by Andrew Jean on 7/16/2010
    Last Modified on 7/16/2010

    Copyright 2010 Mountainhome Project. All rights reserved.
*/

#ifndef _PATH_H_
#define _PATH_H_

#include <Base/Vector.h>
#include "PathFinder.h"
#include <stack>

class MHWorld;

class Path {
    std::stack <Vector3> path;

public:
    Path(Vector3 source, Vector3 dest, MHWorld *world);
    ~Path();

    bool endOfPath();
    Vector3 getNextStep();
};

#endif
