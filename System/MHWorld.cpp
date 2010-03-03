/*
 *  MHWorld.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "MHWorld.h"
#include "Mountainhome.h"
#include <Render/Scene.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHWorld::SetupBindings() {
    Class = rb_define_class("MHWorld", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHWorld::Initialize), 0);
    rb_define_method(Class, "scene=", RUBY_METHOD_FUNC(MHWorld::SetScene), 1);
}

VALUE MHWorld::Initialize(VALUE self) {
    MHWorld::RegisterObject(self, new MHWorld());
    return self;
}

VALUE MHWorld::SetScene(VALUE self, VALUE scene) {
    // Cast and pray!!!!
    GetObject(self)->setScene((Scene*)NUM2ULL(scene));
    return scene;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld implementation
//////////////////////////////////////////////////////////////////////////////////////////
MHWorld::MHWorld(): _scene(NULL) {}

MHWorld::~MHWorld() {}

void MHWorld::setScene(Scene *scene) {
    // This is here to make sure the given scene object is valid.
    scene->getRootNode();
    _scene = scene;
}

Scene* MHWorld::getScene() {
    return _scene;
}
