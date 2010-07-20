/*
 *  AABB.h
 *  3DMath
 *
 *  Created by Brent Wilson on 9/25/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */

#ifndef _AABB_H_
#define _AABB_H_
#include "Vector.h"

/*! The AABB class represents an Axis Aligned Bounding Box of N dimensions. It is the
 *  simplest kind of bounding box as it has no orientation and is defined only by an N
 *  dimensional position vector and an N dimensional radius vector.
 * \todo The class currently generates the min and max values on the fly. It would be much
 *  faster to actually cache these values in the future.
 * \brief An Axis Aligned Bounding Box.
 * \author Brent Wilson */
template <int N>
class AABB {
public:
#pragma mark Initialization and destruction
    /*! The default constructor. Builds an AABB at the origin with a size of 0. */
    AABB();

    /*! Copy constructor */
    AABB(const AABB<N> &copy);

    /*! Constructs an AABB  about the given center with the given radius.
     * \param center The center of the new bounding box.
     * \param radius The radius of the new bounding box. */
    AABB(const Vector<N> &center, const Vector<N> &radius);

    /*! Destructor */
    ~AABB();

    /*! Assignment operator */
    AABB& operator=(const AABB<N> &rhs);

#pragma mark Basic accessors
    /*! Returns a reference to the center of the AABB. */
    const Vector<N>& getCenter() const;

    /*! Returns a reference to a vector describing the radius of the AABB in all
     *  dimensions. */
    const Vector<N>& getRadius() const;

    /*! Returns a vector describing the diameter of the AABB in all dimensions.
     * \seealso getRadius() */
    Vector<N> getDiameter() const;

    /*! Returns a vector representing the maximum value encompassed by the AABB in each dimension. */
    Vector<N> getMax() const;

    /*! Returns a vector representing the minimum value encompassed by the AABB in each dimension. */
    Vector<N> getMin() const;

    /*! Sets the minimum and maximum values defining the AABB in all dimensions.
     * \param min The minimum values in each dimension that the AABB should encompass.
     * \param max The maximum values in each dimension that the AABB should encompass. */
    void setMinMax(const Vector<N> &min, const Vector<N> &max);

    /*! Sets the radius of the AABB. */
    void setRadius(Vector<N> radius);

    /*! Sets the center of the AABB. */
    void setCenter(Vector<N> center);

    /*! Sets the center and radius to 0. */
    void clear();
#pragma mark Functions
    /*! Grows the bounding box so that it encompasses the given bounding box, along with
     *  what it originally encompassed. */
    void encompass(const AABB<N> &rhs);

    /*! Grows the bounding box so that it encompasses the given point, along with what it
     *  originally encompassed. */
    void encompass(const Vector<N> &rhs);

    /*! Tests whether or not the bounding boxes define an overlapping space. To add some
     *  flexibility, the edged of the bounding box can either be included or ignored. If
     *  they are included, then AABB(Position(-1,0,0), Radius(1,1,1)) could be considered
     *  overlapping with AABB(Position(1,0,0), Radius(1,1,1)). If they are not included,
     *  these two would not be overlapping.
     * \param rhs The other bounding box to compare with.
     * \param includeEdges Whether or not to consider the very edges of the AABB.
     * \return True if the AABBs are overlapping, false otherwise. */
    bool overlapping(const AABB<N> &rhs, bool includeEdges = false) const;

    /*! Tests whether or not the bounding box contains a point. To add some flexibility,
     *  the edged of the bounding box can either be included or ignored in this
     *  calculation. If the edges are included, then AABB(Position(-1,0,0), Radius(1,1,1))
     *  would be consider could be considered to contain Point(0, 0, 0). If the edges are
     *  not considered, this would no longer be the case.
     * \param rhs The point to test.
     * \param includeEdges Whether or not to consider the very edges of the AABB.
     * \return True if the AABB contains the point, false otherwise. */
    bool contains(const Vector<N> &point, bool includeEdges = false) const;

    /*! This is a collisions test. It checks for a possible collisions and gives the time
     *  of initial and final collisions as a ratio between 0 and 1. If there is no
     *  collision, the values of r0 and r1 are undefined.
     * \param lhs0 The initial position of one of the AABBs.
     * \param lhs1 The final position of one of the AABBs.
     * \param rhs0 The initial position of the other AABB.
     * \param rhs1 The final position of the other AABB.
     * \param r0 Upon return, this is set to the time of initial collision.
     * \param r1 Upon return, this is set to the time they stop colliding.
     * \return True if there is a collision, false otherwise. */
    static bool Sweep(
        const AABB<N> &lhs_t0, const AABB<N> &lhs_t1,
        const AABB<N> &rhs_t0, const AABB<N> &rhs_t1,
        Real &r0, Real &r1);

    /*! This is a collisions test. It checks for a possible collisions and gives the time
     *  of initial and final collisions as a ratio between 0 and 1. If there is no
     *  collision, the values of r0 and r1 are undefined.
     * \param lhs The initial position of one of the AABBs.
     * \param vL The velocity of the first AABB.
     * \param rhs The initial position of the other AABB.
     * \param vR The velocity of the second AABB.
     * \param r0 Upon return, this is set to the time of initial collision.
     * \param r1 Upon return, this is set to the time they stop colliding.
     * \return True if there is a collision, false otherwise. */
    static bool Sweep(
        const AABB<N> &lhs, const Vector<N> &lhs_velocity,
        const AABB<N> &rhs, const Vector<N> &rhs_velocity,
        Real &r0, Real &r1);

protected:
    Vector<N> _center; //!< The center of the AABB.
    Vector<N> _radius; //!< The radius of the AABB.

};

#pragma mark Other definitions and inclusions
typedef AABB<2> AABB2;
typedef AABB<3> AABB3;

template <int N>
std::ostream& operator<<(std::ostream& lhs, const AABB<N> &rhs) {
    lhs << "AABB:" << std::endl;
    lhs << "  Center: " << rhs.getCenter() << std::endl;
    lhs << "  Size:   " << rhs.getRadius() << std::endl;
    lhs << "  Min:    " << rhs.getMin() << std::endl;
    lhs << "  Max:    " << rhs.getMax() << std::endl;
    lhs << std::endl;
    
    return lhs;
}

#include "AABB.hpp"

#endif /* _BASE_H */
