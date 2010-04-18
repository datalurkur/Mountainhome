/*
 *  MHCore.cpp
 *  Mountainhome
 *
 *  Created by Brent Wilson on 12/21/09.
 *  Copyright 2009 NVIDIA. All rights reserved.
 *
 */

#include <Base/ResourceGroupManager.h>
#include <Base/Assertion.h>
#include <Base/Logger.h>

#include <Render/ShaderManager.h>
#include <Render/TextureManager.h>
#include <Render/MaterialManager.h>
#include <Render/ModelManager.h>
#include <Render/Quad.h>

#include <Engine/AudioSystem.h>
#include <Engine/Keyboard.h>

#include "RubyRenderContext.h"
#include "RubyOptions.h"
#include "RubyWindow.h"
#include "RubyState.h"

#include "MHCore.h"

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHCore ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHCore::SetupBindings() {
    // Define the ruby class.
    Class = rb_define_class("MHCore", rb_cObject);
    rb_define_method(Class, "register_state", RUBY_METHOD_FUNC(MHCore::RegisterState), 2);
    rb_define_method(Class, "set_state", RUBY_METHOD_FUNC(MHCore::SetState), -2);
    rb_define_method(Class, "window", RUBY_METHOD_FUNC(MHCore::GetWindow), 0);
    rb_define_method(Class, "render_context", RUBY_METHOD_FUNC(MHCore::GetRenderContext), 0);
    rb_define_method(Class, "options", RUBY_METHOD_FUNC(MHCore::GetOptions), 0);
    rb_define_method(Class, "exit", RUBY_METHOD_FUNC(MHCore::Exit), 0);
    rb_define_method(Class, "stop_the_music", RUBY_METHOD_FUNC(MHCore::StopMusic), 0);
    rb_define_alloc_func(Class, MHCore::Alloc);
}

VALUE MHCore::Alloc(VALUE klass) {
    MHCore *cCore = new MHCore();
    VALUE rCore = CreateBindingPairWithClass(klass, MHCore, cCore);

    // Register the window.
    CreateBindingPair(RubyWindow, cCore->getMainWindow());

    // Register the options module.
    CreateBindingPair(RubyOptions, cCore->getOptionsModule());

    // Register the render context.
    CreateBindingPair(RubyRenderContext, cCore->getRenderContext());

    // rb_global_variable(&rCore);

    return rCore;
}

void MHCore::Mark(MHCore *cCore) {
    rb_gc_mark(RubyWindow::GetValue(cCore->getMainWindow()));
    rb_gc_mark(RubyOptions::GetValue(cCore->getOptionsModule()));
    rb_gc_mark(RubyRenderContext::GetValue(cCore->getRenderContext()));
    std::list<RubyState *>::iterator itr = cCore->_rubyStates.begin();
    for (; itr != cCore->_rubyStates.end(); itr++) {
        rb_gc_mark(RubyState::GetValue(*itr));
    }
}

VALUE MHCore::Exit(VALUE self) {
    AssignCObjFromValue(MHCore, cSelf, self);

    cSelf->stopMainLoop();
    return self;
}

VALUE MHCore::SetState(VALUE self, VALUE args) {
    VALUE name = rb_ary_shift(args);
    std::string strName = rb_string_value_cstr(&name);
    AssignCObjFromValue(MHCore, cSelf, self);
    cSelf->setActiveState(strName, args);
    return name;
}

VALUE MHCore::RegisterState(VALUE self, VALUE state, VALUE name) {
    std::string stateName = rb_string_value_cstr(&name);
    AssignCObjFromValue(RubyState, cState, state);
    AssignCObjFromValue(MHCore, cSelf, self);

    Info("Registering state " << cState << " under '" << stateName << "'.");
    cSelf->registerState(cState, stateName);
    return state;
}

VALUE MHCore::GetWindow(VALUE self) {
    AssignCObjFromValue(MHCore, cSelf, self);
    return RubyWindow::GetValue(cSelf->getMainWindow());
}

