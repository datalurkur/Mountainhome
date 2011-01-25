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
    WindowBindings();
    virtual ~WindowBindings() {}

};

#endif
