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

class Model;
class SceneManager;

class Entity : public SceneNode, public Renderable {
public:
    static const std::string TypeName;

public:
    virtual void addVisibleObjectsToQueue(Camera *camera, RenderQueue *queue);
	virtual void render(RenderContext *context);
    virtual void updateImplementationValues();

    void setModel(Model *model);

protected:
    friend class SceneManager;

    Entity(const std::string &name);
    virtual ~Entity();

protected:
	Model *_model;

};

#endif
