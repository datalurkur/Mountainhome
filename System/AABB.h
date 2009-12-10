/*
 *  AABB.h
 *  3DMath
 *
 *  Created by Brent Wilson on 9/25/06.
 *  Copyright 2006 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _AABB_H_
#define _AABB_H_
#include "Vector.h"

enum Surroundings {
    LEFT_WALL = 1,
    RIGHT_WALL = 2,
    FLOOR = 4,
    CEILING = 8,
    BACK_WALL = 16,
    FRONT_WALL = 32
};

template <int N>
class AABB {
protected:
    Vector<N> m_center;
    Vector<N> m_radius;

    void setRadius(Vector<N> _radius);
    void setCenter(Vector<N> _center);

public:
//C'tors
    AABB();
    AABB(const Vector<N> &center, const Vector<N> &radius);
    ~AABB();

//Accessors
    const Vector<N>& center() const;
    const Vector<N>& radius() const;
    Vector<N> diameter() const;
    Vector<N> max() const;
    Vector<N> min() const;
    
    Real center(int n) const;
    Real radius(int n) const;
    Real diameter(int n) const;
    Real max(int n) const;
    Real min(int n) const;

    void setMinMax(const Vector3 &min, const Vector3 &max);

//Functions
    bool touching(const AABB<N> &rhs) const;
    bool overlaps(const AABB<N> &rhs) const;
    int nearbyWalls(const AABB<N> &rhs) const;
    void offsetCenter(const Vector<N> &offset);
    bool contains(const Vector<N> &point) const;
    Real distanceFrom(const AABB<N> &rhs) const;

    static bool Sweep(const AABB<N> &lhs_t0, const AABB<N> &lhs_t1,
                      const AABB<N> &rhs_t0, const AABB<N> &rhs_t1,
                      Real &r0, Real &r1);
    static bool Sweep(const AABB<N> &lhs, const Vector<N> &lhs_velocity,
                      const AABB<N> &rhs, const Vector<N> &rhs_velocity,
                      Real &r0, Real &r1);
};

typedef AABB<2> AABB2;
typedef AABB<3> AABB3;

template <int N>
std::ostream& operator<<(std::ostream& lhs, const AABB<N> &rhs) {
    lhs << "AABB:" << std::endl;
    lhs << "  Center: " << rhs.center() << std::endl;
    lhs << "  Size:   " << rhs.radius() << std::endl;
    lhs << "  Min:    " << rhs.min() << std::endl;
    lhs << "  Max:    " << rhs.max() << std::endl;
    lhs << std::endl;
    
    return lhs;
}

//******************************
//******* C'Tor
//******************************
template <int N>
AABB<N>::AABB() {}

template <int N>
AABB<N>::AABB(const Vector<N> &center, const Vector<N> &radius)
:m_center(center), m_radius(radius) {
    m_radius.absolute();
}

template <int N>
AABB<N>::~AABB() {}
    
//******************************
//******* Basic Functions
//******************************
template <int N>
void AABB<N>::offsetCenter(const Vector<N> &offset) {
    m_center += offset;
}

template <int N>
int AABB<N>::nearbyWalls(const AABB<N> &rhs) const {
    //TODO see if it would be faster to just call "touching" and then,
    //if true, checking where rhs is is compared to this.
    Vector<N> minDist = rhs.radius() + radius();
    Vector<N> dist = rhs.center() - center();
    int wall = 0;
    int retVal = 0;

    for (int i = 0; i < N; i++) {
        if (Math::eq(Math::Abs(dist[i]), minDist[i])) {
            bool outside = false;
            for (int j = 0; j < N; j++) {
                if (j == i) { continue; }
                if (Math::le(max(j), rhs.min(j))) { outside = true; }
                if (Math::ge(min(j), rhs.max(j))) { outside = true; }
            }
            if (!outside) {
                retVal |= 1 << (dist[i] > 0 ? wall + 1 : wall);
            }
        }

        wall += 2;
    }

    return retVal;
}

template <int N>
bool AABB<N>::touching(const AABB<N> &rhs) const {
    Vector<N> minDist = radius() + rhs.radius();
    Vector<N> dist = Absolute(center() - rhs.center());
    for (int i = 0; i < N; i++) {
        if (gt(dist[i], minDist[i])) { return false; }
    }

    return true;
}

template <int N>
bool AABB<N>::overlaps(const AABB<N> &rhs) const {
    Vector<N> minDist = radius() + rhs.radius();
    Vector<N> dist = (center() - rhs.center()).getAbsolute();
    for (int i = 0; i < N; i++) {
        if (Math::ge(dist[i], minDist[i])) { return false; }
    }

    return true;
}

template <int N>
bool AABB<N>::contains(const Vector<N> &point) const {
    Vector<N> dist = (center() - point);
    dist.absolute();
    for (int i = 0; i < N; i++) {
        if (Math::ge(dist[i], radius()[i])) { return false; }
    }

    return true;
}

//******************************
//******* Accessors
//******************************
template <int N>
void AABB<N>::setMinMax(const Vector3 &min, const Vector3 &max) {
    setRadius((max - min) / 2.0);
    setCenter(min + radius());
}

template <int N>
void AABB<N>::setRadius(Vector<N> _radius) {
    m_radius = _radius;
    m_radius.absolute();
}

template <int N>
void AABB<N>::setCenter(Vector<N> _center) {
    m_center = _center;
}

template <int N>
const Vector<N>& AABB<N>::center() const {
    return m_center;
}

template <int N>
Real AABB<N>::center(int n) const {
    return m_center[n];
}

template <int N>
const Vector<N>& AABB<N>::radius() const {
    return m_radius;
}

template <int N>
Real AABB<N>::radius(int n) const {
    return m_radius[n];
}

template <int N>
Vector<N> AABB<N>::diameter() const {
    return m_radius * 2;
}

template <int N>
Real AABB<N>::diameter(int n) const {
    return m_radius[n] * 2;
}

template <int N>
Vector<N> AABB<N>::max() const {
    return center() + radius();
}

template <int N>
Real AABB<N>::max(int n) const {
    assert(n < N);
    return center()[n] + radius()[n];
}

template <int N>
Vector<N> AABB<N>::min() const {
    return center() - radius();
}

template <int N>
Real AABB<N>::min(int n) const {
    assert(n < N);
    return center()[n] - radius()[n];
}

template <int N>
Real AABB<N>::distanceFrom(const AABB<N> &rhs) const {
    Vector<N> minDist = radius() + rhs.radius();
    Vector<N> dist = (center() - rhs.center()).getAbsolute();
    Vector<N> edgeDist = dist - minDist;
    edgeDist.clamp(0, 2e99);
    return edgeDist.length();
}

//This is a collisions test. It checks for a possible collisions and gives
//the time of initial and final collisions as a ratio between 0 and 1. If
//there is no collision, the values of r0 and r1 are undefined.
//
// @param lhs0    The initial position of one of the AABBs.
// @param lhs1    The final position of one of the AABBs.
// @param rhs0    The initial position of the other AABB.
// @param rhs1    The final position of the other AABB.
// @param r0    Upon return, this is set to the time of initial collision.
// @param r1    Upon return, this is set to the time they stop colliding.
//
// @return     True if there is a collision, false otherwise.
template <int N>
bool AABB<N>::Sweep(const AABB<N> &lhs0, const AABB<N> &lhs1,
           const AABB<N> &rhs0, const AABB<N> &rhs1,
           Real &r0, Real &r1) {
    Vector<N> vL = lhs1.center() - lhs0.center();
    Vector<N> vR = rhs1.center() - rhs0.center();
    return Sweep(lhs0, vL, rhs0, vR, r0, r1);
}


//This is a collisions test. It checks for a possible collisions and gives
//the time of initial and final collisions as a ratio between 0 and 1. If
//there is no collision, the values of r0 and r1 are undefined.
//
// @param lhs    The initial position of one of the AABBs.
// @param vL    The velocity of the first AABB.
// @param rhs    The initial position of the other AABB.
// @param vR    The velocity of the second AABB.
// @param r0    Upon return, this is set to the time of initial collision.
// @param r1    Upon return, this is set to the time they stop colliding.
//
// @return     True if there is a collision, false otherwise.
template <int N>
bool AABB<N>::Sweep(const AABB<N> &lhs, const Vector<N> &vL,
                    const AABB<N> &rhs, const Vector<N> &vR,
                    Real &r0, Real &r1) {
    Vector<N> relative = vR - vL; //Hold L still
    Vector<N> r0Vec, r1Vec;
    r1Vec.clear(1.0f);

    r0 = 0; r1 = 1;
    if (lhs.overlaps(rhs)) { return true; }
    if (vR == vL) { return false; }
    for (int i = 0; i < N; i++) {
        bool possible = false;
        if (Math::lt(relative[i], 0)) {
            if (Math::lt(lhs.max(i), rhs.min(i))) {
                r0Vec[i] = (lhs.max(i) - rhs.min(i)) / relative[i];
                possible = true;
            }

            if (Math::lt(lhs.min(i), rhs.max(i))) {
                r1Vec[i] = (lhs.min(i) - rhs.max(i)) / relative[i];
                possible = true;
            }

        } else if (Math::gt(relative[i], 0))  {
            if (Math::gt(lhs.min(i), rhs.max(i))) {
                r0Vec[i] = (lhs.min(i) - rhs.max(i)) / relative[i];
                possible = true;
            }

            if (Math::gt(lhs.max(i), rhs.min(i))) {
                r1Vec[i] = (lhs.max(i) - rhs.min(i)) / relative[i];
                possible = true;
            }

        } else {
            if (Math::le(lhs.max(i), rhs.min(i))) { return false; }
            if (Math::ge(lhs.min(i), rhs.max(i))) { return false; }
            possible = true;
        }
        if (!possible) {
            return false;
        }
    }

    r0 = r0Vec[0];
    r1 = r1Vec[0];        
    for (int i = 1; i < N; i++) {
        if (r0Vec[i] > r0) { r0 = r0Vec[i]; }
        if (r1Vec[i] < r1) { r1 = r1Vec[i]; }
    }

    return r0 <= r1;
}

#endif /* _BASE_H */
