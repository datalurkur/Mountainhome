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
#include <Render/RenderContext.h>
#include <Render/Model.h>
#include <Base/AABB.h>

#include "PositionableObject.h"

class Model;
class Node;

///\todo Look into making a parent class for everything that needs set/get position/rotation/transformation

class Material;
class Entity : public PositionableObject {
public:
    Entity(Model *m);
    virtual ~Entity();

    virtual PositionableObject* getParent() const;

    const AABB3& getBoundingBox() const;

    void setNode(Node *node);
    void setMaterial(Material *mat);
	void render(RenderContext *context);

    virtual void updateImplementationValues();

private:
	Model *_model;
    Material *_material;
    Node *_node;

    AABB3 _boundingBox; //!< The bounding box encompassing the entity.

};

#endif
