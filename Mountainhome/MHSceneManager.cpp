/*
 *  MHSceneManager.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include <Render/MaterialManager.h>
#include <Render/Entity.h>
#include <Render/Model.h>
#include <Render/Node.h>

#include "MHSceneManager.h"
#include "MHWorld.h"

MHSceneManager::MHSceneManager(MHWorld *world): _world(world), _octreeMaxDepth(8),
_tileWidth(1.0), _tileHeight(1.0), _tileDepth(0.8) {}

MHSceneManager::~MHSceneManager() {}
