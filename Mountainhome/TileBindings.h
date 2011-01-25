/*
 *  TileBindings.h
 *  Mountainhome
 *
 *  Created by Paul Wuersig on 11/13/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _TILEBINDINGS_H_
#define _TILEBINDINGS_H_
#include "TilePalette.h"
#include "RubyBindings.h"

class TileBindings {
public:
    static VALUE Method_Missing(int argc, VALUE * argv, VALUE self);
public:
    TileBindings();
private:
    static VALUE _rubyLookup; /* Reference to the Keyboard internal hash. */
};

#endif
