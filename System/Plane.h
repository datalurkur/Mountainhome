/***************************************************************************
 *                Plane.h
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

#ifndef _PLANE_H_
#define _PLANE_H_
#include "Vector.h"
#include "Math3D.h"

class Plane {
protected:
    Real _dist;
    Vector3 _normal;

public:
//C'tors
    Plane();
    Plane(Vector3 normal, Real distance);
     ~Plane();

//Functions
    bool isInFront(const Vector3 &point);
    /*! This function returns the distance of a point from the plane. If the point is in
     *  front of the plane (on the same side as the normal) then the returned value will
     *  be greater than 0. If the point is on the plane, the returned value will be 0. If
     *  the point is behind the plane (on the opposite side of the normal), the returned
     *  value will be less than 0. */
    Real distanceFrom(const Vector3 &point) const;
    //Normalizes the plane based on the plane equation (dist is normalized as well).
    void normalize();

//Accessors
    Real distance() const;
    void setDistance(Real newDist);

    const Vector3& normal() const;
    void setNormal(const Vector3& newNormal);
    void setNormal(const Real &x, const Real &y, const Real &z);
};

std::ostream& operator<<(std::ostream &lhs, const Plane &rhs);

#endif //_PLANE_H_
