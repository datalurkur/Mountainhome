/*
 *  main.cpp
 *  Mountainhome
 *
 *  Created by Brent Wilson on 11/10/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Mountainhome.h"
#include "RubyBindings.h"

#if defined(__APPLE__) && defined(__MACH__)
#   include "SDL.H"
#endif

int main(int argc, char *argv[]) {
    int state;
    ruby_sysinit(&argc, &argv); {
        RUBY_INIT_STACK;
        ruby_init();
        ruby_init_loadpath();

        Mountainhome::Get()->startMainLoop();
    }

    return ruby_cleanup(state);
}
