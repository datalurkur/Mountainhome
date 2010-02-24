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

class Scene;

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
     *  with the given ruby object VALUE. */
    static VALUE Initialize(VALUE self);

    /*! Calls teardown on the associated MHWorld object. */
	static VALUE Teardown(VALUE self);

    /*! Calls setup on the associated MHWorld object. */
	static VALUE Setup(VALUE self);

public:
#pragma mark MHWorld declarations
    /*! Creates a new MHWorld and sets up the given scene to render everything correctly. */
    MHWorld();

    /*! Destroys the world and releases everything associated with it. */
    virtual ~MHWorld();

    /*! Returns the scene associated with the world. */
    Scene* getScene();

protected:
    /*! Sets up the scene for rendering by installing the appropriate viewports and
     *  cameras. All work is done on the main window. */
	void setup();

    /*! Removes all viewports from the main window, stopping the world from being
     *  rendered. */
    void teardown();

private:
    Scene *_scene; /*!< The scene associated with the world. */
};

#endif
