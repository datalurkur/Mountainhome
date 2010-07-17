/*
 *  main.cpp
 *  Mountainhome
 *
 *  Created by Brent Wilson on 11/10/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "MHCore.h"

#include "RubyRenderContext.h"
#include "RubyKeyboard.h"
#include "RubyViewport.h"
#include "RubyWindow.h"
#include "RubyLogger.h"
#include "RubyCamera.h"
#include "RubyState.h"
#include "RubyOptions.h"
#include "RubyEntity.h"
#include "RubyPath.h"

#include "MHWorld.h"
#include "MHUIElement.h"
#include "MHUIManager.h"
#include "MHTerrain.h"
#include "MHLiquidManager.h"

#include "SDL.H"

VALUE require_setup_wrapper(VALUE arg) {
    // And setup our ruby bindings before calling down into our main ruby setup script.
    RubyState::SetupBindings();
    RubyLogger::SetupBindings();
    RubyKeyboard::SetupBindings();
	RubyCamera::SetupBindings();
    RubyWindow::SetupBindings();
    RubyRenderContext::SetupBindings();
    RubyViewport::SetupBindings();
    RubyOptions::SetupBindings();
	RubyEntity::SetupBindings();
    RubyPath::SetupBindings();

    MHCore::SetupBindings();
    MHWorld::SetupBindings();
    MHUIElement::SetupBindings();
    MHUIManager::SetupBindings();
    MHTerrain::SetupBindings();
    MHLiquidManager::SetupBindings();

	rb_require("Mountainhome");
    VALUE rCore = rb_gv_get("$mhcore");
    AssignCObjFromValue(MHCore, cCore, rCore);
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
