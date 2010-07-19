/*
    MHPath.h
    Mountainhome

    Created by Andrew Jean on 7/16/2010
    Last Modified on 7/16/2010

    Copyright 2010 Mountainhome Project. All rights reserved.
*/

#ifndef _MHPATH_H_
#define _MHPATH_H_

#include <Base/Vector.h>
#include <stack>

#include "RubyBindings.h"

class MHWorld;
class MHTerrain;

class MHPath : public RubyBindings<MHPath, true> {
public:
    // Ruby bindings and binding setup
    static void SetupBindings();

    static VALUE Initialize(VALUE rSelf, VALUE rWorld, VALUE rSX, VALUE rSY, VALUE rSZ, VALUE rDX, VALUE rDY, VALUE rDZ);
    static VALUE NextStep(VALUE self);
    static VALUE Blocked(VALUE self);
    static VALUE EndOfPath(VALUE self);

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

private:
    bool accessible;
    std::stack <Vector3> path;

};

#endif
