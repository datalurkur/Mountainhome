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
    SceneNode(name, TypeName), _numRenderables(0), _renderables(0) {
}

Entity::~Entity() {}

void Entity::setModel(Model *model) {
    _model = model;
    generateRenderables();
}

void Entity::addVisibleObjectsToQueue(Camera *camera, RenderQueue *queue) {
    int c;

    SceneNode::addVisibleObjectsToQueue(camera, queue);

    // Add the MeshPartRenderables to the render queue
    for(c = 0; c < _numRenderables; c++) {
        queue->add((Renderable*)(_renderables[c]));
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
}

void Entity::generateRenderables() {
    clearRenderables();

    if(_model != NULL) {
        int c, count;
        unsigned int numRenderables = 0;

        for(c = 0; c < _model->_numMeshes; c++) {
            numRenderables += _model->getMesh(c)->getPartCount();
        }

        Matrix posMatrix = getDerivedPositionalMatrix();

        // If there are no meshparts, just generate a single renderable with all the indices
        if(numRenderables == 0) {
            _numRenderables = 1;

            _renderables = (MeshPartRenderable**)malloc(sizeof(MeshPartRenderable*));
            _renderables[0] = new MeshPartRenderable(posMatrix, _model->getIndexCount(),
                                                     _model->getIndexBuffer(), _model->getVertexBuffer(),
                                                     _model->getNormalBuffer(), _model->getTexCoordBuffer());
            _renderables[0]->setMaterial(_model->getDefaultMaterial());
        }
        else {
            _numRenderables=numRenderables;

            _renderables = (MeshPartRenderable**)malloc(sizeof(MeshPartRenderable*) * numRenderables);

            count=0;
            for(c = 0; c < _model->_numMeshes; c++) {
                int d;

                for(d = 0; d < _model->getMesh(c)->getPartCount(); d++) {
                    ModelMeshPart *meshPart = _model->getMesh(c)->getPart(d);
                    MeshPartRenderable *renderable = new MeshPartRenderable(posMatrix, meshPart->getIndexCount(),
                                                                            _model->getIndexBuffer(), _model->getVertexBuffer(), 
                                                                            _model->getNormalBuffer(), _model->getTexCoordBuffer());
                    renderable->setMaterial(meshPart->getMaterial());
                    _renderables[count] = renderable;
                    count++;
                }
            }
        }
    }
}

void Entity::clearRenderables() {
    for(int c = 0; c < _numRenderables; c++) { delete _renderables[c]; }
    _numRenderables = 0;

    free(_renderables);
    _renderables = NULL;
}
