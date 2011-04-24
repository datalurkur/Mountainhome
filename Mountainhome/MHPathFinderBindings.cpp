/*
 *  MHPathFinderBindings.cpp
 *  Mountainhome
 *
 *  Created by datalurkur on 12/28/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHPathFinderBindings.h"
#include <Base/Vector.h>

MHPathFinderBindings::MHPathFinderBindings()
: RubyBindings<MHPathFinder, false>(
    rb_define_class("MHPathFinder", rb_cObject),
    "MHPathFinderBindings")
{
    rb_define_method(_class, "block_tile", RUBY_METHOD_FUNC(MHPathFinderBindings::BlockTile), 3);
    rb_define_method(_class, "unblock_tile", RUBY_METHOD_FUNC(MHPathFinderBindings::UnblockTile), 3);

	rb_define_method(_class, "block_z_range", RUBY_METHOD_FUNC(MHPathFinderBindings::BlockZRange), 4);
	rb_define_method(_class, "unblock_z_range", RUBY_METHOD_FUNC(MHPathFinderBindings::UnblockZRange), 4);

    rb_define_method(_class, "set_start_position", RUBY_METHOD_FUNC(MHPathFinderBindings::SetStartPosition), 3);
    rb_define_method(_class, "get_path_to", RUBY_METHOD_FUNC(MHPathFinderBindings::GetPathTo), 3);
    rb_define_method(_class, "closest_path_to", RUBY_METHOD_FUNC(MHPathFinderBindings::GetClosestPath), 1);
    rb_define_method(_class, "first_path_to", RUBY_METHOD_FUNC(MHPathFinderBindings::FirstPathTo), 1);
    rb_define_method(_class, "blocked_path_to?", RUBY_METHOD_FUNC(MHPathFinderBindings::IsPathBlocked), 3);
}

VALUE MHPathFinderBindings::BlockTile(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    MHPathFinder *cSelf = Get()->getPointer(rSelf);
    cSelf->tileBlocked(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ));
    return rSelf;    
}

VALUE MHPathFinderBindings::UnblockTile(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    MHPathFinder *cSelf = Get()->getPointer(rSelf);
    cSelf->tileUnblocked(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ));
    return rSelf;    
}

VALUE MHPathFinderBindings::BlockZRange(VALUE rSelf, VALUE rX, VALUE rY, VALUE rS_z, VALUE rE_z) {
    MHPathFinder *cSelf = Get()->getPointer(rSelf);
    cSelf->zRangeBlocked(NUM2INT(rX), NUM2INT(rY), NUM2INT(rS_z), NUM2INT(rE_z));
    return rSelf;
}

VALUE MHPathFinderBindings::UnblockZRange(VALUE rSelf, VALUE rX, VALUE rY, VALUE rS_z, VALUE rE_z) {
    MHPathFinder *cSelf = Get()->getPointer(rSelf);
    cSelf->zRangeUnblocked(NUM2INT(rX), NUM2INT(rY), NUM2INT(rS_z), NUM2INT(rE_z));
    return rSelf;
}

VALUE MHPathFinderBindings::SetStartPosition(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    MHPathFinder *cSelf = Get()->getPointer(rSelf);
    cSelf->setStartPosition(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ));
    return rSelf;
}

VALUE MHPathFinderBindings::GetPathTo(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    MHPathFinder *cSelf = Get()->getPointer(rSelf);
    std::stack<Vector3> path;
    cSelf->getPathTo(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ), path);
    while(!path.empty()) {
        Vector3 thisNode = path.top();
        rb_yield(rb_ary_new3(3, INT2NUM(thisNode[0]), INT2NUM(thisNode[1]), INT2NUM(thisNode[2])));
        path.pop();
    }
    return rSelf;
}

VALUE MHPathFinderBindings::IsPathBlocked(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    MHPathFinder *cSelf = Get()->getPointer(rSelf);
    return cSelf->isPathBlocked(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ)) ? Qtrue : Qfalse;
}

VALUE MHPathFinderBindings::GetClosestPath(VALUE rSelf, VALUE rArray) {
    MHPathFinder *cSelf = Get()->getPointer(rSelf);
    std::stack<Vector3> destinations;
    std::stack<Vector3> path;

    VALUE thisPosition;
    while((thisPosition = rb_ary_shift(rArray)) != Qnil) {
        int x = NUM2INT(rb_ary_entry(thisPosition, 0)),
            y = NUM2INT(rb_ary_entry(thisPosition, 1)),
            z = NUM2INT(rb_ary_entry(thisPosition, 2));

        destinations.push(Vector3(x, y, z));
    }

    cSelf->getClosestPath(destinations, path);
    while(!path.empty()) {
        Vector3 thisNode = path.top();
        rb_yield(rb_ary_new3(3, INT2NUM(thisNode[0]), INT2NUM(thisNode[1]), INT2NUM(thisNode[2])));
        path.pop();    
    }
    return rSelf;
}

// Essentially the identical bindings to GetClosestPath. Macro?
VALUE MHPathFinderBindings::FirstPathTo(VALUE rSelf, VALUE rArray) {
    MHPathFinder *cSelf = Get()->getPointer(rSelf);
    std::stack<Vector3> destinations;
    std::stack<Vector3> path;

    VALUE thisPosition;
    while((thisPosition = rb_ary_shift(rArray)) != Qnil) {
        int x = NUM2INT(rb_ary_entry(thisPosition, 0)),
            y = NUM2INT(rb_ary_entry(thisPosition, 1)),
            z = NUM2INT(rb_ary_entry(thisPosition, 2));

        destinations.push(Vector3(x, y, z));
    }

    cSelf->getFirstPath(destinations, path);
    while(!path.empty()) {
        Vector3 thisNode = path.top();
        rb_yield(rb_ary_new3(3, INT2NUM(thisNode[0]), INT2NUM(thisNode[1]), INT2NUM(thisNode[2])));
        path.pop();
    }
    return rSelf;
}

