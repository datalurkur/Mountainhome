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
#include <Base/FileSystem.h>

#include <Render/ShaderManager.h>
#include <Render/TextureManager.h>
#include <Render/MaterialManager.h>
#include <Render/ModelManager.h>

#include <Render/Sphere.h>
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
    rb_define_method(Class, "personal_directory", RUBY_METHOD_FUNC(MHCore::GetPersonalDir), 0);
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

VALUE MHCore::GetPersonalDir(VALUE self) {
    AssignCObjFromValue(MHCore, cSelf, self);

    return rb_str_new2((cSelf->getPersonalDir()).c_str());
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
    LogStream::SetLogLevel(LogStream::WarningMessage);
    LogStream::SetLogLevel(LogStream::InfoMessage);
    LogStream::SetLogTarget("Mountainhome.log");

    // Be lazy and add the base resource path with recursive searching enabled.
    _resourceGroupManager->addResourceLocation(_resourceDirectory, true);

    // Set the name of the state.
    _name = "Mountainhome";

    // Register some default resources.
    ///\todo XXXBMW: This REALLY must be doable from elsewhere.
    Material *grass = _materialManager->getOrLoadResource("grass.material");
    grass->enableMaterial();
    grass->getShader()->setTexture("color", 0);
    grass->getShader()->setTexture("normal", 1);
    grass->disableMaterial();
	_materialManager->registerResource("grass", grass);

    ///\todo XXXBMW: This REALLY must be doable from elsewhere.
    Material *gravel = _materialManager->getOrLoadResource("gravel.material");
    gravel->enableMaterial();
    gravel->getShader()->setTexture("color", 0);
    gravel->getShader()->setTexture("normal", 1);
    gravel->disableMaterial();
	_materialManager->registerResource("gravel", gravel);

    ///\todo XXXBMW: This REALLY must be doable from elsewhere.
    _textureManager->getOrLoadResource("PixelGrass.png")->setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    _textureManager->getOrLoadResource("PixelStone.png")->setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    _textureManager->getOrLoadResource("PixelDirt.png")->setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    // Make our two main textures nice and pretty.
    _textureManager->getOrLoadResource("cursor-black.png")->setFiltering(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    _textureManager->getOrLoadResource("cursor-black.png")->setTexCoordHandling(GL_CLAMP, GL_CLAMP);

    Material *whiteLit = new Material();
	whiteLit->setColor(1.0, 1.0, 1.0, 1.0);
    whiteLit->setAmbient(1.0, 1.0, 1.0, 1.0);
    whiteLit->setDiffuse(1.0, 1.0, 1.0, 1.0);
    whiteLit->applyLighting(true);
    _materialManager->registerResource("white-lit", whiteLit);

    Material *blueLit = new Material();
	blueLit->setColor(0.0, 0.0, 1.0, 1.0);
    blueLit->setAmbient(0.0, 0.0, 1.0, 1.0);
    blueLit->setDiffuse(0.0, 0.0, 1.0, 1.0);
    blueLit->applyLighting(true);
    _materialManager->registerResource("blue-lit", blueLit);

    Material *dwarfMat = new Material();
	dwarfMat->setColor(0.9, 0.6, 0.3, 1.0);
    dwarfMat->setAmbient(0.9, 0.6, 0.3, 1.0);
    dwarfMat->setDiffuse(0.9, 0.6, 0.3, 1.0);
    dwarfMat->applyLighting(true);
    _materialManager->registerResource("Dwarf", dwarfMat);
    
    _modelManager->registerResource("Dwarf", new Sphere(0.4));
    _modelManager->registerResource("Sphere", new Sphere(0.3));

    Model *fern = _modelManager->getOrLoadResource("fern.fbx");
    fern->setDefaultMaterial(whiteLit);
    _modelManager->registerResource("Fern", fern);

    // Temporary code!!!
    _audioSystem->playMusic(_resourceDirectory + "UI/intro.ogg");

//    Model *cube = _modelManager->getOrLoadResource("DrunkenMaster.fbx");
    _modelManager->registerResource("Cube", new Sphere(0.5));
}

MHCore::~MHCore() {}

void MHCore::registerState(RubyState *s, const std::string &key) {
    ParentState::registerState(s, key);
    _rubyStates.push_back(s);
}

void MHCore::innerLoop(int elapsed) {
    DefaultCore::innerLoop(elapsed);

    char buffer[64];
    snprintf(buffer, 64, "FPS: %i Models: %i Primitives: %i Vertices: %i",
        (int)_framerate,
        _renderContext->getModelCount(),
        _renderContext->getPrimitiveCount(),
        _renderContext->getVertexCount());

    _mainWindow->setPostCaption(buffer);
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
