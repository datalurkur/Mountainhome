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

MHWorld::MHWorld(): _materialManager(NULL), _modelManager(NULL),
_selection(NULL), _scene(NULL),
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
    _selection = new MHSelection();

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

MHSelection* MHWorld::getSelection() {
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

    // Clear previous selection
    _selection->clear();

    // SELECT ACTORS
    // Query the camera for a scaled version of the viewing frustum using the input parameters
    Frustum scaledFrustum;
    activeCam->createSelectionFrustum(lowerLeft, upperRight, scaledFrustum);

    // Pass the scaled frustum to the sceneManager
    _scene->addVisibleObjectsToList(&scaledFrustum, selectedObjects);

    // Modify world's selection object based on the objects returned from sceneManager
    std::list <SceneNode*>::iterator itr = selectedObjects.begin();
    for(; itr != selectedObjects.end(); itr++) {
        if((*itr)->getType() == "MHActor") {
            _selection->append((MHActor*)(*itr));
        }
    }

    // SELECT TILES
    // Compute projection vectors for each of the two corners respective
    //  to the camera's position and orientation
    Vector3 llStart, llDir,
            urStart, urDir;
    activeCam->createProjectionVector(lowerLeft, llStart, llDir);
    activeCam->createProjectionVector(upperRight, urStart, urDir);

    // Test the terrain for collision with the projections
    Vector3 llTile, urTile;
    if(projectRay(llStart, llDir, llTile) && projectRay(urStart, urDir, urTile)) {
        // Determine the affected area of tiles and add their coordinates to
        //  the tile selection
        Info("Tiles selected from " << llTile << " to " << urTile);
    }
}

bool MHWorld::projectRay(const Vector3 &start, const Vector3 &dir, Vector3 &nearestTile) {
    Vector3 rayPosition = start;

    // If our ray starts out of bounds, we need to check if it is disjoint from the world space or
    //  just needs to be moved fowards a bit, first.
    while(_terrain->isOutOfBounds(rayPosition)) {
        if     (rayPosition[0] < 0 && dir[0] < 0) { return false; }
        else if(rayPosition[0] >= _terrain->getWidth() && dir[0] > 0) { return false; }

        if     (rayPosition[1] < 0 && dir[1] < 0) { return false; }
        else if(rayPosition[1] >= _terrain->getHeight() && dir[1] > 0) { return false; }

        if     (rayPosition[2] < 0 && dir[2] < 0) { return false; }
        else if(rayPosition[2] >= _terrain->getDepth() && dir[2] > 0) { return false; }

        rayPosition = rayPosition + dir;
    }

    // Until the ray goes out of bounds
    while(!_terrain->isOutOfBounds(rayPosition)) {
        int iX = rayPosition[0],
            iY = rayPosition[1],
            iZ = rayPosition[2];

        // Check the currently occupied space in the world
        if(_terrain->getTileType(iX, iY, iZ) != TILE_EMPTY) {
            nearestTile = Vector3(iX, iY, iZ);
            return true;
        }

        // Move the point along the ray forward
        rayPosition = rayPosition + dir;
    }

    return false;
}
