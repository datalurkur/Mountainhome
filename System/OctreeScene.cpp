/*
 *  OctreeScene.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "OctreeScene.h"
#include "Camera.h"
#include "Entity.h"
#include "Light.h"
#include "Node.h"

OctreeScene::OctreeScene(): _rootNode(NULL) {
    _rootNode = new Node(NULL);
}

OctreeScene::~OctreeScene() {
    delete _rootNode;
    _rootNode = NULL;
}

void OctreeScene::clearScene() {
    Scene::clearScene();

    _rootNode->removeAllChildren();
    clear_map(_cameraMap);
    clear_map(_lightMap);
}

Camera* OctreeScene::createCamera(const std::string &name) {
    if (_cameraMap.find(name) != _cameraMap.end()) {
        RAISE(DuplicateItemError, "Camera named " << name <<
              " already exists in this scene!");
    }

    return _cameraMap[name] = new Camera();
}

Light* OctreeScene::createLight(const std::string &name) {
    if (_lightMap.find(name) != _lightMap.end()) {
        RAISE(DuplicateItemError, "Light named " << name <<
              " already exists in this scene!");
    }

    return _lightMap[name] = new Light();
}

Camera* OctreeScene::getCamera(const std::string &name) {
    return _cameraMap[name];
}

Light* OctreeScene::getLight(const std::string &name) {
    return _lightMap[name];
}

Node* OctreeScene::getRootNode() {
    return _rootNode;
}
