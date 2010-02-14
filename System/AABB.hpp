/*
 *  AABB.hpp
 *  System
 *
 *  Created by loch on 2/7/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#ifndef _AABB_HPP_
#define _AABB_HPP_

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Initialization and destruction
//////////////////////////////////////////////////////////////////////////////////////////
template <int N>
AABB<N>::AABB() {}

template <int N>
AABB<N>::AABB(const AABB<N> &copy) {
    _center = copy.getCenter();
    _radius = copy.getRadius();
}

template <int N>
AABB<N>::AABB(const Vector<N> &center, const Vector<N> &radius)
:_center(center), _radius(radius) {
    _radius.absolute();
}

template <int N>
AABB<N>::~AABB() {}

template <int N>
AABB<N>& AABB<N>::operator=(const AABB<N> &rhs) {
    _center = rhs.getCenter();
    _radius = rhs.getRadius();
    return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Basic accessors
//////////////////////////////////////////////////////////////////////////////////////////
template <int N>
const Vector<N>& AABB<N>::getCenter() const {
    return _center;
}

template <int N>
const Vector<N>& AABB<N>::getRadius() const {
    return _radius;
}

template <int N>
Vector<N> AABB<N>::getDiameter() const {
    return _radius * 2;
}

template <int N>
Vector<N> AABB<N>::getMax() const {
    return getCenter() + getRadius();
}

template <int N>
Vector<N> AABB<N>::getMin() const {
    return getCenter() - getRadius();
}

template <int N>
void AABB<N>::setMinMax(const Vector<N> &min, const Vector<N> &max) {
    setRadius((max - min) / 2.0);
    setCenter(min + getRadius());
}

template <int N>
void AABB<N>::setRadius(Vector<N> radius) {
    _radius = radius;
    _radius.absolute();
}

template <int N>
void AABB<N>::setCenter(Vector<N> center) {
    _center = center;
}

//////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Functions
//////////////////////////////////////////////////////////////////////////////////////////
template <int N>
void AABB<N>::encompass(const AABB<N> &rhs) {
    setMinMax(
        getMin().getMinimum(rhs.getMin()),
        getMax().getMaximum(rhs.getMax()));
}

template <int N>
bool AABB<N>::overlapping(const AABB<N> &rhs, bool includeEdges) const {
    Vector<N> minDist = getRadius() + rhs.getRadius();
    Vector<N> dist = (getCenter() - rhs.getCenter()).getAbsolute();
    for (int i = 0; i < N; i++) {
        if (includeEdges) {
            if (Math::gt(dist[i], minDist[i])) { return false; }
        } else {
            if (Math::ge(dist[i], minDist[i])) { return false; }
        }
    }

    return true;
}

template <int N>
bool AABB<N>::contains(const Vector<N> &point, bool includeEdges) const {
    Vector<N> dist = (getCenter() - point);
    dist.absolute();
    for (int i = 0; i < N; i++) {
        if (includeEdges) {
            if (Math::gt(dist[i], getRadius()[i])) { return false; }
        } else {
            if (Math::ge(dist[i], getRadius()[i])) { return false; }
        }
    }

    return true;
}

template <int N>
bool AABB<N>::Sweep(const AABB<N> &lhs0, const AABB<N> &lhs1,
           const AABB<N> &rhs0, const AABB<N> &rhs1,
           Real &r0, Real &r1) {
    Vector<N> vL = lhs1.getCenter() - lhs0.getCenter();
    Vector<N> vR = rhs1.getCenter() - rhs0.getCenter();
    return Sweep(lhs0, vL, rhs0, vR, r0, r1);
}

template <int N>
bool AABB<N>::Sweep(const AABB<N> &lhs, const Vector<N> &vL,
                    const AABB<N> &rhs, const Vector<N> &vR,
                    Real &r0, Real &r1) {
    Vector<N> relative = vR - vL; //Hold L still
    Vector<N> r0Vec, r1Vec;
    r1Vec.clear(1.0f);

    r0 = 0; r1 = 1;
    if (lhs.overlapping(rhs)) { return true; }
    if (vR == vL) { return false; }
    for (int i = 0; i < N; i++) {
        bool possible = false;
        if (Math::lt(relative[i], 0)) {
            if (Math::lt(lhs.getMax()[i], rhs.getMin()[i])) {
                r0Vec[i] = (lhs.getMax()[i] - rhs.getMin()[i]) / relative[i];
                possible = true;
            }

            if (Math::lt(lhs.getMin()[i], rhs.getMax()[i])) {
                r1Vec[i] = (lhs.getMin()[i] - rhs.getMax()[i]) / relative[i];
                possible = true;
            }

        } else if (Math::gt(relative[i], 0))  {
            if (Math::gt(lhs.getMin()[i], rhs.getMax()[i])) {
                r0Vec[i] = (lhs.getMin()[i] - rhs.getMax()[i]) / relative[i];
                possible = true;
            }

            if (Math::gt(lhs.getMax()[i], rhs.getMin()[i])) {
                r1Vec[i] = (lhs.getMax()[i] - rhs.getMin()[i]) / relative[i];
                possible = true;
            }

        } else {
            if (Math::le(lhs.getMax()[i], rhs.getMin()[i])) { return false; }
            if (Math::ge(lhs.getMin()[i], rhs.getMax()[i])) { return false; }
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

#endif
