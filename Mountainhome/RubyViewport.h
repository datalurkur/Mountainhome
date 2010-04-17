/*
 *  RubyViewport.h
 *  Mountainhome
 *
 *  Created by loch on 4/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RUBYVIEWPORT_H_
#define _RUBYVIEWPORT_H_
#include <Render/Viewport.h>
#include "RubyBindings.h"

/*! Provides a direct mapping of Render/Viewport to ruby. */
class RubyViewport : public RubyBindings<Viewport, false> {
public:
    static void SetupBindings();
    static VALUE AddSource(VALUE self, VALUE source, VALUE zLevel);

};

#endif
