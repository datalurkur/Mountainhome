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
    rb_define_method(_class, "set_tile_pathable", RUBY_METHOD_FUNC(PathManagerBindings::SetTilePathable), 3);
    rb_define_method(_class, "set_tile_open", RUBY_METHOD_FUNC(PathManagerBindings::SetTileOpen), 3);
    rb_define_method(_class, "set_tile_closed", RUBY_METHOD_FUNC(PathManagerBindings::SetTileClosed), 3);
}

VALUE PathManagerBindings::SetTilePathable(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    PathManager *cSelf = Get()->getPointer(rSelf);
    cSelf->setNodeType(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ), PATHABLE);
    return rSelf;
}

VALUE PathManagerBindings::SetTileOpen(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    PathManager *cSelf = Get()->getPointer(rSelf);
    cSelf->setNodeType(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ), OPEN);
    return rSelf;
}

VALUE PathManagerBindings::SetTileClosed(VALUE rSelf, VALUE rX, VALUE rY, VALUE rZ) {
    PathManager *cSelf = Get()->getPointer(rSelf);
    cSelf->setNodeType(NUM2INT(rX), NUM2INT(rY), NUM2INT(rZ), CLOSED);
    return rSelf;
}
