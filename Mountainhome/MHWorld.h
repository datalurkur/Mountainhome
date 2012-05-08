/*
 *  MHWorld.h
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MHWORLD_H_
#define _MHWORLD_H_
#include <Engine/Entity.h>
#include <Base/Vector.h>

class OctreeSceneManager;
class Camera;
class EntityBindings;
class LiquidManager;
class MHCamera;
class MHObject;
class MHCore;
class MHSelection;
class Terrain;
class PathManager;
class PathVisualizer;

/*! Represents the world itself in game. This contains all of the hooks into the engine
 *  and handles any and all direct engine interaction that may need to be done, such as
 *  setting up the initial scene: clear color, camera properties, lights, etc... It also
 *  provides the ruby bindings necessary to interact with the C++ object from within ruby.
 * \note This class should remain generally barebones, leaving much of the higher level
 *  logic to the ruby class. */
class MHWorld {
public:
    /*! The distance any MHCamera is allowed to see. Fog should prevent viewing past this
     *  and cameras should be configured with this as their maximum view distance, to
     *  allow for intelligent frustem culling. */
    static const int MaximumViewDistance = 100;
    static const int FogStartDistance = 50;

public:
    /*! Creates a new MHWorld */
    MHWorld();

    /*! Destroys the world and releases everything associated with it. */
    virtual ~MHWorld();

    /* Sets up the given scene to render everything correctly.
     * \param width The width of the new world.
     * \param height The height of the new world.
     * \param depth The depth of the new world. */
    void initialize(MHCore *core);

    /* Calls on the scene manager to create a camera, marking it and initializing its Ruby bindings */
    MHCamera* createCamera(std::string cameraName);

    /*! Gets the scene manager that was created by the world. */
    SceneManager *getScene() const;
    
    /*! Gets the terrain object. */
    Terrain *getTerrain() const;

    /*! Gets the path finder object. */
    PathManager *getPathFinder() const;

    /*! Tells the terrain object to pass world geometry to the scene. */
    void populate();

    /*! Populates the pathVisualizer with path data an adds its geometry to the scene. */
    void showPath();

    /*! Removes the pathVisualizer from the scene and deletes it. */
    void hidePath();

    /*! Gets the width of the world */
    int getWidth();

    /*! Gets the height of the world */
    int getHeight();

    /*! Gets the depth of the world */
    int getDepth();

    MHSelection* getSelection();

    LiquidManager* getLiquidManager();

    /*! Saves the world data */
    void save(std::string worldName);

    /*! Loads world data from a group of files */
    bool load(std::string worldName);

    void loadEmpty(int width, int height, int depth, MHCore *core);

    /*! Returns a list of objects within a selection area */
    void pickObjects(MHCamera *activeCam, Real startX, Real startY, Real endX, Real endY);

    /*! Projects a ray into the world and returns the first voxel it hits,
     *  returning false if a voxel is not hit. */
    bool projectRay(const Vector3 &start, const Vector3 &dir, Vector3 &nearestVoxel);

    /*! Sets the currently active camera used for rendering. */
    void setActiveCamera(MHCamera *newActive);

    /*! Gets the currently active camera used for rendering. */
    MHCamera * getActiveCamera();

    /*! Render the world scene with the current active camera and the given context. */
    void render(RenderContext *context);

protected:
    /*! Creates and initializes the scene, setting up cameras, lights, etc... */
    void initializeScene();

protected:
    OctreeSceneManager *_scene;

    Terrain *_terrain;
    MHSelection *_selection;
    PathManager *_pathFinder;
    LiquidManager *_liquidManager;
    MHCamera *_activeCamera;

    PathVisualizer *_pathVisualizer;

    bool  _split;  /*!< Whether or not split screen is active. */
    int   _width;  /*!< The width of the world. */
    int   _height; /*!< The height of the world. */
    int   _depth;  /*!< The depth of the world. */
};

#endif
