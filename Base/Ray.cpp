/*
 *  Ray.cpp
 *  3DMath
 *
 *  Created by Brent Wilson on 9/30/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */

#include "Ray.h"
#include "Plane.h"

Ray::Ray(): _origin(0, 0, 0), _direction(0, 0, -1), _minTime(0), _maxTime(Math::INF) {}
Ray::Ray(const Vector3 &origin, const Vector3 &direction, const Real &minT, const Real &maxT)
:_origin(origin), _direction(direction), _minTime(minT), _maxTime(maxT) {}

Ray::~Ray() {}

//Functions
void Ray::jitter(const Real &variance) { _direction.jitter(variance); }
void Ray::jitter(const Vector3 &variance) { _direction.jitter(variance); }
Ray Ray::getJitteredRay(const Real &variance) const {
    Ray result(*this);
    result.jitter(variance);
    return result;
}

Ray Ray::getJitteredRay(const Vector3 &variance) const {
    Ray result(*this);
    result.jitter(variance);
    return result;
}

bool Ray::intersectPlane(const Plane &plane, Real &t) const {
    t = plane.distance() / plane.normal().dotProduct(direction());
    return t >= 0;
}

//Accessors
Real Ray::length() const { return Math::Sqrt(lengthSquared()); }
Real Ray::lengthSquared() const { return (atMaxTime() - atMinTime()).lengthSquared(); }

const Real& Ray::minTime() const { return _minTime; }
const Real& Ray::maxTime() const { return _maxTime; }
const Vector3& Ray::origin() const { return _origin; }
const Vector3& Ray::direction() const { return _direction; }

void Ray::setOrigin(const Vector3 &newVec) { _origin = newVec; }
void Ray::setDirection(const Vector3 &newVec) { _direction = newVec; }
void Ray::setMinTime(const Real &newTime) const { ASSERT(newTime <= _maxTime); _minTime = newTime; }
void Ray::setMaxTime(const Real &newTime) const { ASSERT(newTime >= _minTime);  _maxTime = newTime; }

Vector3 Ray::atMinTime() const { return atTime(_minTime); }
Vector3 Ray::atMaxTime() const { return atTime(_maxTime); }
Vector3 Ray::atTime(Real time) const { return _origin + (_direction * time); }
