/*
 *  MHWorld.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "CameraBindings.h"

#include "MHWorld.h"
#include "SingleStepTerrain.h"
#include "IncrementalTerrain.h"
#include "ChunkedTerrain.h"

#include "SingleStepLiquidManager.h"
#include "MHCore.h"
#include "OctreeSceneManager.h"
#include "EntityBindings.h"
#include "MHSelection.h"

#include <Render/Light.h>
#include <Render/Camera.h>
#include <Engine/Window.h>

#include <Render/MaterialManager.h>
#include <Render/ModelManager.h>
#include <Render/Entity.h>
#include <Render/Light.h>

#include <Render/Quad.h>

#include <Base/FileSystem.h>

MHWorld::MHWorld(): _materialManager(NULL), _modelManager(NULL), _selection(NULL), _scene(NULL),
_width(0), _height(0), _depth(0), _terrain(NULL) {}

MHWorld::~MHWorld() {
    delete _scene;   _scene   = NULL;
    delete _terrain; _terrain = NULL;
    delete _selection; _selection = NULL;
}

void MHWorld::initialize(MHCore *core) {
    _materialManager = core->getMaterialManager();
    _modelManager = core->getModelManager();

    _scene = new OctreeSceneManager();
    _selection = new MHActorSelection();

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

MHTerrain *MHWorld::getTerrain() const { return _terrain; }

MHLiquidManager *MHWorld::getLiquidManager() const { return _liquidManager; }

OctreeSceneManager* MHWorld::getScene() const {
    return _scene;
}

MHActorSelection* MHWorld::getSelection() {
    return _selection;
}

MaterialManager *MHWorld::getMaterialManager() {
    return _materialManager;
}

ModelManager *MHWorld::getModelManager() {
    return _modelManager;
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

    // Modify world's selection object based on the objects returned from sceneManager
    _selection->clear();
    std::list <SceneNode*>::iterator itr = selectedObjects.begin();
    for(; itr != selectedObjects.end(); itr++) {
        if((*itr)->getType() == "MHActor") {
            _selection->append((MHActor*)(*itr));
        }
    }
}

