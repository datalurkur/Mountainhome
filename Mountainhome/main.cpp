/*
 *  main.cpp
 *  Mountainhome
 *
 *  Created by Brent Wilson on 11/10/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "RubyBindings.h"
#include "SDL.H"

VALUE require_setup_wrapper(VALUE arg) {
    MHCore::Get()->startMainLoop();
}

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

<<<<<<< HEAD
        int state = 0;
        rb_protect(require_setup_wrapper, 0, &state);
        translate_ruby_exception(state);
=======
		int state = 0;
		rb_protect(require_setup_wrapper, 0, &state);
		translate_ruby_exception(state);
>>>>>>> a47597a3333c45e657314197e092ad1b6f35a958
    }

    return ruby_cleanup(0);
}
