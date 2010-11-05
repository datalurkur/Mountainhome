/*
 *  MHPath.h
 *  Mountainhome
 *
 *  Created by datalurkur on 7/16/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHPATH_H_
#define _MHPATH_H_

#include <Base/Vector.h>
#include <stack>

class MHWorld;
class MHTerrain;

class MHPath {
public:
    static bool FindPath(const Vector3 &source, const Vector3 &dest, std::stack <Vector3> *path, MHTerrain *terrain);
    static void GetTraversibleNeighbors(MHTerrain *terrain, const Vector3 &loc, std::stack <Vector3> *neighbors);

public:
    // C-side functions
    MHPath();
    ~MHPath();

    void initialize(MHWorld *world, Vector3 source, Vector3 dest);
    bool blocked();
    bool endOfPath();
    Vector3 getNextStep();

    bool isEmpty() { return path.empty(); }

private:
    bool accessible;
    std::stack <Vector3> path;

};

#endif
