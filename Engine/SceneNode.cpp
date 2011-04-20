/*
 *  SceneNode.cpp
 *  System
 *
 *  Created by loch on 2/1/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include <Base/Assertion.h>
#include <Base/Frustum.h>
#include <Content/Content.h>

#include "Renderable.h"
#include "SceneNode.h"
#include "Camera.h"

const std::string SceneNode::TypeName = "SceneNode";

SceneNode::SceneNode(const std::string &name):
_dirty(true), _fixedYawAxis(true), _yawAxis(0,1,0), _derivedPosition(0.0), _position(0.0),
_parent(NULL), _type(TypeName), _name(name), _visible(true), _boundingBoxRenderable(NULL) {}

SceneNode::SceneNode(const std::string &name, const std::string &type):
_dirty(true), _fixedYawAxis(true), _yawAxis(0,1,0), _derivedPosition(0.0), _position(0.0),
_parent(NULL), _type(type), _name(name), _visible(true), _boundingBoxRenderable(NULL) {}

SceneNode::~SceneNode() {
    clear_list(_renderables);
    if(_boundingBoxRenderable) { delete _boundingBoxRenderable; }
    if (_parent) { _parent->dettach(this); }
}

void SceneNode::setVisibility(bool state) {
    _visible = state;
}

void SceneNode::addRenderable(Renderable *renderable) {
#if DEBUG
    renderable->Parent = this;
#endif
    _renderables.push_back(renderable);
}

void SceneNode::removeRenderable(Renderable *renderable) {
    RenderableList::iterator itr = _renderables.begin();
    while (itr != _renderables.end()) {
        if (renderable == *itr) {
            itr = _renderables.erase(itr);
        } else {
            itr++;
        }
    }
}

void SceneNode::clearRenderables() {
    clear_list(_renderables);
}

void SceneNode::addVisibleObjectsToList(const Frustum &bounds, std::list<SceneNode*> &visible) {
    SceneNodeMap::iterator itr = _children.begin();
    for (; itr != _children.end(); itr++) {
        // Only render an entity if some part of it is contained by the frustum.
        if (bounds.checkAABB(itr->second->_derivedBoundingBox) && itr->second->_visible) {
            visible.push_back(itr->second);
            itr->second->addVisibleObjectsToList(bounds, visible);
        }
    }
}

void SceneNode::addAllObjectsToList(std::list<SceneNode*> &objects) {
    SceneNodeMap::iterator itr = _children.begin();
    for (; itr != _children.end(); itr++) {
        objects.push_back(itr->second);
        itr->second->addAllObjectsToList(objects);
    }
}

void SceneNode::addRenderablesToList(RenderableList &list, bool includeBB) {
    RenderableList::iterator itr;
    for (itr = _renderables.begin(); itr != _renderables.end(); itr++) {
        list.push_back(*itr);
    }
    if(includeBB && _boundingBoxRenderable) {
        list.push_back(_boundingBoxRenderable);
    }
}

const std::string &SceneNode::getName() const { return _name;   }
const std::string &SceneNode::getType() const { return _type;   }
SceneNode* SceneNode::getParent() const { return _parent; }

void SceneNode::attach(SceneNode *obj) {
    // Take care of the object's current parent.
    if (obj->_parent) {
        obj->_parent->dettach(obj);
    }

    // Drop it in the _children map and set its new parent.
    _children[obj->getName()] = obj;
    obj->_parent = this;
}

void SceneNode::dettach(SceneNode *obj) {
    _children.erase(obj->getName());
    obj->_parent = NULL;
}

void SceneNode::dettachAllChildren() {
    SceneNodeMap::iterator itr = _children.begin();
    for (; itr != _children.end(); itr++) {
        itr->second->_parent = NULL;
    }

    _children.clear();
}

void SceneNode::updateDerivedValues() {
    if (!isDirty()) { return; }
    if (getParent()) {
        _derivedOrientation = getParent()->_derivedOrientation * _orientation;
        _derivedPosition = getParent()->_derivedPosition + _position;
    } else {
        _derivedOrientation = _orientation;
        _derivedPosition = _position;
    }

    bool updated = updateImplementationValues();
    if(updated) { updateBoundingBoxRenderable(); }
    setDirty(false);
} // updateDerivedValues

void SceneNode::updateRenderableViewMatrices() {
    RenderableList::iterator itr = _renderables.begin();
    for (; itr != _renderables.end(); itr++) {
        (*itr)->setModelMatrix(_derivedTransform);
    }
}

void SceneNode::updateTransformationMatrices() {
    _derivedTransform = Matrix::Affine(_derivedOrientation, _derivedPosition);
    _transform = Matrix::Affine(_orientation, _position);
}

bool SceneNode::updateImplementationValues() {
    AABB3 oldAABB = _derivedBoundingBox;

    if (_children.size() == 0) {
        _derivedBoundingBox.setCenter(_derivedPosition);
        _derivedBoundingBox.setRadius(Vector3(0, 0, 0));
    } else {
        bool first = true;
        SceneNodeMap::iterator nItr = _children.begin();
        for (; nItr != _children.end(); nItr++) {
            nItr->second->updateDerivedValues();
            if (first) { _derivedBoundingBox = nItr->second->_derivedBoundingBox; first = false; }
            else       { _derivedBoundingBox.encompass(nItr->second->_derivedBoundingBox);       }
        }
    }

    updateTransformationMatrices();
    updateRenderableViewMatrices();

    return (oldAABB != _derivedBoundingBox);
}

void SceneNode::updateBoundingBoxRenderable() {
    if(_boundingBoxRenderable) { delete _boundingBoxRenderable; }

    RenderOperation *bbOp = RenderOperation::CreateBoxOp(_derivedBoundingBox.getRadius() * 2.0, true);
    Material *bbMat = Content::GetOrLoad<Material>(_renderables.size() ? "white" : "red");
    _boundingBoxRenderable = new Renderable(bbOp, bbMat);

    _boundingBoxRenderable->setModelMatrix(Matrix::Translation(_derivedBoundingBox.getCenter()));
}

const Matrix & SceneNode::getDerivedTransformationMatrix() const {
    return _derivedTransform;
}

const Matrix & SceneNode::getLocalTransformationMatrix() const {
    return _transform;
}

const AABB3 & SceneNode::getDerivedAABB() const {
    return _derivedBoundingBox;
}

void SceneNode::setDirty(bool value) {
    _dirty = value;
    // Only cascade dirty calls upwards.
    if (_dirty && getParent()) {
        getParent()->setDirty();
    }
}

bool SceneNode::isDirty() const {
    return _dirty;
}

void SceneNode::setOrientation(const Quaternion &newOrientation) {
    _orientation = newOrientation;
    setDirty();
}

void SceneNode::setPosition(Real x, Real y, Real z) {
    setPosition(Vector3(x, y, z));
    setDirty();
}

void SceneNode::setPosition(const Vector3 &newPosition) {
    _position = newPosition;
    setDirty();
}

void SceneNode::setPositionX(Real x) { _position[0] = x; setDirty(); }
void SceneNode::setPositionY(Real y) { _position[1] = y; setDirty(); }
void SceneNode::setPositionZ(Real z) { _position[2] = z; setDirty(); }

Real SceneNode::getPositionX() { return _position[0]; }
Real SceneNode::getPositionY() { return _position[1]; }
Real SceneNode::getPositionZ() { return _position[2]; }

void SceneNode::moveAbsolute(const Vector3 &difference) {
    _position += difference;
    setDirty();
}

void SceneNode::rotate(const Quaternion &rot) {
    _orientation = rot * _orientation;
    setDirty();
}

Quaternion SceneNode::getDerivedOrientation() const { return _derivedOrientation; }
Quaternion SceneNode::getLocalOrientation() const { return _orientation; }

Vector3 SceneNode::getDerivedPosition() const { return _derivedPosition; }
Vector3 SceneNode::getLocalPosition() const { return _position; }

void SceneNode::moveRelative(const Vector3 &difference) { moveAbsolute(_orientation * difference); }
void SceneNode::moveForward (Real dist) { moveRelative(Vector3(0, 0, -1) * dist); }

void SceneNode::moveBackward(Real dist) { moveForward(-dist);                     }
void SceneNode::moveUpward  (Real dist) { moveRelative(Vector3(0, 1, 0) * dist);  }
void SceneNode::moveDownward(Real dist) { moveUpward(-dist);                      }
void SceneNode::strafeRight (Real dist) { moveRelative(Vector3(1, 0, 0) * dist);  }
void SceneNode::strafeLeft  (Real dist) { strafeRight(-dist);                     }

void SceneNode::rotate(Radian angle, const Vector3 &axis) { rotate(Quaternion::FromAxisAngle(angle, axis)); }
void SceneNode::rotate(Degree angle, const Vector3 &axis) { rotate(Quaternion::FromAxisAngle(angle, axis)); }

void SceneNode::adjustPitch(Radian angle) { rotate(angle, _orientation * Vector3(1, 0, 0)); }
void SceneNode::adjustRoll (Radian angle) { rotate(angle, _orientation * Vector3(0, 0, 1)); }
void SceneNode::adjustYaw  (Radian angle) {
    rotate(angle, (_fixedYawAxis ? _yawAxis : _orientation * Vector3(0, 1, 0)));
}

void SceneNode::lookAt(const Vector3 &pos) {
    setDirection(pos - _position);
}

void SceneNode::setDirection(const Vector3 &newDir) {
    if (_fixedYawAxis) {
        Vector3 zAxis = -newDir.getNormalized();
        Vector3 xAxis = _yawAxis.crossProduct(zAxis).getNormalized();
        Vector3 yAxis = zAxis.crossProduct(xAxis);
        _orientation = Quaternion::FromAxes(xAxis, yAxis, zAxis);
    } else {
        rotate(Quaternion::FindQuaternion(getDirection(), newDir.getNormalized()));
    }

    ASSERT_EQ(newDir.getNormalized(), (_orientation * Vector3(0,0,-1)).getNormalized());
} // setDirection

Vector3 SceneNode::getUpDirection() const { return _orientation * Vector3(0, 1, 0);  }
Vector3 SceneNode::getDirection()   const { return _orientation * Vector3(0, 0, -1); }

Vector3 SceneNode::getFixedYawAxis() const { return _yawAxis; }
void SceneNode::setFixedYawAxis(bool fixed, const Vector3 &axis) {
    _fixedYawAxis = fixed;
    _yawAxis = axis;
}