VALUE MHCore::GetRenderContext(VALUE self) {
    AssignCObjFromValue(MHCore, cSelf, self);
    return RubyRenderContext::GetValue(cSelf->getRenderContext());
}

VALUE MHCore::GetOptions(VALUE self) {
    AssignCObjFromValue(MHCore, cSelf, self);
    return RubyOptions::GetValue(cSelf->getOptionsModule());
}

VALUE MHCore::StopMusic(VALUE self) {
    AssignCObjFromValue(MHCore, cSelf, self);
    cSelf->_audioSystem->haltMusic();
    return self;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHCore declarations
//////////////////////////////////////////////////////////////////////////////////////////
MHCore::MHCore(): DefaultCore("Mountainhome") {
    // Setup the logger how we want it.
    LogStream::SetLogLevel(LogStream::InfoMessage);
    LogStream::SetLogTarget("Mountainhome.log");

    // Be lazy and add the base resource path with recursive searching enabled.
    _resourceGroupManager->addResourceLocation(_resourceDirectory, true);

    // Set the name of the state.
    _name = "Mountainhome";

    // Register some default resources.
    // Some very basic materials to work with.
    Material *t_grey = new Material();
    t_grey->setColor(0.5f, 0.5f, 0.5f, 0.5f);
    t_grey->setTransparent(true);

    Material *white = new Material();
	white->setColor(1.0f, 1.0f, 1.0f, 1.0f);
    white->setAmbient(1.0f, 1.0f, 1.0f);
    white->setDiffuse(1.0, 1.0, 1.0, 1.0);
	
	Material *red = new Material();
	red->setColor(1.0f, 0.0f, 0.0f, 1.0f);
    red->setAmbient(1.0f, 0.0f, 0.0f);
    red->setDiffuse(1.0, 0.0, 0.0, 1.0);
	
	Material *blue = new Material();
	blue->setColor(0.0f,0.0f,1.0f,1.0f);
	blue->setAmbient(0.0f,1.0f,0.0f);
	blue->setDiffuse(0.0f,1.0f,0.0f,1.0f);

    Material *grass = _materialManager->getOrLoadResource("grass.material");
    grass->enableMaterial();
    grass->getShader()->setTexture("tex0", 0);
    grass->getShader()->setTexture("tex1", 1);
    grass->disableMaterial();

    Material *cursor = new Material();
	cursor->setColor(1.0f, 1.0f, 1.0f, 1.0f);
    cursor->setAmbient(1.0f, 1.0f, 1.0f);
    cursor->setDiffuse(1.0, 1.0, 1.0, 1.0);
    cursor->setTexture(_textureManager->getOrLoadResource("cursor-black.png"));
    cursor->setTransparent(true);

    Material *mhtitle = new Material();
    mhtitle->setColor(1.0f, 1.0f, 1.0f, 1.0f);
    mhtitle->setTexture(_textureManager->getOrLoadResource("mh-title.png"));
    mhtitle->setTransparent(true);

	_materialManager->registerResource("t_grey", t_grey);
	_materialManager->registerResource("cursor", cursor);
    _materialManager->registerResource("mh-title", mhtitle);
	_materialManager->registerResource("grass", grass);
	_materialManager->registerResource("white", white);
	_materialManager->registerResource("red", red);
	_materialManager->registerResource("blue", blue);

    // Make our two main textures nice and pretty.
    _textureManager->getOrLoadResource("cursor-black.png")->setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    _textureManager->getOrLoadResource("cursor-black.png")->setTexCoordHandling(GL_CLAMP, GL_CLAMP);
    _textureManager->getOrLoadResource("grass.png")->setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Temporary code!!!
    _audioSystem->playMusic(_resourceDirectory + "intro.ogg");
}

MHCore::~MHCore() {}

void MHCore::registerState(RubyState *s, const std::string &key) {
    ParentState::registerState(s, key);
    _rubyStates.push_back(s);
}

void MHCore::keyPressed(KeyEvent *event) {
    switch(event->key()) {
    case Keyboard::KEY_F1:
        stopMainLoop();
        break;
	default:
        ParentState::keyPressed(event);
    }
}
