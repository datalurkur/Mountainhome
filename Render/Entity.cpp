/*
 *  Entity.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "RenderQueue.h"
#include "Material.h"
#include "Entity.h"

const std::string Entity::TypeName = "Entity";

Entity::Entity(const std::string &name):
    SceneNode(name, TypeName) {}

Entity::~Entity() {}

void Entity::setModel(Model *model) {
    _model = model;
}

void Entity::addVisibleObjectsToQueue(Camera *camera, RenderQueue *queue) {
    SceneNode::addVisibleObjectsToQueue(camera, queue);
    queue->add(this);
}

void Entity::updateImplementationValues() {
    Vector3 min = _derivedOrientation * _model->getBoundingBox().getMin() + _derivedPosition;
    Vector3 max = _derivedOrientation * _model->getBoundingBox().getMax() + _derivedPosition;

    if (_children.size() == 0) {
        _derivedBoundingBox.setMinMax(min, max);
    } else {
        SceneNode::updateImplementationValues();
        _derivedBoundingBox.encompass(min);
        _derivedBoundingBox.encompass(max);
    }

}

void Entity::render(RenderContext *context) {
    ASSERT(getMaterial() && _model);
    context->setActiveMaterial(getMaterial());
    context->setModelMatrix(getDerivedPositionalMatrix());
	_model->render(context);
    context->unsetActiveMaterial(getMaterial());
}
