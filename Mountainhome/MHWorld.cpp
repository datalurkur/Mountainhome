/*
 *  MHWorld.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "MHWorld.h"
#include "MHCore.h"
#include "MHSceneManager.h"
#include "MHCamera.h"
#include "MHObject.h"

#include <Render/Light.h>
#include <Render/Camera.h>
#include <Engine/Window.h>

#include <Render/MaterialManager.h>
#include <Render/ModelManager.h>
#include <Render/Entity.h>
#include <Render/Light.h>
#include <Render/Node.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHWorld::SetupBindings() {
    Class = rb_define_class("MHWorld", rb_cObject);
    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHWorld::Initialize), 3);
    rb_define_method(Class, "update_tile", RUBY_METHOD_FUNC(MHWorld::UpdateTile), 4);
    rb_define_method(Class, "populate", RUBY_METHOD_FUNC(MHWorld::Populate), 0);
}

VALUE MHWorld::Initialize(VALUE self, VALUE width, VALUE height, VALUE depth) {
    RegisterObject(self, new MHWorld(NUM2INT(width), NUM2INT(height), NUM2INT(depth)));
    return self;
}

VALUE MHWorld::UpdateTile(VALUE self, VALUE type, VALUE x, VALUE y, VALUE z) {
    GetObject(self)->updateTile(type, NUM2INT(x), NUM2INT(y), NUM2INT(z));
    return self;
}

VALUE MHWorld::Populate(VALUE self) {
    GetObject(self)->populate();
    return self;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld implementation
//////////////////////////////////////////////////////////////////////////////////////////
MHWorld::MHWorld(int width, int height, int depth): _scene(NULL), _width(width), _height(height), _depth(depth), _tiles(NULL), _split(true) {
    _materialManager = MHCore::GetMaterialManager();
    _modelManager = MHCore::GetModelManager();

    initializeTiles();
    initializeScene();
}

MHWorld::~MHWorld() {
    MHCore::GetWindow()->removeAllViewports();
    delete[] _tiles;
    delete _scene;
}

MHObject *MHWorld::createObject(const std::string &name, const std::string model, const std::string material) {
    Entity *entity = NULL;

    // Create the entity and add it to the scene.
    entity = _scene->createEntity(_modelManager->getOrLoadResource(model), name);
    entity->setMaterial(_materialManager->getOrLoadResource(material));
    _scene->getRootNode()->attach(entity);

    return new MHObject(name, this, entity);
}

MHSceneManager* MHWorld::getScene() const {
    return _scene;
}

void MHWorld::updateTile(VALUE type, int x, int y, int z) {
    _tiles[coordsToIndex(x, y, z)].type = type;
}

MHWorld::Tile* MHWorld::getTile(int x, int y, int z) {
    // Boundry checking.
    if (x < 0 || x >= _width ) { return NULL; }
    if (y < 0 || y >= _height) { return NULL; }
    if (z < 0 || z >= _depth ) { return NULL; }
    return _tiles + coordsToIndex(x, y, z);
}

MHWorld::Tile* MHWorld::getTopTile(int x, int y) {
    // Boundry checking.
    if (x < 0 || x >= _width ) { return NULL; }
    if (y < 0 || y >= _height) { return NULL; }

    // Look from top to bottom for the first filled in tile.
    for (int z = _depth - 1; z >= 0; z--) {
        Tile *t = getTile(x, y, z);
        if (t->isFilled()) {
            return t;
        }
    }

    THROW(InvalidStateError, "Bottomless pit!!!!");
    return NULL;
}

int MHWorld::coordsToIndex(int x, int y, int z) {
    return z * _width * _height + y * _width + x;
}

void MHWorld::initializeTiles() {
    _tiles = new Tile[_width * _height * _depth];
    for (int x = 0; x < _width; x++) {
        for (int y = 0; y < _height; y++) {
            for (int z = 0; z < _depth; z++) {
                Tile *t = getTile(x, y, z);
                t->parent = this;
                t->type = NULL;
                t->x = x;
                t->y = y;
                t->z = z;
            }
        }
    }
}

#include "TestSceneManager1.h"
#include "TestSceneManager2.h"
#include "TestSceneManager3.h"
void MHWorld::initializeScene() {
    _scene = new TestSceneManager2(this, _materialManager);
	Light *l = _scene->createLight("mainLight");
    ///\todo Make this a directional light.
    l->setAmbient(0.1f, 0.1f, 0.1f);
    l->setDiffuse(0.8f, 0.8f, 0.8f);
	l->setPosition(16, 16, 32);

	MHCore::GetWindow()->setBGColor(Color4(.4,.6,.8,1));
    MHCore::GetWindow()->addViewport(NULL, 0, 0.0f, 0.0f, 1.0f, 1.0f);
}

void MHWorld::populate() {
    _scene->populate();
}

int MHWorld::getWidth() { return _width; }
int MHWorld::getHeight() { return _height; }
int MHWorld::getDepth() { return _depth; }
