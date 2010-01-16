/*
 *  Scene.cpp
 *  System
 *
 *  Created by loch on 12/29/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#include "Scene.h"
#include "Entity.h"

Scene::Scene() {}

Scene::~Scene() {
    clearScene();
}

void Scene::clearScene() {
    clear_map(_entityMap);
}

Entity* Scene::createEntity(Model *model, const std::string &name) {
    if (_entityMap.find(name) != _entityMap.end()) {
        RAISE(DuplicateItemError, "Entity named " << name <<
              " already exists in this scene!");
    }

    return _entityMap[name] = new Entity(model);
}

Entity* Scene::getEntity(const std::string &name) {
    return _entityMap[name];
}
