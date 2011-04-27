/*
 *  PathManagerBindings.h
 *  Mountainhome
 *
 *  Created by datalurkur on 4/26/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _PATHMANAGERBINDINGS_H_
#define _PATHMANAGERBINDINGS_H_
#include "RubyBindings.h"
#include "PathManager.h"

class PathManagerBindings : public RubyBindings<PathManager, false> {
public:
    static VALUE SetTilePathable(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE SetTileOpen(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE SetTileClosed(VALUE self, VALUE x, VALUE y, VALUE z);

    static VALUE GetPath(VALUE self, VALUE sX, VALUE sY, VALUE sZ, VALUE dX, VALUE dY, VALUE dZ);
    static VALUE GetFirstPath(VALUE self, VALUE sX, VALUE sY, VALUE sZ, VALUE destinations);
    static VALUE GetShortestPath(VALUE self, VALUE sX, VALUE sY, VALUE sZ, VALUE destinations);

public:
    PathManagerBindings();
    virtual ~PathManagerBindings() {}
};

#endif
