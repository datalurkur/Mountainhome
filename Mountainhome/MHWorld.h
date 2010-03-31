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
#include "RubyBindings.h"
#include <Base/Vector.h>

class MaterialManager;
class ModelManager;

class MHSceneManager;
class Camera;
class MHCamera;
class MHObject;
class MHTerrain;

/*! Represents the world itself in game. This contains all of the hooks into the engine
 *  and handles any and all direct engine interaction that may need to be done, such as
 *  setting up the initial scene: clear color, camera properties, lights, etc... It also
 *  provides the ruby bindings necessary to interact with the C++ object from within ruby.
 * \note This class should remain generally barebones, leaving much of the higher level
 *  logic to the ruby class. */
class MHWorld : public ManyObjectBinding<MHWorld> {
public:
#pragma mark MHWorld ruby bindings
    /*! Creates the MHWorld bindings. */
    static void SetupBindings();

    /*! Creates a new instance of the MHWorld object and registers it as being associated
     *  with the given ruby object VALUE.
     * \param self The ruby space World object.
     * \param width The width of the world in tiles.
     * \param height The height of the world in tiles.
     * \param depth The depth of the world in tiles. */
    static VALUE Initialize(VALUE self, VALUE width, VALUE height, VALUE depth);
    
    /*! Terrain setters and accessors */
    static VALUE GetTerrain(VALUE self);
    static VALUE SetTerrain(VALUE self, VALUE terrain);

    /*! Tells the world to generate geometry in the scene.
     * \param self The ruby space World object. */
    static VALUE Populate(VALUE self);
	
#pragma mark MHWorld declarations
public:
    /*! Creates a new MHWorld and sets up the given scene to render everything correctly.
     * \param width The width of the new world.
     * \param height The height of the new world.
     * \param depth The depth of the new world. */
    MHWorld(int width, int height, int depth);

    /*! Destroys the world and releases everything associated with it. */
    virtual ~MHWorld();

    /*! Gets the scene manager that was created by the world. */
    MHSceneManager *getScene() const;
    
    /*! Gets the terrain object. */
    MHTerrain *getTerrain() const;

    /*! Creates a new object in the world.
     * \param name The name of the object.
     * \param model The name of the model to represent the object.
     * \param material The material to represent the object. */
    MHObject *createObject(const std::string &name, const std::string model, const std::string material);

    /*! Tells the underlying scene to populate the world with geometry. */
    void populate();

    /*! Gets the width of the world */
    int getWidth();

    /*! Gets the height of the world */
    int getHeight();

    /*! Gets the depth of the world */
    int getDepth();

protected:
    /*! Creates and initializes the scene, setting up cameras, lights, etc... */
    void initializeScene();

    /*! Updates the viewports based on the current state. */
    void updateViewports();

protected:
    MaterialManager *_materialManager;
    ModelManager *_modelManager;
    MHSceneManager *_scene;
    MHTerrain *_terrain;

    bool  _split;  /*!< Whether or not split screen is active. */
    int   _width;  /*!< The width of the world. */
    int   _height; /*!< The height of the world. */
    int   _depth;  /*!< The depth of the world. */
};

#endif
