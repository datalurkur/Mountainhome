/*
 *  MHCoreBindings.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "MHCoreBindings.h"
#include "WindowBindings.h"
#include "OptionsModuleBindings.h"
#include "RenderContextBindings.h"
#include "RubyStateBindings.h"
#include "RubyState.h"

#include <Engine/AudioSystem.h>

MHCoreBindings::MHCoreBindings()
: RubyBindings<MHCore, true>(
    rb_define_class("MHCore", rb_cObject),
    "MHCoreBindings")
{
    rb_define_method(_class, "register_state", RUBY_METHOD_FUNC(MHCoreBindings::RegisterState), 2);
    rb_define_method(_class, "set_state", RUBY_METHOD_FUNC(MHCoreBindings::SetState), -2);
    rb_define_method(_class, "window", RUBY_METHOD_FUNC(MHCoreBindings::GetWindow), 0);
    rb_define_method(_class, "render_context", RUBY_METHOD_FUNC(MHCoreBindings::GetRenderContext), 0);
    rb_define_method(_class, "options", RUBY_METHOD_FUNC(MHCoreBindings::GetOptions), 0);
    rb_define_method(_class, "personal_directory", RUBY_METHOD_FUNC(MHCoreBindings::GetPersonalDir), 0);
    rb_define_method(_class, "exit", RUBY_METHOD_FUNC(MHCoreBindings::Exit), 0);
    rb_define_method(_class, "stop_the_music", RUBY_METHOD_FUNC(MHCoreBindings::StopMusic), 0);
    rb_define_alloc_func(_class, MHCoreBindings::Alloc);
}

VALUE MHCoreBindings::Alloc(VALUE klass) {
    MHCore *cCore = new MHCore();
    VALUE rCore = NEW_RUBY_OBJECT_FULL(MHCoreBindings, cCore, klass);

    // Register the window.
    rb_iv_set(rCore, "@window", NEW_RUBY_OBJECT(WindowBindings, cCore->getMainWindow()));

    // Register the options module.
    rb_iv_set(rCore, "@options", NEW_RUBY_OBJECT(OptionsModuleBindings, cCore->getOptionsModule()));

    // Register the render context.
    rb_iv_set(rCore, "@render_context", NEW_RUBY_OBJECT(RenderContextBindings, cCore->getRenderContext()));
    
    // Create an array to store states in.
    rb_iv_set(rCore, "@states", rb_ary_new());

    // rb_global_variable(&rCore);

    return rCore;
}

VALUE MHCoreBindings::Exit(VALUE rSelf) {
    MHCore *cSelf = Get()->getPointer(rSelf);

    cSelf->stopMainLoop();
    return rSelf;
}

VALUE MHCoreBindings::SetState(VALUE rSelf, VALUE args) {
    VALUE name = rb_ary_shift(args);
    std::string strName = rb_string_value_cstr(&name);
    MHCore *cSelf = Get()->getPointer(rSelf);
    cSelf->setActiveState(strName, args);
    return name;
}

VALUE MHCoreBindings::RegisterState(VALUE rSelf, VALUE rState, VALUE name) {
    std::string stateName = rb_string_value_cstr(&name);
    RubyState *cState = RubyStateBindings::Get()->getPointer(rState);
    MHCore *cSelf = Get()->getPointer(rSelf);

    // Push the state onto the states array to prevent gc.
    rb_ary_push(rb_iv_get(rSelf, "@states"), rState);

    Info("Registering state " << cState << " under '" << stateName << "'.");
    cSelf->registerState(cState, stateName);
    return Qnil;
}

VALUE MHCoreBindings::GetWindow(VALUE rSelf) {
    MHCore *cSelf = Get()->getPointer(rSelf);
    return WindowBindings::Get()->getValue(cSelf->getMainWindow());
}

VALUE MHCoreBindings::GetRenderContext(VALUE rSelf) {
    MHCore *cSelf = Get()->getPointer(rSelf);
    return RenderContextBindings::Get()->getValue(cSelf->getRenderContext());
}

VALUE MHCoreBindings::GetOptions(VALUE rSelf) {
    MHCore *cSelf = Get()->getPointer(rSelf);
    return OptionsModuleBindings::Get()->getValue(cSelf->getOptionsModule());
}

VALUE MHCoreBindings::GetPersonalDir(VALUE rSelf) {
    MHCore *cSelf = Get()->getPointer(rSelf);

    return rb_str_new2((cSelf->getPersonalDir()).c_str());
}

VALUE MHCoreBindings::StopMusic(VALUE rSelf) {
    MHCore *cSelf = Get()->getPointer(rSelf);
    cSelf->getAudioSystem()->haltMusic();
    return rSelf;
}
