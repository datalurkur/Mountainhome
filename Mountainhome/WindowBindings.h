/*
 *  WindowBindings.h
 *  Mountainhome
 *
 *  Created by loch on 4/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _WINDOWBINDINGS_H_
#define _WINDOWBINDINGS_H_
#include <Engine/Window.h>
#include "RubyBindings.h"

/*! Provides a direct mapping of Engine/Window to ruby. */
class WindowBindings : public RubyBindings<Window, false> {
public:
    static VALUE ClearViewports(VALUE self);
    static VALUE AddViewport(VALUE self, VALUE zLevel, VALUE x, VALUE y, VALUE w, VALUE z);
    static VALUE SetBGColor(VALUE self, VALUE r, VALUE g, VALUE b);
    static void Mark(Window *cWindow);

public:
    WindowBindings();
    virtual ~WindowBindings() {}

};

#endif
