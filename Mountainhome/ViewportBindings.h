/*
 *  ViewportBindings.h
 *  Mountainhome
 *
 *  Created by loch on 4/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _VIEWPORTBINDINGS_H_
#define _VIEWPORTBINDINGS_H_
#include <Render/Viewport.h>
#include "RubyBindings.h"

/*! Provides a direct mapping of Render/Viewport to ruby. */
class ViewportBindings : public RubyBindings<Viewport, false> {
public:

public:
    ViewportBindings();
    virtual ~ViewportBindings() {}

};

#endif
