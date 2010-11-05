/*
 *  main.cpp
 *  Mountainhome
 *
 *  Created by Brent Wilson on 11/10/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "RenderContextBindings.h"
#include "KeyboardBindings.h"
#include "ViewportBindings.h"
#include "WindowBindings.h"
#include "LoggerBindings.h"
#include "CameraBindings.h"
#include "RubyStateBindings.h"
#include "OptionsModuleBindings.h"
#include "EntityBindings.h"
#include "MHWorldBindings.h"
#include "MHCoreBindings.h"
#include "MHPathBindings.h"
#include "MHUIElementBindings.h"
#include "MHUIManagerBindings.h"
#include "MHTerrainBindings.h"
#include "MHLiquidManagerBindings.h"
#include "SceneNodeBindings.h"
#include "RenderSourceBindings.h"

#include "SDL.H"

VALUE require_setup_wrapper(VALUE arg) {
    // And setup our ruby bindings before calling down into our main ruby setup script.
    // Just dump the objects into the heap. They'll be cleaned up on exit.
    new RubyStateBindings();
    new LoggerBindings();
    new KeyboardBindings();
	new CameraBindings();
    new WindowBindings();
    new RenderContextBindings();
    new ViewportBindings();
    new OptionsModuleBindings();
	new EntityBindings();
    new MHCoreBindings();
    new MHWorldBindings();
    new MHPathBindings();
    new MHUIElementBindings();
    new MHUIManagerBindings();
    new MHTerrainBindings();
    new MHLiquidManagerBindings();
    new SceneNodeBindings();
    new RenderSourceBindings();

	rb_require("Mountainhome");
    VALUE rCore = rb_gv_get("$mhcore");
    MHCore *cCore = MHCoreBindings::Get()->getPointer(rCore);
    cCore->startMainLoop();
    return Qnil;
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

        int state = 0;
        rb_protect(require_setup_wrapper, 0, &state);
        translate_ruby_exception(state);
    }

    return ruby_cleanup(0);
}
