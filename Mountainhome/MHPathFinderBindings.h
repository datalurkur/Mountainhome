/*
 *  MHPathFinderBindings.h
 *  Mountainhome
 *
 *  Created by datalurkur on 12/28/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHPATHFINDERBINDINGS_H_
#define _MHPATHFINDERBINDINGS_H_
#include "RubyBindings.h"
#include "MHPathFinder.h"

class MHPathFinderBindings : public RubyBindings<MHPathFinder, false> {
public:
    static VALUE BlockTile(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE UnblockTile(VALUE self, VALUE x, VALUE y, VALUE z);

    static VALUE SetStartPosition(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE GetPathTo(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE IsPathBlocked(VALUE self, VALUE x, VALUE y, VALUE z);
    static VALUE GetClosestPath(VALUE self, VALUE array);

public:
    MHPathFinderBindings();
    virtual ~MHPathFinderBindings() {}
};

#endif
