/*
 *  PositionableObject.cpp
 *  System
 *
 *  Created by loch on 2/1/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include <Base/Assertion.h>
#include "PositionableObject.h"

PositionableObject::PositionableObject() {}
PositionableObject::~PositionableObject() {}

void PositionableObject::updateDerivedValues() {
    if (getParent()) {
        _derivedOrientation = getParent()->_derivedOrientation * _orientation;
        _derivedPosition = getParent()->_derivedPosition + _position;
    } else {
        _derivedOrientation = _orientation;
        _derivedPosition = _position;    
    }
} // updateDerivedValues

Matrix PositionableObject::getDerivedPositionalMatrix() const {
    Matrix mat(_derivedOrientation);
    mat.setTranslation(_derivedPosition);
    return mat;
} // getDerivedPositionalMatrix

Matrix PositionableObject::getPositionalMatrix() const {
    Matrix mat(_orientation);
    mat.setTranslation(_position);
    return mat;
} // getPositionalMatrix

Quaternion PositionableObject::getDerivedOrientation() const { return _derivedOrientation; }
Quaternion PositionableObject::getOrientation() const { return _orientation; }

Vector3 PositionableObject::getDerivedPosition() const { return _derivedPosition; }
Vector3 PositionableObject::getPosition() const { return _position; }

void PositionableObject::setOrientation(const Quaternion &newOrientation) { _orientation = newOrientation; }

void PositionableObject::setPosition(Real x, Real y, Real z) { setPosition(Vector3(x, y, z)); }
void PositionableObject::setPosition(const Vector3 &newPosition) { _position = newPosition; }

void PositionableObject::moveAbsolute(const Vector3 &difference) { _position += difference; }
void PositionableObject::moveRelative(const Vector3 &difference) { moveAbsolute(_orientation * difference); }
void PositionableObject::moveForward (Real dist) { moveRelative(Vector3(0, 0, -1) * dist); }

void PositionableObject::moveBackward(Real dist) { moveForward(-dist);                     }
void PositionableObject::moveUpward  (Real dist) { moveRelative(Vector3(0, 1, 0) * dist);  }
void PositionableObject::moveDownward(Real dist) { moveUpward(-dist);                      }
void PositionableObject::strafeRight (Real dist) { moveRelative(Vector3(1, 0, 0) * dist);  }
void PositionableObject::strafeLeft  (Real dist) { strafeRight(-dist);                     }

void PositionableObject::rotate(Radian angle, const Vector3 &axis) { rotate(Quaternion(angle, axis));         }
void PositionableObject::rotate(Degree angle, const Vector3 &axis) { rotate(Quaternion(Radian(angle), axis)); }
void PositionableObject::rotate(const Quaternion &rot)             { _orientation = rot * _orientation;       }

void PositionableObject::adjustPitch(Radian angle) { rotate(angle, _orientation * Vector3(1, 0, 0)); }
void PositionableObject::adjustYaw  (Radian angle) { rotate(angle, _orientation * Vector3(0, 1, 0)); }
void PositionableObject::adjustRoll (Radian angle) { rotate(angle, _orientation * Vector3(0, 0, 1)); }
