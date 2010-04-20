/***************************************************************************
 *                Plane.cc
 *
 *  Sun May 21 14:19:56 2006
 *  Copyright  2005  Brent Wilson
 *  Email    llazarus@gmail.com
 *
 *  File: $Id$
 *
 *  Revisions:
 *        $Log$
 ****************************************************************************/

#include "Plane.h"
#include "Assertion.h"
#include "Vector.h"

Plane::Plane(): _dist(0), _normal(0, 1, 0) {}
Plane::Plane(Vector3 normal, Real distance): _normal(normal), _dist(distance) {}

Plane::~Plane() {}

Real Plane::distanceFrom(const Vector3 &point) const {
    return point.dotProduct(_normal) - _dist;
}

bool Plane::isInFrontOrOn(const Vector3 &point) {
    return Math::ge(distanceFrom(point), 0.0);
}

const Vector3& Plane::normal() const {
    return _normal;
}

void Plane::setNormal(const Vector3& newNormal) {
    setNormal(newNormal[0], newNormal[1], newNormal[2]);
}

void Plane::setNormal(const Real &x, const Real &y, const Real &z) {
    _normal[0] = x;
    _normal[1] = y;
    _normal[2] = z;
}

void Plane::normalize() {
    Real length = _normal.length();
    _normal /= length;
    _dist /= length;
}

Real Plane::distance() const {
    return _dist;
}

void Plane::setDistance(Real newDist) {
    _dist = newDist;
}

std::ostream& operator<<(std::ostream &lhs, const Plane &rhs) {
    lhs << "Plane(";
    lhs << " Normal: " << rhs.normal();
    lhs << " Distance: " << rhs.distance();
    lhs << ")";
    return lhs;
}
