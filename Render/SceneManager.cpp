/*
 *  SceneManager.cpp
 *  System
 *
 *  Created by loch on 12/29/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#include "SceneManager.h"
#include "Light.h"
#include "RenderQueue.h"

SceneManager::SceneManager(): _rootNode(NULL), _ambientLight(.1, .1, .1, 1) {
    _rootNode = new SceneNode("ROOT");
}

SceneManager::~SceneManager() {
    clearScene();
    delete _rootNode;
    _rootNode = NULL;
}

void SceneManager::clearScene() {
    // Need to loop manually since clear_map can't delete SceneNodes.
    SceneNodeMap::iterator itr = _nodeMap.begin();
    for (; itr != _nodeMap.end(); itr++) {
        delete itr->second;
    }

    _nodeMap.clear();
}

void SceneManager::addVisibleObjectsToList(Frustum *bounds, std::list<SceneNode*> &visible) {
    _rootNode->addVisibleObjectsToList(bounds, visible);
}

void SceneManager::render(RenderContext *context, Camera *source) {
    // Update the bounding boxes and derived orientation/positions of everything in the scene.
    _rootNode->updateDerivedValues();

    ///\todo Move this to the RenderContext
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, _ambientLight.ptr());

    // Setup the light state for the look.
	LightMap::iterator lightItr = _lightMap.begin();
	for (int i = 0; lightItr != _lightMap.end(); i++, lightItr++) {
        lightItr->second->setupState(i);
	}

    // Find and render everything.
    RenderQueue *queue = RenderQueue::Get();
    _rootNode->addRenderablesToQueue(source->getFrustum(), queue);
    queue->renderAndClear(context);
}

void SceneManager::setAmbientLight(const Vector4& color) {
    _ambientLight = color;
}

const Vector4& SceneManager::getAmbientLight(void) const {
    return _ambientLight;
}

SceneNode* SceneManager::getRootNode() {
    return _rootNode;
}

bool SceneManager::genericHas(const std::string &name, const std::string &type) {
    SceneNodeMap::iterator itr = _nodeMap.find(name);
    return itr != _nodeMap.end() && itr->second->getType() == type;
}

SceneNode* SceneManager::genericGet(const std::string &name, const std::string &type) {
    SceneNodeMap::iterator itr = _nodeMap.find(name);
    return (itr != _nodeMap.end() && itr->second->getType() == type) ? itr->second : NULL;
}

void SceneManager::genericDestroy(const std::string &name, const std::string &type) {
    SceneNodeMap::iterator itr = _nodeMap.find(name);

    if (itr == _nodeMap.end()) {
        THROW(ItemNotFoundError, "Attempting to remove a node that "
            "does not exist: " << name);
    }

    if (itr->second->getType() != type) {
        THROW(TypeMismatchError, "Types do not match: " <<
            itr->second->getType() << " != " << type);
    }

    delete itr->second;
    _nodeMap.erase(itr);
}

Light* SceneManager::createLight(const std::string &name) {
    if (_lightMap.find(name) != _lightMap.end()) {
        THROW(DuplicateItemError, "Light named " << name <<
              " already exists in this scene!");
    }

    return _lightMap[name] = new Light();
}

template <>
Camera* SceneManager::create<Camera>(const std::string &name) {
    if (_nodeMap.find(name) != _nodeMap.end()) {
        THROW(DuplicateItemError, "Node named " << name <<
              " already exists in this scene!");
    }

    Camera* node = new Camera(name, this);
    _rootNode->attach(node);
    _nodeMap[name] = node;
    return node;
}
