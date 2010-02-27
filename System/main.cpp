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
    ruby_sysinit(&argc, &argv); {
        RUBY_INIT_STACK;
        ruby_init();
        ruby_init_loadpath();

#if SYS_PLATFORM == PLATFORM_APPLE
        std::string path = macBundlePath() + "/Contents/Resources";
        ruby_incpush(path.c_str());
#endif

        ruby_script("Mountainhome (ruby)");
        Mountainhome::Get()->startMainLoop();
    }

    return ruby_cleanup(0);
}
