/*
 *  Entity.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "Entity.h"
#include "Material.h"
#include "Node.h"

Entity::Entity(Model *model): _model(model), _material(NULL), _node(NULL) {}

Entity::~Entity() {}

PositionableObject* Entity::getParent() const { return _node; }

void Entity::updateImplementationValues() {
    _boundingBox.setMinMax(
        _orientation * _model->getBoundingBox().getMin() + _position,
        _orientation * _model->getBoundingBox().getMax() + _position);
}

const AABB3& Entity::getBoundingBox() const {
    return _boundingBox;
}

void Entity::setNode(Node *node) { _node = node; }
void Entity::setMaterial(Material *m) { _material = m; }
Material* Entity::getMaterial() { return _material; }

void Entity::render(RenderContext *context) {
    if (!_material) { THROW(InternalError, "Material is NULL!"); }

    context->setActiveMaterial(_material);
    context->setModelMatrix(getDerivedPositionalMatrix());
	_model->render(context);
    context->unsetActiveMaterial(_material);
}
