/*
 *  RubyRenderContext.h
 *  Mountainhome
 *
 *  Created by loch on 4/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RUBYRENDERCONTEXT_H_
#define _RUBYRENDERCONTEXT_H_
#include <Render/RenderContext.h>
#include "RubyBindings.h"

class RubyRenderContext : public RubyBindings<RenderContext, false> {
public:
    static void SetupBindings();
    static VALUE SetFilled(VALUE self);
    static VALUE SetWireframe(VALUE self);

};

#endif
