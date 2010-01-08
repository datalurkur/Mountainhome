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
#include <Base/Vector.h>

class Entity;

///\todo Look into making a parent class for everything that needs set/get position/rotation/transformation

class Node {
public:
    Node(Node *parent)                              { /*!\todo implement me */ }
    virtual ~Node()                                 { /*!\todo implement me */ }
    void attach(Entity *entity)                     { /*!\todo implement me */ }
    void setPosition(const Vector3 &vec)            { /*!\todo implement me */ }
    void setPosition(Real x, Real y, Real z = 0.0f) { /*!\todo implement me */ }
    Node* createNode(const std::string &name)       { /*!\todo implement me */ return NULL; } ///\todo Name? Compare to ogre...

};

#endif
