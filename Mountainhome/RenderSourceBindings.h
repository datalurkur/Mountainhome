/*
 *  RenderSourceBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/24/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RENDERSOURCEBINDINGS_H_
#define _RENDERSOURCEBINDINGS_H_
#include <Render/RenderSource.h>
#include "RubyBindings.h"

class RenderSourceBindings : public RubyBindings<RenderSource, false> {
public:
    RenderSourceBindings();
    virtual ~RenderSourceBindings();

    RenderSource *getPointer(VALUE rObj);

};

#endif
