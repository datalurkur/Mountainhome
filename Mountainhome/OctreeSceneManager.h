/*
 *  OctreeSceneManager.h
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MHSCENEMANAGER_H_
#define _MHSCENEMANAGER_H_
#include <Engine/SceneManager.h>
#include <Render/Model.h>
#include <Base/Vector.h>

class MaterialManager;
class MHWorld;

class OctreeSceneManager : public SceneManager {
public:
    OctreeSceneManager();
    virtual ~OctreeSceneManager();

protected:
    int  _octreeMaxDepth;

};

#endif
