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
class MHSceneManager : public SceneManager {
public:
    MHSceneManager(MHWorld *world);
    virtual ~MHSceneManager();

    void populate();

private:
    void createChunk(int x, int y, int z);

private:
    MHWorld *_world;
    int  _octreeMaxDepth;
    int  _tileChunkWidth;
    int  _tileChunkHeight;
    int  _tileChunkDepth;
    Real _tileWidth;
    Real _tileHeight;
    Real _tileDepth;

};

#endif
