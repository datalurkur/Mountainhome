/*
 *  MHWorld.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RubyCamera.h"

#include "MHWorld.h"
#include "SingleStepTerrain.h"
#include "IncrementalTerrain.h"
#include "ChunkedTerrain.h"

#include "SingleStepLiquidManager.h"
#include "MHCore.h"
#include "OctreeSceneManager.h"
#include "RubyEntity.h"

#include <Render/Light.h>
#include <Render/Camera.h>
#include <Engine/Window.h>

#include <Render/MaterialManager.h>
#include <Render/ModelManager.h>
#include <Render/Entity.h>
#include <Render/Light.h>

#include <Render/Quad.h>

#include <Base/FileSystem.h>

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
void MHWorld::SetupBindings() {
    Class = rb_define_class("MHWorld", rb_cObject);

    rb_define_method(Class, "initialize", RUBY_METHOD_FUNC(MHWorld::Initialize), 1);
    rb_define_method(Class, "terrain", RUBY_METHOD_FUNC(MHWorld::GetTerrain), 0);
    rb_define_method(Class, "liquid_manager", RUBY_METHOD_FUNC(MHWorld::GetLiquidManager), 0);

    rb_define_method(Class, "populate", RUBY_METHOD_FUNC(MHWorld::Populate), 0);
    rb_define_method(Class, "create_entity", RUBY_METHOD_FUNC(MHWorld::CreateEntity), 6);
    rb_define_method(Class, "delete_entity", RUBY_METHOD_FUNC(MHWorld::DeleteEntity), 1);

    rb_define_method(Class, "create_camera", RUBY_METHOD_FUNC(MHWorld::CreateCamera), 1);

    rb_define_method(Class, "width", RUBY_METHOD_FUNC(MHWorld::GetWidth), 0);
    rb_define_method(Class, "height", RUBY_METHOD_FUNC(MHWorld::GetHeight), 0);
    rb_define_method(Class, "depth", RUBY_METHOD_FUNC(MHWorld::GetDepth), 0);

    rb_define_method(Class, "save", RUBY_METHOD_FUNC(MHWorld::Save), 1);
    rb_define_method(Class, "load", RUBY_METHOD_FUNC(MHWorld::Load), 1);
    rb_define_method(Class, "load_empty", RUBY_METHOD_FUNC(MHWorld::LoadEmpty), 4);

    rb_define_method(Class, "pick_objects", RUBY_METHOD_FUNC(MHWorld::PickObjects), 5);

    rb_define_alloc_func(Class, MHWorld::Alloc);
}

void MHWorld::Mark(MHWorld* world) {
    rb_gc_mark(MHTerrain::GetValue(world->_terrain));
    rb_gc_mark(MHLiquidManager::GetValue(world->_liquidManager));
}

VALUE MHWorld::Initialize(VALUE rSelf, VALUE rCore) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    AssignCObjFromValue(MHCore, cCore, rCore);
    cSelf->initialize(cCore);

    return rSelf;
}

VALUE MHWorld::CreateCamera(VALUE rSelf, VALUE cameraName) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);

    std::string cCameraName = rb_string_value_cstr(&cameraName);

    Camera *cam = cSelf->createCamera(cCameraName);

    return RubyCamera::New(cam);
}

VALUE MHWorld::Populate(VALUE rSelf) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    cSelf->populate();
    return rSelf;
}

VALUE MHWorld::CreateEntity(VALUE rSelf, VALUE name, VALUE model, VALUE material, VALUE rX, VALUE rY, VALUE rZ) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);

    std::string cName  = rb_string_value_cstr(&name);
    std::string cMaterialName = rb_string_value_cstr(&material);
    std::string cModelName = rb_string_value_cstr(&model);

    // Get the model/material.
    Material *cMaterial = cSelf->_materialManager->getOrLoadResource(cMaterialName);
    Model    *cModel    = cSelf->_modelManager->getOrLoadResource(cModelName);
    cModel->setDefaultMaterial(cMaterial);

    // Setup the Entity.
    Entity* cEntity = cSelf->getScene()->create<Entity>(cName);
    cEntity->setPosition(Vector3(NUM2DBL(rX)+0.5, NUM2DBL(rY)+0.5, NUM2DBL(rZ)));
    cEntity->setModel(cModel);

    // define and return new Ruby-side MHEntity class object
    return CreateBindingPair(RubyEntity, cEntity);
}

VALUE MHWorld::DeleteEntity(VALUE rSelf, VALUE rName) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    
    std::string name = rb_string_value_cstr(&rName);
    
    cSelf->getScene()->destroy<Entity>(name);
    return rSelf;
}

VALUE MHWorld::GetTerrain(VALUE rSelf) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    return MHTerrain::GetValue(cSelf->_terrain);
}

VALUE MHWorld::GetLiquidManager(VALUE rSelf) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    return MHLiquidManager::GetValue(cSelf->_liquidManager);
}

VALUE MHWorld::GetWidth(VALUE rSelf) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    return INT2NUM(cSelf->_width);
}

VALUE MHWorld::GetHeight(VALUE rSelf) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    return INT2NUM(cSelf->_height);
}

VALUE MHWorld::GetDepth(VALUE rSelf) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    return INT2NUM(cSelf->_depth);
}

VALUE MHWorld::Save(VALUE rSelf, VALUE world) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    std::string cWorld = rb_string_value_cstr(&world);

    cSelf->save(cWorld);

    return rSelf;
}

VALUE MHWorld::Load(VALUE rSelf, VALUE world) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    std::string cWorld = rb_string_value_cstr(&world);
    cSelf->load(cWorld);
    CreateBindingPairWithClass(MHTerrain::GetClass(), MHTerrain, cSelf->_terrain);
    CreateBindingPairWithClass(get_class_value("LiquidManager"), MHLiquidManager, cSelf->_liquidManager);
    return rSelf;
}

VALUE MHWorld::LoadEmpty(VALUE rSelf, VALUE width, VALUE height, VALUE depth, VALUE rCore) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    AssignCObjFromValue(MHCore, cCore, rCore);
    cSelf->loadEmpty(NUM2INT(width), NUM2INT(height), NUM2INT(depth), cCore);
    CreateBindingPairWithClass(MHTerrain::GetClass(), MHTerrain, cSelf->_terrain);
    CreateBindingPairWithClass(get_class_value("LiquidManager"), MHLiquidManager, cSelf->_liquidManager);
    return rSelf;
}

VALUE MHWorld::PickObjects(VALUE rSelf, VALUE rCam, VALUE rLeft, VALUE rBottom, VALUE rRight, VALUE rTop) {
    AssignCObjFromValue(MHWorld, cSelf, rSelf);
    AssignCObjFromValue(Camera, cCam, rCam);

    Vector2 lowerLeft(NUM2DBL(rLeft),NUM2DBL(rBottom));
    Vector2 upperRight(NUM2DBL(rRight),NUM2DBL(rTop));
    cSelf->pickObjects(cCam, lowerLeft, upperRight);

    // TODO Eventually this will return a selection object
    return rSelf;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld implementation
//////////////////////////////////////////////////////////////////////////////////////////
MHWorld::MHWorld(): _materialManager(NULL), _modelManager(NULL), _scene(NULL),
_width(0), _height(0), _depth(0), _terrain(NULL) {}

MHWorld::~MHWorld() {
    delete _scene;   _scene   = NULL;
    delete _terrain; _terrain = NULL;
}

void MHWorld::initialize(MHCore *core) {
    _materialManager = core->getMaterialManager();
    _modelManager = core->getModelManager();

    _scene = new OctreeSceneManager();

	Light *l = _scene->createLight("mainLight");
	l->makeDirectionalLight(Vector3(5, 5, 5));
    l->setAmbient(0.3f, 0.3f, 0.3f);
    l->setDiffuse(0.7f, 0.7f, 0.7f);

//    Entity *sun = _scene->create<Entity>("sun");
//    sun->setMaterial(_materialManager->getOrLoadResource("sun.material"));
//    sun->setPosition(Vector3(5, 5, -5) * 100);
//    sun->setModel(new Sphere(100));
}

Camera* MHWorld::createCamera(std::string cameraName) {
    return _scene->create<Camera>(cameraName);
}

void MHWorld::loadEmpty(int width, int height, int depth, MHCore *core) {
    _width = width;
    _height = height;
    _depth = depth;

    _terrain = new ChunkedTerrain(_width, _height, _depth, _scene, _materialManager);
    // _terrain = new SingleStepTerrain(_width, _height, _depth, _scene, _materialManager);
    _liquidManager = new SingleStepLiquidManager(_terrain, _scene, _materialManager);
}

OctreeSceneManager* MHWorld::getScene() const {
    return _scene;
}

MHTerrain* MHWorld::getTerrain() const {
    return _terrain;
}

MHLiquidManager* MHWorld::getLiquidManager() const {
    return _liquidManager;
}

void MHWorld::populate() {
    _terrain->populate();
    _liquidManager->populate();
}

int MHWorld::getWidth() { return _width; }
int MHWorld::getHeight() { return _height; }
int MHWorld::getDepth() { return _depth; }

void MHWorld::save(std::string worldName) {
    if(worldName.length() == 0) {
        Error("Can't save a world with a zero-length filename.");
        return;
    }

    // Save off general world configuration stuffs (world dimensions, etc)
    std::string worldFile = worldName + ".mhw";

    File *wFile = FileSystem::GetFile(worldFile);
    wFile->open();
    if(!wFile->isOpen()) {
        Error("Filesystem failed to open world save file");
        return;
    }

    Info("Saving world data to " << worldFile);

    // Write world dimensions
    wFile->write(&_width,  sizeof(int));
    wFile->write(&_height, sizeof(int));
    wFile->write(&_depth,  sizeof(int));

    // Finish with the world file
    wFile->close();

    // Save off terrain data
    _terrain->save(worldName + ".mht");

    // Save off liquid data
    // TODO - Add liquid saving
}

bool MHWorld::load(std::string worldName) {
    // Load general world parameters
    std::string worldFile = worldName + ".mhw";

    File *wFile = FileSystem::GetFile(worldFile);
    if(!wFile->exists()) {
        Error("World file " << worldFile << " does not exist.");
        return false;
    }

    wFile->open();

    Info("Loading world data from " << worldFile);

    // Read the world dimensions
    wFile->read(&_width,  sizeof(int));
    wFile->read(&_height, sizeof(int));
    wFile->read(&_depth,  sizeof(int));

    Info("Read in " << _width << "," << _height << "," << _depth);

    // Finish with world file
    wFile->close();

    // Load the terrain data
    _terrain = new ChunkedTerrain(_width, _height, _depth, _scene, _materialManager);
    // _terrain = new SingleStepTerrain(_width, _height, _depth, _scene, _materialManager);
    _terrain->load(worldName + ".mht");

    // Load the liquid data
    _liquidManager = new SingleStepLiquidManager(_terrain, _scene, _materialManager);
    // TODO - Add liquid loading

    populate();

    return true;
}

void MHWorld::pickObjects(Camera *activeCam, Vector2 &lowerLeft, Vector2 &upperRight) {
    std::list <SceneNode*> selectedObjects;

    // Query the camera for a scaled version of the viewing frustum using the input parameters
    Frustum scaledFrustum;
    activeCam->createSelectionFrustum(lowerLeft, upperRight, scaledFrustum);

    // Pass the scaled frustum to the sceneManager
    _scene->addVisibleObjectsToList(&scaledFrustum, selectedObjects);
    Info("Selected " << selectedObjects.size() << " objects");

    // DEBUG CODE ====
    selectedObjects.clear();
    _scene->addVisibleObjectsToList(activeCam->getFrustum(), selectedObjects);
    Info("With camera's frustum, " << selectedObjects.size() << " objects");
    // ===============

    // Modify world's selection object based on the objects returned from sceneManager
    // TODO FIXME PLZKTHXBAI

    return;
}

