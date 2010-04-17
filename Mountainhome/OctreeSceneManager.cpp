/*
 *  OctreeSceneManager.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "OctreeSceneManager.h"

#include <Render/Entity.h>
#include <Render/Node.h>

OctreeSceneManager::OctreeSceneManager(): _octreeMaxDepth(8) {}
OctreeSceneManager::~OctreeSceneManager() {}

void OctreeSceneManager::removeWorldObjects() {
    _rootNode->removeAllChildren();
    clear_map(_entityMap);
}
