/*
 *  Entity.h
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _ENTITY_H_
#define _ENTITY_H_
#include <Base/AABB.h>

#include "RenderContext.h"
#include "Renderable.h"
#include "SceneNode.h"
#include "Model.h"
#include "MeshPartRenderable.h"

class Model;
class SceneManager;

class Entity : public SceneNode {
public:
    static const std::string TypeName;

public:
    virtual void addVisibleObjectsToQueue(Camera *camera, RenderQueue *queue);
    virtual void updateImplementationValues();

    void setModel(Model *model);
    Model* getModel() { return _model; }

protected:
    friend class SceneManager;

    Entity(const std::string &name);
    virtual ~Entity();

    void generateRenderables();
    void clearRenderables();

protected:
	Model *_model;

    unsigned int _numRenderables;
    MeshPartRenderable **_renderables;
};

#endif
