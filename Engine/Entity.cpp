/*
 *  Entity.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "Model.h"
#include "Material.h"
#include "Entity.h"
#include "Renderable.h"

const std::string Entity::TypeName = "Entity";

Entity::Entity(const std::string &name):
    SceneNode(name, TypeName), _hasLocalAABB(false) {}

Entity::Entity(const std::string &name, const std::string &typeName):
    SceneNode(name, typeName), _hasLocalAABB(false) {}

Entity::~Entity() {}

void Entity::expandLocalAABB(const AABB3 &aabb) {
    if (!_hasLocalAABB) {
        _localAABB = aabb;
    } else {
        _localAABB.encompass(aabb);
    }

    _hasLocalAABB = true;
}

void Entity::addModel(Model *model, Material *mat) {
    ASSERT(model);

    for (int i = 0; i < model->getMeshCount(); i++) {
        ModelMesh *mesh = model->getMesh(i);
        expandLocalAABB(mesh->getBoundingBox());
        _renderables.push_back(new Renderable(
            mesh->getRenderOperation(),
            mat ? mat : mesh->getDefaultMaterial()));
    }
}

void Entity::updateImplementationValues() {
    SceneNode::updateImplementationValues();

    if (_hasLocalAABB) {
        _derivedBoundingBox.encompass(_derivedTransform * _localAABB.getMin());
        _derivedBoundingBox.encompass(_derivedTransform * _localAABB.getMax());
    }
}
