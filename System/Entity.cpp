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

const PositionableObject* Entity::getParent() const { return _node; }

const AABB3& Entity::getBoundingBox() const {
    ///\todo Would be nice to be able to directly apply rotations and transformations to bounding boxes.
    Vector3 min = _model->getBoundingBox().getMin();
    Vector3 max = _model->getBoundingBox().getMax();
    min = _orientation * min + _position;
    max = _orientation * max + _position;
    _boundingBox.setMinMax(min, max);

    return _boundingBox;
}

void Entity::setNode(Node *node) { _node = node; }
void Entity::setMaterial(Material *m) { _material = m; }

void Entity::render(RenderContext *context) {
    context->setActiveMaterial(_material);
    context->setModelMatrix(getDerivedPositionalMatrix());
	_model->render(context);
}
