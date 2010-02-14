/*
 *  Node.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "Node.h"
#include "Entity.h"

Node::Node(Node *parent): _parent(parent) {}
Node::~Node() { removeAllChildren(); }

const PositionableObject* Node::getParent() const { return _parent; }
const AABB3& Node::getBoundingBox() const { return _boundingBox; }

void Node::attach(Entity *entity) {
    _entityList.push_back(entity);
    entity->setNode(this);
}

Node* Node::createChildNode(const std::string &name) {
    Node *newNode = new Node(this);
    _nodeList.push_back(newNode);
    return newNode;
}

void Node::removeAllChildren() {
    // Actually delete all children and clear the contents of the list.
    clear_list(_nodeList);

    // The scene keeps track of deleting entities. Here we can simply mark the Entities as
    // orphans and clear the list.
    EntityList::iterator itr = _entityList.begin();
    for(; itr != _entityList.end(); itr++) {
        (*itr)->setNode(NULL);
    }

    _entityList.clear();
}

void Node::update() {
    // Update orientation and position.
    updateDerivedValues();

    // And update the attached entities.
    bool first = true;
    EntityList::iterator entityItr = _entityList.begin();
    for (; entityItr != _entityList.end(); entityItr++) {
        if (first) { _boundingBox = (*entityItr)->getBoundingBox(); first = false; }
        else       { _boundingBox.encompass((*entityItr)->getBoundingBox());       }
        (*entityItr)->updateDerivedValues();
    }

    // And update the children.
    NodeList::iterator nodeItr = _nodeList.begin();
    for (; nodeItr != _nodeList.end(); nodeItr++) {
        _boundingBox.encompass((*nodeItr)->getBoundingBox());
        (*nodeItr)->update();
    }
}
