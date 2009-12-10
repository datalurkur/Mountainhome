/*
 *  Ray.h
 *  3DMath
 *
 *  Created by Brent Wilson on 9/30/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _RAY_H_
#define _RAY_H_
#include "Vector.h"

class Plane;
class Ray {
    Vector3 _origin;
    Vector3 _direction;
    mutable Real _minTime;
    mutable Real _maxTime;

public:
    Ray();
    Ray(const Vector3 &origin, const Vector3 &direction,
        const Real &minT = 0, const Real &maxT = Math::Infinity);
    ~Ray();

//Functions
    void jitter(const Real &variance);
    void jitter(const Vector3 &variance);
    Ray getJitteredRay(const Real &variance) const;
    Ray getJitteredRay(const Vector3 &variance) const;
    bool intersectPlane(const Plane &plane, Real &t) const;

//Accessors
    Real length() const;
    Real lengthSquared() const;
    const Real& minTime() const;
    const Real& maxTime() const;
    const Vector3& origin() const;
    const Vector3& direction() const;

    void setMinTime(const Real &newTime) const;
    void setMaxTime(const Real &newTime) const;
    void setOrigin(const Vector3 &newVec);
    void setDirection(const Vector3 &newVec);

    Vector3 atMinTime() const;
    Vector3 atMaxTime() const;
    Vector3 atTime(Real time) const;

};

#endif
