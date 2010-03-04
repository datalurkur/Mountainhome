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
    MHSceneManager();
    virtual ~MHSceneManager();

private:
    MHWorld *_world;
    Vector3  _tileChunkSize;
    int      _octreeMaxDepth;

};

#endif
