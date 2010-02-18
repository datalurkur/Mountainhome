/*
 *  MHWorld.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "MHWorld.h"

#include <Render/OctreeScene.h>
#include <Render/Light.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHWorld::SetupBindings() {
    Class = rb_define_class("MHWorld", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHWorld::Initialize), 0);
}

VALUE MHWorld::Initialize(VALUE self) {
    MHWorld::RegisterObject(self, new MHWorld());
    return self;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld implementation
//////////////////////////////////////////////////////////////////////////////////////////
MHWorld::MHWorld(): _scene(NULL) {
    _scene = new OctreeScene();
    Light *l = _scene->createLight("mainLight");
    ///\todo Make this a directional light.
    l->setAmbient(0.2f, 0.2f, 0.2f);
    l->setDiffuse(0.8f, 0.8f, 0.8f);
}

MHWorld::~MHWorld() {}

Scene* MHWorld::getScene() {
    return _scene;
}
