/*
 *  Frustum.h
 *  Render
 *
 *  Created by Brent Wilson on 4/8/06.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _FRUSTUM_H
#define _FRUSTUM_H
#include "Vector.h"
#include "Matrix.h"
#include "AABB.h"
#include "Plane.h"

/*! The frustum is the imaginary box that extends outwards from a camera and represents
 *  the bounds of what a camera actually sees. The shape of this box is determined by the
 *  type of projection specified (either ortho or perspective). This object acts kind of
 *  like a lense for the camera and specifies what will be rendered in a given scene.
 *
 *  Perspective projection is the most typical type of projection. The as the distance
 *  from the location of the camera increases the size of the frustum increases. It is
 *  this affect that makes things appear to become smaller as the move further away from
 *  us. This is the projection type that best mirrors human vision.
 *
 *  Orthographic projection treats the camera as if it is infinitely far away. As such,
 *  the boundings lines of the frustum advance in a straight line, unlike a perspective
 *  projection in which they move outwards. Because of this difference, objects do NOT
 *  appear smaller as they move away from the screen. This projection mode is often used
 *  in cad programs.
 *  \brief A frustum that is attached to a camera. Used for culling and maintains the
 *  projection matrix.
 *  \author Brent Wilson
 *  \date 4/6/07 */
class Frustum {
public:
    enum Sides {
        LEFT = 0,
        RIGHT,
        BOTTOM,
        TOP,
        NEAR,
        FAR
    };

    enum Collision {
        COMPLETE_OUT = 0,
        INTERSECT,
        COMPLETE_IN
    };

    Frustum();
    virtual ~Frustum();

    /*! Scales the sides of the frustum by linearly interpolating towards the opposite side.
     *  Scaling by 0 results in no change, while scaling by 1 moves the side to the opposite
     *  side of the frustum. */
    void scaleFrustum(
        Real leftRatio,       Real rightRatio,
        Real bottomRatio,     Real topRatio,
        Real nearRatio = 0.0, Real farRatio = 0.0);

    /*! Gets the plane representing the specified side of the frustum. */
    Plane* getPlane(Sides side);

    /*! Sets the specified plane to match the given plane. */
    void setPlane(Sides side, const Plane &plane);

    //Tests to see if different shapes are inside the current frustum.
    bool checkPoint(const Vector3 &point) const;
    Collision checkSphere(const Vector3 &center, float radius) const;
    Collision checkAABB(const Vector3 &mins, const Vector3 &maxs) const;
    Collision checkAABB(const AABB3 &box) const;

    friend std::ostream& operator<<(std::ostream &lhs, const Frustum &rhs);

protected:
    Plane _frustum[6]; //left, right, bottom, top, near, far

protected:

    //Normalize the frustum planes.
    void normalize();
    
};

#endif /* _FRUSTUM_H */
