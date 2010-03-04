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

class MHSceneManager;
class Camera;

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
     * \params width The width of the world in tiles.
     * \params height The height of the world in tiles.
     * \params depth The depth of the world in tiles. */
    static VALUE Initialize(VALUE self, VALUE width, VALUE height, VALUE depth);

public:
#pragma mark MHWorld declarations
    /*! Creates a new MHWorld and sets up the given scene to render everything correctly. */
    MHWorld(const Vector3 &dimensions);

    /*! Destroys the world and releases everything associated with it. */
    virtual ~MHWorld();

    MHSceneManager *getScene() const;

    Camera* getCamera() const;

    void toggleCamera();

public:
    MHSceneManager *_scene; /*!< The scene associated with the world. */
    Camera *_lCam, *_rCam;  /*!< The cameras in the scene. */    
    Camera *_activeCam;     /*!< The camera currently controlled by input. */

    Vector3 _dimensions;    /*!< The dimensions of the world. */
};

#endif
