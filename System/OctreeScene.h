/*
 *  OctreeScene.h
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _OCTREESCENE_H_
#define _OCTREESCENE_H_
#include "Scene.h"

class Light;
class Camera;
class Entity;
class Node;

class OctreeScene : public Scene {
public:
    OctreeScene();
    virtual ~OctreeScene();

private:

};

#endif
