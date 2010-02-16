/*
 *  MHObject.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "MHObject.h"
#include "MHWorld.h"

#include <Render/MaterialManager.h>
#include <Render/ModelManager.h>
#include <Render/Entity.h>
#include <Render/Scene.h>
#include <Render/Node.h>

MHObject::MHObject(const std::string name, MHWorld *world, const std::string model,
const std::string material): _world(world), _entity(NULL) {
    Scene *scene = _world->getScene();

    // Create the entity and add it to the scene.
    _entity = scene->createEntity(ModelManager::Get()->loadResource(model), name);
    _entity->setMaterial(MaterialManager::Get()->loadResource(material));

    scene->getRootNode()->attach(_entity);
}

MHObject::~MHObject() {}
