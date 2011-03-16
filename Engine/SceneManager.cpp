/*
 *  SceneManager.cpp
 *  System
 *
 *  Created by loch on 12/29/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#include <Render/RenderContext.h>

#include "SceneManager.h"
#include "Light.h"

SceneManager::SceneManager(): _rootNode(NULL), _ambientLight(.3, .3, .3, 1) {
    _rootNode = new SceneNode("ROOT");
}

SceneManager::~SceneManager() {
    deleteAllNodes();
    delete _rootNode;
    _rootNode = NULL;
}

void SceneManager::render(const std::string &camera, RenderContext *context) {
    render(getNode<Camera>(camera), context);
}

void SceneManager::render(Camera *camera, RenderContext *context) {
    _rootNode->updateDerivedValues();

    SceneNodeList visibleNodes;
    addVisibleObjectsToList(camera->getFrustum(), visibleNodes);

    RenderableList visibleRenderables;
    SceneNodeList::iterator itr;
    for (itr = visibleNodes.begin(); itr != visibleNodes.end(); itr++) {
        // Info("Visible: " << (*itr)->getName());
        (*itr)->addRenderablesToList(visibleRenderables);
        (*itr)->preRenderNotice();
    }

    // Setup the light state for the look.
    LightMap::iterator lightItr = _lightMap.begin();
    for (int i = 0; lightItr != _lightMap.end(); i++, lightItr++) {
        lightItr->second->enable(i);
    }

    context->setGlobalAmbient(_ambientLight);
    context->render(
        camera->getViewMatrix(),
        camera->getProjectionMatrix(),
        visibleRenderables);

    lightItr = _lightMap.begin();
    for (int i = 0; lightItr != _lightMap.end(); i++, lightItr++) {
        lightItr->second->disable();
    }
}

void SceneManager::deleteAllNodes() {
    // Need to loop manually since clear_map can't delete SceneNodes.
    SceneNodeMap::iterator itr = _nodeMap.begin();
    for (; itr != _nodeMap.end(); itr++) {
        delete itr->second;
    }

    _nodeMap.clear();
}

void SceneManager::addVisibleObjectsToList(const Frustum &bounds, SceneNodeList &visible) {
    _rootNode->addVisibleObjectsToList(bounds, visible);
}

void SceneManager::update() {
    // Update the bounding boxes and derived orientation/positions of everything in the scene.
    _rootNode->updateDerivedValues();
}

void SceneManager::setAmbientLight(const Vector4& color) {
    _ambientLight = color;
}

const Vector4& SceneManager::getAmbientLight(void) const {
    return _ambientLight;
}

bool SceneManager::genericHasNode(const std::string &name, const std::string &type) {
    SceneNodeMap::iterator itr = _nodeMap.find(name);
    return itr != _nodeMap.end() && itr->second->getType() == type;
}

SceneNode * SceneManager::genericGetNode(const std::string &name, const std::string &type) {
    SceneNodeMap::iterator itr = _nodeMap.find(name);
    return (itr != _nodeMap.end() && itr->second->getType() == type) ? itr->second : NULL;
}

SceneNode * SceneManager::genericRemoveNode(const std::string &name, const std::string &type) {
    SceneNodeMap::iterator itr = _nodeMap.find(name);

    if (itr == _nodeMap.end()) {
        THROW(ItemNotFoundError, "Attempting to remove a node that does not exist: " << name);
    }

    if (itr->second->getType() != type) {
        THROW(TypeMismatchError, "Types do not match: " <<
            itr->second->getType() << " != " << type);
    }

    _nodeMap.erase(itr);
    return itr->second;
}

Light* SceneManager::createLight(const std::string &name) {
    if (_lightMap.find(name) != _lightMap.end()) {
        THROW(DuplicateItemError, "Light named " << name <<
              " already exists in this scene!");
    }

    return _lightMap[name] = new Light();
}
