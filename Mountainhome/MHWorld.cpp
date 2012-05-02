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
#include "Terrain.h"

#include "MHCore.h"
#include "OctreeSceneManager.h"
#include "EntityBindings.h"
#include "MHSelection.h"
#include "AStarPathFinder.h"

#include "LiquidManager.h"

#include <Base/FileSystem.h>
#include <Base/Math3D.h>

#include <Render/Light.h>
#include <Render/Light.h>

#include <Content/Content.h>
#include "PathVisualizer.h"

#include <Engine/Entity.h>
#include <Engine/Camera.h>
#include <Engine/Window.h>

MHWorld::MHWorld():
    _selection(NULL),
    _scene(NULL),
    _pathFinder(NULL),
    _pathVisualizer(NULL),
    _liquidManager(NULL),
    _activeCamera(NULL),
    _width(0),
    _height(0),
    _depth(0),
    _terrain(NULL)
{}

MHWorld::~MHWorld() {
    delete _terrain; _terrain = NULL;
    delete _selection; _selection = NULL;
    delete _pathFinder; _pathFinder = NULL;
    delete _liquidManager; _liquidManager = NULL;

    // Delete the scene AFTER the terrain as the terrain depends on scene.
    delete _scene; _scene = NULL;

    // Scene deletes the path visualizer for us
    _pathVisualizer = NULL;
}

void MHWorld::initialize(MHCore *core) {
    _scene = new OctreeSceneManager();
    _selection = new MHSelection();

    Light *l = _scene->createLight("mainLight");
    l->makeDirectionalLight(Vector3(5, 5, 5));
    l->setAmbient(0.3f, 0.3f, 0.3f);
    l->setDiffuse(0.7f, 0.7f, 0.7f);

//    Entity *sun = _scene->create<Entity>("sun");
//    sun->setMaterial(Content::GetOrLoad<Material>("sun.material"));
//    sun->setPosition(Vector3(5, 5, -5) * 100);
//    sun->setModel(new Sphere(100));
}

Camera* MHWorld::createCamera(std::string cameraName) {
    Camera *cam = new Camera(cameraName);
    _scene->addNode(cam);
    return cam;
}

void MHWorld::loadEmpty(int width, int height, int depth, MHCore *core) {
    _width = width;
    _height = height;
    _depth = depth;

    _pathFinder = new AStarPathFinder(Vector3(_width,_height,_depth));
    _liquidManager = new LiquidManager();
    _terrain = new Terrain(_width, _height, _depth, _scene);
}

Terrain *MHWorld::getTerrain() const { return _terrain; }

PathManager *MHWorld::getPathFinder() const { return _pathFinder; }

SceneManager* MHWorld::getScene() const {
    return _scene;
}

MHSelection* MHWorld::getSelection() {
    return _selection;
}

LiquidManager* MHWorld::getLiquidManager() { return _liquidManager; }

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
    _pathFinder = new AStarPathFinder(Vector3(_width,_height,_depth));
    _terrain = new Terrain(_width, _height, _depth, _scene);

    _terrain->load(worldName + ".mht");

    populate();

    return true;
}

void MHWorld::pickObjects(Camera *activeCam, Real startX, Real startY, Real endX, Real endY) {
    std::list <SceneNode*> selectedObjects;

    Vector2 start(startX, startY);
    Vector2 end(endX, endY);

    // Turn off real-time updating until all the new voxel properties are set
    _terrain->setAutoUpdate(false);

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
    _scene->addVisibleObjectsToList(scaledFrustum, selectedObjects);

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
    Vector3 startVoxel, endVoxel;
    if(projectRay(startPos, startDir, startVoxel) && projectRay(endPos, endDir, endVoxel)) {
        // Determine the affected area of voxels and add their coordinates to
        //  the voxel selection
        Info("Voxels selected from " << startVoxel << " to " << endVoxel);

		// Now, set voxels selected - only on the starting z-level for now.
        for(Real x = Math::Min(startVoxel[0], endVoxel[0]); x <= Math::Max(startVoxel[0], endVoxel[0]); x++) {

            for(Real y = Math::Min(startVoxel[1], endVoxel[1]); y <= Math::Max(startVoxel[1], endVoxel[1]); y++) {
				if(_terrain->getPaletteIndex(x, y, startVoxel[2]) != VoxelPalette::EmptyVoxel) {
                    // Add voxel to selection
                    Vector3 toAdd(x, y, startVoxel[2]);
                    _selection->append(toAdd);
				}
            }
        }
    }

    // Turn auto updating back on
    _terrain->setAutoUpdate(true);
}

bool MHWorld::projectRay(const Vector3 &start, const Vector3 &dir, Vector3 &nearestVoxel) {
    Vector3 rayPosition = start;

    // If our ray starts out of bounds, we need to check if it is disjoint from the world space or
    //  just needs to be moved fowards a bit, first.
    while(_terrain->isOutOfBounds(rayPosition)) {
        if      (rayPosition[0] < 0 && dir[0] < 0) { return false; }
        else if (rayPosition[0] >= _terrain->getWidth() && dir[0] > 0) { return false; }

        if      (rayPosition[1] < 0 && dir[1] < 0) { return false; }
        else if (rayPosition[1] >= _terrain->getHeight() && dir[1] > 0) { return false; }

        if      (rayPosition[2] < 0 && dir[2] < 0) { return false; }
        else if (rayPosition[2] >= _terrain->getDepth() && dir[2] > 0) { return false; }

        rayPosition = rayPosition + dir;
    }

    // Until the ray goes out of bounds
    while(!_terrain->isOutOfBounds(rayPosition)) {
        int iX = rayPosition[0],
            iY = rayPosition[1],
            iZ = rayPosition[2];

        // Check the currently occupied space in the world
        if(_terrain->getPaletteIndex(iX, iY, iZ) != VoxelPalette::EmptyVoxel) {
            nearestVoxel = Vector3(iX, iY, iZ);
            return true;
        }

        // Move the point along the ray forward
        rayPosition = rayPosition + dir;
    }

    return false;
}

void MHWorld::setActiveCamera(Camera *newActive) {
    _activeCamera = newActive;
}

Camera * MHWorld::getActiveCamera() {
    return _activeCamera;
}

void MHWorld::render(RenderContext *context) {
    if (getActiveCamera()) {
        _scene->render(getActiveCamera(), context);
    }
}

void MHWorld::showPath() {
    if(_pathVisualizer == NULL) {
        _pathVisualizer = new PathVisualizer(_pathFinder);
        _scene->addNode(_pathVisualizer);
    }

    _pathVisualizer->update(true, true);
    _pathVisualizer->addPath(_pathFinder->getLastPath());
}

void MHWorld::hidePath() {
    _pathVisualizer->clear();
}
