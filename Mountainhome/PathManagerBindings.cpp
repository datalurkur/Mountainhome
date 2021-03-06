/*
 *  PathManagerBindings.cpp
 *  Mountainhome
 *
 *  Created by datalurkur on 4/26/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "PathManagerBindings.h"
#include <Base/Vector.h>

PathManagerBindings::PathManagerBindings()
: RubyBindings<PathManager, false>(
    rb_define_class("PathManager", rb_cObject),
    "PathManagerBindings")
{
    rb_define_method(_class, "set_voxel_pathable", RUBY_METHOD_FUNC(PathManagerBindings::SetVoxelPathable), 3);
    rb_define_method(_class, "set_voxel_open", RUBY_METHOD_FUNC(PathManagerBindings::SetVoxelOpen), 3);
    rb_define_method(_class, "set_voxel_closed", RUBY_METHOD_FUNC(PathManagerBindings::SetVoxelClosed), 3);

    rb_define_method(_class, "get_path", RUBY_METHOD_FUNC(PathManagerBindings::GetPath), 6);
    rb_define_method(_class, "get_first_path", RUBY_METHOD_FUNC(PathManagerBindings::GetFirstPath), 4);
    rb_define_method(_class, "get_shortest_path", RUBY_METHOD_FUNC(PathManagerBindings::GetShortestPath), 4);
}

VALUE PathManagerBindings::SetVoxelPathable(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    PathManager *cSelf = Get()->getPointer(rSelf);
    cSelf->setNodeType(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ), PATHABLE);
    return rSelf;
}

VALUE PathManagerBindings::SetVoxelOpen(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    PathManager *cSelf = Get()->getPointer(rSelf);
    cSelf->setNodeType(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ), OPEN);
    return rSelf;
}

VALUE PathManagerBindings::SetVoxelClosed(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    PathManager *cSelf = Get()->getPointer(rSelf);
    cSelf->setNodeType(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ), CLOSED);
    return rSelf;
}

VALUE PathManagerBindings::GetPath(VALUE rSelf, VALUE sX, VALUE sY, VALUE sZ, VALUE dX, VALUE dY, VALUE dZ) {
    PathManager *cSelf = Get()->getPointer(rSelf);

    Path cPath;
    cSelf->getPath(Vector3(NUM2INT(sX),NUM2INT(sY),NUM2INT(sZ)), Vector3(NUM2INT(dX),NUM2INT(dY),NUM2INT(dZ)), cPath);

    VALUE rPath = rb_ary_new();
    Path::iterator itr = cPath.begin();
    for(; itr != cPath.end(); itr++) {
        rb_ary_push(rPath, rb_ary_new3(3, INT2NUM((*itr).x), INT2NUM((*itr).y), INT2NUM((*itr).z)));
    }
    return rPath;
}

VALUE PathManagerBindings::GetFirstPath(VALUE rSelf, VALUE sX, VALUE sY, VALUE sZ, VALUE destinations) {
    PathManager *cSelf = Get()->getPointer(rSelf);
    Vector3 start(NUM2INT(sX), NUM2INT(sY), NUM2INT(sZ));

    Path path;
    for(int c=0; c < RARRAY_LEN(destinations); c++) {
        VALUE nextDest = rb_ary_entry(destinations, c);
        Vector3 end(NUM2INT(rb_ary_entry(nextDest,0)), NUM2INT(rb_ary_entry(nextDest,1)), NUM2INT(rb_ary_entry(nextDest,2)));

        int distance = cSelf->getPath(start, end, path);
        if(distance != PATH_NOT_FOUND) { break; }
    }

    VALUE rPath = rb_ary_new();
    Path::iterator itr = path.begin();
    for(; itr != path.end(); itr++) {
        rb_ary_push(rPath, rb_ary_new3(3, INT2NUM((*itr).x), INT2NUM((*itr).y), INT2NUM((*itr).z)));
    }
    return rPath;
}

VALUE PathManagerBindings::GetShortestPath(VALUE rSelf, VALUE sX, VALUE sY, VALUE sZ, VALUE destinations) {
    PathManager *cSelf = Get()->getPointer(rSelf);
    Vector3 start(NUM2INT(sX), NUM2INT(sY), NUM2INT(sZ));

    Path shortestPath;
    int shortestDistance = -1;

    for(int c=0; c < RARRAY_LEN(destinations); c++) {
        VALUE nextDest = rb_ary_entry(destinations, c);
        Vector3 end(NUM2INT(rb_ary_entry(nextDest,0)), NUM2INT(rb_ary_entry(nextDest,1)), NUM2INT(rb_ary_entry(nextDest,2)));

        Path path;
        int distance = cSelf->getPath(start, end, path);
        if(distance != -1 && (shortestDistance == -1 || distance < shortestDistance)) {
            shortestPath = path;
            shortestDistance = distance;
        }
    }

    VALUE rPath = rb_ary_new();
    Path::iterator itr = shortestPath.begin();
    for(; itr != shortestPath.end(); itr++) {
        rb_ary_push(rPath, rb_ary_new3(3, INT2NUM((*itr).x), INT2NUM((*itr).y), INT2NUM((*itr).z)));
    }
    return rPath;
}
