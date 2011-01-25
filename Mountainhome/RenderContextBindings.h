/*
 *  RenderContextBindings.h
 *  Mountainhome
 *
 *  Created by loch on 4/17/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RENDERCONTEXTBINDINGS_H_
#define _RENDERCONTEXTBINDINGS_H_
#include <Render/RenderContext.h>
#include "RubyBindings.h"

class RenderContextBindings : public RubyBindings<RenderContext, false> {
public:
    RenderContextBindings();
    virtual ~RenderContextBindings() {}

};

#endif
