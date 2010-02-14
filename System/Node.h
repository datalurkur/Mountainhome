/*
 *  Node.h
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _NODE_H_
#define _NODE_H_
#include <Base/AABB.h>

#include "PositionableObject.h"

class Entity;

///\todo Look into making a parent class for everything that needs set/get position/rotation/transformation with derivable values and whatnot.

class Node : public PositionableObject {
public:
    Node(Node *parent);
    virtual ~Node();

    const PositionableObject* getParent() const;

    const AABB3& getBoundingBox() const;

    void attach(Entity *entity);

    Node* createChildNode(const std::string &name);
    void removeAllChildren();
    void update();

private:
    typedef std::list<Entity*> EntityList;
    typedef std::list<Node*> NodeList;

private:
    Node *_parent;

    EntityList _entityList;
    NodeList _nodeList;

    AABB3 _boundingBox;

};

#endif
