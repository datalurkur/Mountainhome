/*
 *  RubyWindow.h
 *  Mountainhome
 *
 *  Created by loch on 4/2/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _RUBYWINDOWPROXY_H_
#define _RUBYWINDOWPROXY_H_
#include <Engine/Window.h>
#include "RubyBindings.h"

/*! Provides a direct mapping of Engine/Window to ruby. */
class RubyWindow : public RubyBindings<Window, false> {
public:
    static void SetupBindings();
    static VALUE AddViewport(VALUE self, VALUE zLevel, VALUE x, VALUE y, VALUE w, VALUE z);
    static VALUE SetBGColor(VALUE self, VALUE r, VALUE g, VALUE b);
    static void Mark(Window *cWindow);

};

#endif
