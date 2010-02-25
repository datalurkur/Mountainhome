/*
 *  Scene.cpp
 *  System
 *
 *  Created by loch on 12/29/09.
 *  Copyright 2009 Brent Wilson. All rights reserved.
 *
 */

#include "Scene.h"
#include "Camera.h"
#include "Entity.h"
#include "Light.h"
#include "Node.h"
#include "RenderQueue.h"

Scene::Scene(): _rootNode(NULL), _ambientLight(.1, .1, .1, 1) {
    _rootNode = new Node(NULL);
}

Scene::~Scene() {
    clearScene();
    delete _rootNode;
    _rootNode = NULL;
}

void Scene::clearScene() {
    _rootNode->removeAllChildren();
    clear_map(_entityMap);
    clear_map(_cameraMap);
    clear_map(_lightMap);
}

void Scene::render(RenderContext *context, Camera *source) {
    //Info("Rendering scene");

    // Update the bounding boxes and derived orientation/positions of everything in the scene.
    _rootNode->update();

    //!\todo Move this to the RenderContext
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, _ambientLight.ptr());

    // Setup the light state for the look.
	LightMap::iterator lightItr = _lightMap.begin();
	for (int i = 0; lightItr != _lightMap.end(); i++, lightItr++) {
        lightItr->second->setupState(i);
	}

    // Loop through the entities, adding them to the renderqueue
    EntityMap::iterator entityItr = _entityMap.begin();
    for (; entityItr != _entityMap.end(); entityItr++) {
        // Only render an entity if some part of it is contained by the frustum.
        if (source->getFrustum().checkAABB(entityItr->second->getBoundingBox())) {
            //Info("Adding to render queue entity " << entityItr->first);
			RenderQueue::Get()->addEntity(entityItr->second);
        }
    }
	
	// Tell the RenderQueue to render its contents
	//RenderQueue::Get()->renderAndClear(context);
	RenderQueue::Get()->renderWithWire(context);
}

Entity* Scene::createEntity(Model *model, const std::string &name) {
    if (_entityMap.find(name) != _entityMap.end()) {
        THROW(DuplicateItemError, "Entity named " << name <<
              " already exists in this scene!");
    }

    return _entityMap[name] = new Entity(model);
}

Camera* Scene::createCamera(const std::string &name) {
    if (_cameraMap.find(name) != _cameraMap.end()) {
        THROW(DuplicateItemError, "Camera named " << name <<
              " already exists in this scene!");
    }

    return _cameraMap[name] = new Camera(this);
}

Light* Scene::createLight(const std::string &name) {
    if (_lightMap.find(name) != _lightMap.end()) {
        THROW(DuplicateItemError, "Light named " << name <<
              " already exists in this scene!");
    }

    return _lightMap[name] = new Light();
}

Entity* Scene::getEntity(const std::string &name) {
    return _entityMap[name];
}

Camera* Scene::getCamera(const std::string &name) {
    return _cameraMap[name];
}

Light* Scene::getLight(const std::string &name) {
    return _lightMap[name];
}

void Scene::setAmbientLight(const Vector4& color) {
    _ambientLight = color;
}

const Vector4& Scene::getAmbientLight(void) const {
    return _ambientLight;
}

Node* Scene::getRootNode() {
    return _rootNode;
}
