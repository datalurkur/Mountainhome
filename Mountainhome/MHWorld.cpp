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

#include "MHCore.h"
#include "OctreeSceneManager.h"
#include "EntityBindings.h"
#include "MHSelection.h"
#include "MHPathFinder.h"

#include <Render/Light.h>
#include <Engine/Camera.h>
#include <Engine/Window.h>

#include <Render/MaterialManager.h>
#include <Render/ModelManager.h>
#include <Engine/Entity.h>
#include <Render/Light.h>

#include <Render/Quad.h>
#include "PathVisualizer.h"

#include <Base/FileSystem.h>
#include <Base/Math3D.h>

MHWorld::MHWorld(): _materialManager(NULL), _modelManager(NULL),
_selection(NULL), _scene(NULL), _pathFinder(NULL),
_width(0), _height(0), _depth(0), _terrain(NULL) {}

MHWorld::~MHWorld() {
    delete _scene;   _scene   = NULL;
    delete _terrain; _terrain = NULL;
    delete _selection; _selection = NULL;
    delete _pathFinder; _pathFinder = NULL;
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

    _pathFinder = new MHPathFinder(_width, _height, _depth);
    _terrain = new ChunkedTerrain(_width, _height, _depth, _scene, _materialManager);
}

MHTerrain *MHWorld::getTerrain() const { return _terrain; }

MHPathFinder *MHWorld::getPathFinder() const { return _pathFinder; }

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
    _pathFinder = new MHPathFinder(_width, _height, _depth);
    _terrain = new ChunkedTerrain(_width, _height, _depth, _scene, _materialManager);

    _terrain->load(worldName + ".mht");

    populate();

    return true;
}

//VALUE MHWorldBindings::PickObjects(VALUE rSelf, VALUE rCam, VALUE rLeft, VALUE rBottom, VALUE rRight, VALUE rTop) {

void MHWorld::pickObjects(Camera *activeCam, Real startX, Real startY, Real endX, Real endY) {
    std::list <SceneNode*> selectedObjects;

    Vector2 start(startX, startY);
    Vector2 end(endX, endY);

    // Turn off real-time updating until all the new tile properties are set
    _terrain->setAutoUpdate(false);

    // Unselect any previously selected tiles
    // TODO - Depending on what pieces of code can modify the selection, it might be wise to write an "unselect" function at some point
    std::list <Vector3> previousSelection = _selection->getSelectedTiles();
    for(std::list <Vector3>::iterator itr = previousSelection.begin(); itr != previousSelection.end(); itr++) {
        _terrain->setTileProperty((*itr)[0], (*itr)[1], (*itr)[2], SELECTED, PropertyType(false));
    }

    // Clear previous selection
    _selection->clear();

    // SELECT ACTORS
    // Query the camera for a scaled version of the viewing frustum using the input parameters

    // Frustum needs minimum and maximum vectors to work properly.
    Vector2 lowerLeft(Math::Min(startX, endX), Math::Min(startY, endY));
    Vector2 upperRight(Math::Max(startX, endX), Math::Max(startY, endY));

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

    // Produce position and direction vectors for the starting xy and the ending xy.
    Vector3 startPos, startDir,
              endPos,   endDir;
    activeCam->createProjectionVector(start, startPos, startDir);
    activeCam->createProjectionVector(end, endPos, endDir);

    // Test the terrain for collision with the projections
    Vector3 startTile, endTile;
    if(projectRay(startPos, startDir, startTile) && projectRay(endPos, endDir, endTile)) {
        // Determine the affected area of tiles and add their coordinates to
        //  the tile selection
        Info("Tiles selected from " << startTile << " to " << endTile);

		// Now, set tiles selected - only on the starting z-level for now.
        for(Real x = Math::Min(startTile[0], endTile[0]); x <= Math::Max(startTile[0], endTile[0]); x++) {

            for(Real y = Math::Min(startTile[1], endTile[1]); y <= Math::Max(startTile[1], endTile[1]); y++) {
				if(!_terrain->isTileEmpty(x, y, startTile[2])) {
                    // Add tile to selection
                    Vector3 toAdd(x, y, startTile[2]);
                    _selection->append(toAdd);

                    // Mark tile to display as selected
                    // FIXME: this currently isn't being removed anywhere
					_terrain->setTileProperty(x, y, startTile[2], SELECTED, PropertyType(true));
				}
            }
        }
    }

    // Turn auto updating back on
    _terrain->setAutoUpdate(true);
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
        if(_terrain->getPaletteIndex(iX, iY, iZ) != TILE_EMPTY) {
            nearestTile = Vector3(iX, iY, iZ);
            return true;
        }

        // Move the point along the ray forward
        rayPosition = rayPosition + dir;
    }

    return false;
}
