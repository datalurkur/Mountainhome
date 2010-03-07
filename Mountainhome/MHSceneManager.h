/*
 *  MHSceneManager.h
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MHSCENEMANAGER_H_
#define _MHSCENEMANAGER_H_
#include <Render/SceneManager.h>
#include <Base/Vector.h>

class MHWorld;

/*! The MHSceneManager is specially desiged to render the MH world. It currently takes
 *  an MHWorld and is capable of querying the world for tile information, generating
 *  geometry, grouping it into renderable chunks and placing these chunks into the scene.
 *  The chunk size and tile sizes are all variable. 
 * \breif A SceneManager designed specifically for rendering MHWorld objects. */
class MHSceneManager : public SceneManager {
public:
    /*! Creates a new scene manager that draws the given world.
     * \param world The world the scene manager is responsible for drawing. */
    MHSceneManager(MHWorld *world);

    /*! D'tor */
    virtual ~MHSceneManager();

    /*! Causes the scene manager to examine the world's tile matrix and generates the
     *  geometry and entities needed to render everything properly. */
    void populate();

private:
    /*! Creates a new chunk entity, fills it with geometry and puts it in the scene.
     * \params x The x location in the tile matrix that the chunk begins at.
     * \params y The y location in the tile matrix that the chunk begins at. 
     * \params z The z location in the tile matrix that the chunk begins at. */
    void createChunk(int x, int y, int z);

    /*! Looks at the generated geometry and calculates pretty normals for everything. */
    void generatePrettyNormalsForChunks();

private:
    MHWorld *_world;       /*!< A reference to the world we're rednering. */
    int  _octreeMaxDepth;  /*!< How deep to allow the octree to grow. */
    int  _tileChunkWidth;  /*!< How many tiles wide a chunk is. */
    int  _tileChunkHeight; /*!< How many tiles long a chunk is. */
    int  _tileChunkDepth;  /*!< How many tiles deep a chunk is. */
    Real _tileWidth;       /*!< The width of the tile in world units. */
    Real _tileHeight;      /*!< The height of the tile in world units. */
    Real _tileDepth;       /*!< The depth of the tile in world units. */

};

#endif
