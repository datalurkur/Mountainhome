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

    void clearScene();

    Node*   getRootNode();

    Camera* createCamera(const std::string &name);
    Light*  createLight(const std::string &name);

    Camera* getCamera(const std::string &name);
    Light*  getLight(const std::string &name);

private:
    typedef std::map<std::string, Camera*> CameraMap;
    typedef std::map<std::string, Light*> LightMap;

    Node      *_rootNode;
    CameraMap _cameraMap;
    LightMap  _lightMap;

};

#endif
