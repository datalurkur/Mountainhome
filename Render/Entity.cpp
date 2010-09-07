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

Entity::~Entity() {
    clear_list(_renderables);
}

void Entity::setModel(Model *model) {
    _model = model;
    generateRenderables();
}

void Entity::setMaterial(Material *material) {
    if (_renderables.size() > 1) {
        THROW(NotImplementedError, "This is not legit!");
    }

    RenderableList::iterator itr = _renderables.begin();
    for (; itr != _renderables.end(); itr++) {
        (*itr)->setMaterial(material);
    }
}

void Entity::addVisibleObjectsToQueue(Camera *camera, RenderQueue *queue) {
    SceneNode::addVisibleObjectsToQueue(camera, queue);

    // Add the MeshPartRenderables to the render queue.
    RenderableList::iterator itr = _renderables.begin();
    for (; itr != _renderables.end(); itr++) {
        queue->add(*itr);
    }
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

    ///\fixme Returning a complex data type by value!!
    Matrix posMatrix = getDerivedPositionalMatrix();

    RenderableList::iterator itr = _renderables.begin();
    for (; itr != _renderables.end(); itr++) {
        (*itr)->setPositionalMatrix(posMatrix);
    }
}

void Entity::generateRenderables() {
    clear_list(_renderables);

    if (_model != NULL) {
        unsigned int numRenderables = 0;
        for(int c = 0; c < _model->getMeshCount(); c++) {
            numRenderables += _model->getMesh(c)->getPartCount();
        }

        // If there are no meshparts, just generate a single renderable with all the indices
        if(numRenderables == 0) {
            _renderables.push_back(new MeshPartRenderable(_model, NULL));
        } else {
            for (int c = 0; c < _model->getMeshCount(); c++) {
                for (int d = 0; d < _model->getMesh(c)->getPartCount(); d++) {
                    _renderables.push_back(new MeshPartRenderable(_model, _model->getMesh(c)->getPart(d)));
                }
            }
        }
    }
}
