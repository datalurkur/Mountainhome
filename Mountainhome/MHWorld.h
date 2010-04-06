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
class MHCore;

/*! Represents the world itself in game. This contains all of the hooks into the engine
 *  and handles any and all direct engine interaction that may need to be done, such as
 *  setting up the initial scene: clear color, camera properties, lights, etc... It also
 *  provides the ruby bindings necessary to interact with the C++ object from within ruby.
 * \note This class should remain generally barebones, leaving much of the higher level
 *  logic to the ruby class. */
class MHWorld : public RubyBindings<MHWorld, true> {
//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld ruby bindings
//////////////////////////////////////////////////////////////////////////////////////////
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
    static VALUE Initialize(VALUE self, VALUE width, VALUE height, VALUE depth, VALUE core);

    /*! Sets the type associated with a specific tile in the world.
     * \param self The ruby space World object.
     * \param type The ruby class representing what the tile is being set to.
     * \param x The x position of the tile to modify.
     * \param y The y position of the tile to modify.
     * \param z The z position of the tile to modify. */
    static VALUE UpdateTile(VALUE self, VALUE type, VALUE x, VALUE y, VALUE z);

    /*! Tells the world to generate geometry in the scene.
     * \param self The ruby space World object. */
    static VALUE Populate(VALUE self);

    /*! Gets the camera. */
    static VALUE GetCamera(VALUE self);

    /*! Gets the world's width. */
    static VALUE GetWidth(VALUE self);

    /*! Gets the world's height. */
    static VALUE GetHeight(VALUE self);

    /*! Gets the world's depth. */
    static VALUE GetDepth(VALUE self);

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark MHWorld declarations
//////////////////////////////////////////////////////////////////////////////////////////
public:
    /*! Repesents a tile in the world in as small a form as possible. */
    struct Tile {
        VALUE type;      /*!< The ruby class representing what this tile is. */
        MHWorld *parent; /*!< The world this tile belongs to. */
        int x, y, z;     /*!< The location of this tile in the tile matrix. */

        /*! Gets the topmost tile at the location offset from this tile's location.
         * \param xOffset The x offset from this tile to look at.
         * \param yOffset The y offset from this tile to look at.
         * \return If the offset moves out of bounds, a NULL pointer is returned. */
        Tile* getTopByOffset(int xOffset, int yOffset) {
            return parent->getTopTile(x + xOffset, y + yOffset);
        }

        /*! Checks whether or not the tile has been set to something valid. */
        bool isFilled() {
            return type;
        }

        /*! Checks to see if this tile is the highest in its column. */
        bool isTopLevel() {
            if (z + 1 >= parent->getDepth()) { return true; }
            return !parent->getTile(x, y, z + 1)->isFilled();
        }
    };

public:
    /*! Creates a new MHWorld */
    MHWorld();

    /*! Destroys the world and releases everything associated with it. */
    virtual ~MHWorld();

    /* Sets up the given scene to render everything correctly.
     * \param width The width of the new world.
     * \param height The height of the new world.
     * \param depth The depth of the new world. */
    void initialize(int width, int height, int depth, MHCore *core);

    /*! Gets the scene manager that was created by the world. */
    MHSceneManager *getScene() const;

    /*! Updates the type of a tile at a specific location.
     * \param type The ruby class representing what the tile is being set to.
     * \param x The x position of the tile to modify.
     * \param y The y position of the tile to modify.
     * \param z The z position of the tile to modify. */
    void updateTile(VALUE type, int x, int y, int z);

    /*! Gets the tile at the given location.
     * \return If the location is out of bounds, a NULL pointer is returned instead. */
    MHWorld::Tile* getTile(int x, int y, int z);

    /*! Gets the topmost tile at the given column location.
     * \return If the location is out of bounds, a NULL pointer is returned instead. */
    MHWorld::Tile* getTopTile(int x, int y);

    /*! Tells the underlying scene to populate the world with geometry. */
    void populate();

    /*! Gets the width of the world */
    int getWidth();

    /*! Gets the height of the world */
    int getHeight();

    /*! Gets the depth of the world */
    int getDepth();

protected:
    /*! Converts the given 3d array index to the needed 1d array index. */
    int coordsToIndex(int x, int y, int z);

    /*! Creates and sets up the tile matrix with default values. */
    void initializeTiles();

    /*! Creates and initializes the scene, setting up cameras, lights, etc... */
    void initializeScene();

    /*! Updates the viewports based on the current state. */
    void updateViewports();

protected:
    MaterialManager *_materialManager;
    ModelManager *_modelManager;
    MHSceneManager *_scene;
    Camera *_camera;

    bool  _split;  /*!< Whether or not split screen is active. */
    int   _width;  /*!< The width of the world. */
    int   _height; /*!< The height of the world. */
    int   _depth;  /*!< The depth of the world. */
    Tile *_tiles;  /*!< The matrix containing all world tiles. */
};

#endif
