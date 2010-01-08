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
class Node;

class OctreeScene : public Scene {
public:
    OctreeScene()                                 { /*!\todo implement me */ }
    virtual ~OctreeScene()                        { /*!\todo implement me */ }
    void clearScene()                             { /*!\todo implement me */ }
    Camera* createCamera(const std::string &name) { /*!\todo implement me */ return NULL; }
    Light*  createLight(const std::string &name)  { /*!\todo implement me */ return NULL; }
    Camera* getCamera(const std::string &name)    { /*!\todo implement me */ return NULL; }
    Light*  getLight(const std::string &name)     { /*!\todo implement me */ return NULL; }
    Node*   getRootNode()                         { /*!\todo implement me */ return NULL; }
};

#endif
