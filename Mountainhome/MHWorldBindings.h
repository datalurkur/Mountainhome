/*
 *  MHWorldBindings.h
 *  Mountainhome
 *
 *  Created by loch on 10/23/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _MHWORLDBINDINGS_H_
#define _MHWORLDBINDINGS_H_
#include "RubyBindings.h"
#include "MHWorld.h"

class MHWorldBindings : public RubyBindings<MHWorld, true> {
public:
    /*! Creates the MHWorld bindings. */
    static void SetupBindings();

    /*! Marks the camera and world objects to keep the from being deleted. */
    static void Mark(MHWorld* world);

    /*! Creates a new instance of the MHWorld object and registers it as being associated
     *  with the given ruby object VALUE.
     * \param self The ruby space World object.
     * \param width The width of the world in tiles.
     * \param height The height of the world in tiles.
     * \param depth The depth of the world in tiles. */
    static VALUE Initialize(VALUE self, VALUE rCore);

    /*! Calls on scenemanager to create a new camera object */
    static VALUE CreateCamera(VALUE self, VALUE cameraName);

    /*! Terrain getter. */
    static VALUE GetTerrain(VALUE self);

    /*! LiquidManager getter. */
    static VALUE GetLiquidManager(VALUE self);

    /*! Tells the world to generate geometry in the scene.
     * \param self The ruby space World object. */
    static VALUE Populate(VALUE self);

    /*! Entity creation. */
    static VALUE CreateEntity(VALUE self, VALUE name, VALUE model, VALUE material, VALUE rX, VALUE rY, VALUE rZ);

    /*! Entity deletion. */
    static VALUE DeleteEntity(VALUE self, VALUE entity);

	/*! Get the selection in this world. */
	static VALUE GetSelection(VALUE self);
    
    /*! Gets the world's width. */
    static VALUE GetWidth(VALUE self);

    /*! Gets the world's height. */
    static VALUE GetHeight(VALUE self);

    /*! Gets the world's depth. */
    static VALUE GetDepth(VALUE self);

    /*! Import/Export the world. */
    static VALUE Save(VALUE self, VALUE world);
    static VALUE Load(VALUE self, VALUE world);
    static VALUE LoadEmpty(VALUE rSelf, VALUE width, VALUE height, VALUE depth, VALUE rCore);

    /*! Do picking. */
    static VALUE PickObjects(VALUE rSelf, VALUE rCam, VALUE rLeft, VALUE rBottom, VALUE rRight, VALUE rTop);

public:
    MHWorldBindings();
    virtual ~MHWorldBindings() {}

private:
    ID _terrain;
    ID _liquidManager;

};

#endif
