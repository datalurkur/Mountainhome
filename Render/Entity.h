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
    virtual void addRenderablesToQueue(Frustum *bounds, RenderQueue *queue);
    virtual void updateImplementationValues();

    void setModel(Model *model);

    /*!\todo XXXBMW: Really, it doesn't make sense to set a material for an entire entity.
     * I'm allowing it for now, but this really should be revisited and corrected. */
    void setMaterial(Material *material);

    void setVisibility(bool state);

protected:
    friend class SceneManager;

    Entity(const std::string &name);
    virtual ~Entity();

    void generateRenderables();

protected:
    typedef std::list<MeshPartRenderable *> RenderableList;

	Model *_model;
    RenderableList _renderables;

    bool _visible;
};

#endif
