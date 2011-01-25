/*
 *  Frustum.cpp
 *  Render
 *
 *  Created by Brent Wilson on 4/8/06.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Frustum.h"
#include "Math3D.h"

Frustum::Frustum() {}

Frustum::~Frustum() {}

void Frustum::setProjectionMatrix(const Matrix &matrix) {
    _projectionMatrix = matrix;
    updatePlanes();
}

void Frustum::setWorldMatrix(const Matrix &matrix) {
    _worldMatrix = matrix;
    updatePlanes();
}

void Frustum::updatePlanes() {
    Matrix clipping = _projectionMatrix * _worldMatrix;

    extractLeft  (clipping);
    extractRight (clipping);
    extractBottom(clipping);
    extractTop   (clipping);
    extractNear  (clipping);
    extractFar   (clipping);

    normalize();
}

// Note that we divide the resulting distance by -1 because our Plane implementation
// specifies the distance as the distance from the origin in the direction of the plane's
// normal.

void Frustum::extractLeft(const Matrix &clipping) {
    const float *clip = clipping.getMatrix();
    _frustum[LEFT].setNormal(   clip[ 3] + clip[ 0],
                                clip[ 7] + clip[ 4],
                                clip[11] + clip[ 8]);
    _frustum[LEFT].setDistance((clip[15] + clip[12]) * -1);
}

void Frustum::extractRight(const Matrix &clipping) {
    const float *clip = clipping.getMatrix();
    _frustum[RIGHT].setNormal(   clip[ 3] - clip[ 0],
                                 clip[ 7] - clip[ 4],
                                 clip[11] - clip[ 8]);
    _frustum[RIGHT].setDistance((clip[15] - clip[12]) * -1);
}

void Frustum::extractBottom(const Matrix &clipping) {
    const float *clip = clipping.getMatrix();
    _frustum[BOTTOM].setNormal(   clip[ 3] + clip[ 1],
                                  clip[ 7] + clip[ 5],
                                  clip[11] + clip[ 9]);
    _frustum[BOTTOM].setDistance((clip[15] + clip[13]) * -1);
}

void Frustum::extractTop(const Matrix &clipping) {
    const float *clip = clipping.getMatrix();
    _frustum[TOP].setNormal(   clip[ 3] - clip[ 1],
                               clip[ 7] - clip[ 5],
                               clip[11] - clip[ 9]);
    _frustum[TOP].setDistance((clip[15] - clip[13]) * -1);
}

void Frustum::extractNear(const Matrix &clipping) {
    const float *clip = clipping.getMatrix();
    _frustum[NEAR].setNormal(   clip[ 3] + clip[ 2],
                                clip[ 7] + clip[ 6],
                                clip[11] + clip[10]);
    _frustum[NEAR].setDistance((clip[15] + clip[14]) * -1);
}

void Frustum::extractFar(const Matrix &clipping) {
    const float *clip = clipping.getMatrix();
    _frustum[FAR].setNormal(   clip[ 3] - clip[ 2],
                               clip[ 7] - clip[ 6],
                               clip[11] - clip[10]);
    _frustum[FAR].setDistance((clip[15] - clip[14]) * -1);
}

void Frustum::scaleFrustum(
    Real leftRatio,   Real rightRatio,
    Real bottomRatio, Real topRatio,
    Real nearRatio,   Real farRatio)
{
    Plane unscaledFrustum[6];
    for(int i = 0; i < 6; i++) { unscaledFrustum[i] = _frustum[i]; }

    // We invert normal and distance to make sure they're conceptually in the same space before lerping.
    #define SCALE(startIndex, endIndex, amt) do { \
        if ((amt) != 0) { \
            Vector3 normal = unscaledFrustum[startIndex].getNormal(); \
            normal.lerp(-unscaledFrustum[endIndex].getNormal(), (amt)); \
            normal.normalize(); \
            \
            Real distance = Math::Lerp( \
                 unscaledFrustum[startIndex].getDistance(), \
                -unscaledFrustum[  endIndex].getDistance(), \
                (amt)); \
            \
            _frustum[startIndex].setDistance(distance); \
            _frustum[startIndex].setNormal(normal); \
        } \
    } while(0)

    SCALE(LEFT,   RIGHT,  leftRatio);
    SCALE(RIGHT,  LEFT,   rightRatio);
    SCALE(BOTTOM, TOP,    bottomRatio);
    SCALE(TOP,    BOTTOM, topRatio);
    SCALE(NEAR,   FAR,    nearRatio);
    SCALE(FAR,    NEAR,   farRatio);

    #undef SCALE
}

Plane* Frustum::getPlane(Sides side) {
    return _frustum + side;
}

void Frustum::setPlane(Sides side, const Plane &plane) {
    _frustum[side].setDistance(plane.getDistance());
    _frustum[side].setNormal(plane.getNormal());
}

void Frustum::normalize() {
    for (int i = 0; i < 6; i++) {
        _frustum[i].normalize();
    }
}

bool Frustum::checkPoint(const Vector3 &point) const {
    for (int k = 0; k < 6; k++) {
        if (_frustum[k].distanceFrom(point) <= 0) {
            return false;
        }
    }
    return true;
}

Frustum::Collision Frustum::checkSphere(const Vector3 &center, float radius) const {
    for (int k = 0; k < 6; k++) {
        Real dist = _frustum[k].distanceFrom(center);
        if (dist < -radius) { return COMPLETE_OUT; }
        if (dist >  radius) { continue; }
        return INTERSECT;
    }

    return COMPLETE_IN;
}

Frustum::Collision Frustum::checkAABB(const AABB3 &box) const {
    return checkAABB(box.getMin(), box.getMax());
}

#define VERTEX_IN  1
#define VERTEX_OUT 2
#define VERTEX_INT 3
#define checkAABBPoint(a) \
    mode |= _frustum[i].distanceFrom(a) >= 0 ? VERTEX_IN : VERTEX_OUT; \
    if (mode == VERTEX_INT) { continue; }
Frustum::Collision Frustum::checkAABB(const Vector3 &min, const Vector3 &max) const {
    const Vector3 &lbf(min);
    const Vector3  lbn(min[0], min[1], max[2]);
    const Vector3  ltf(min[0], max[1], min[2]);
    const Vector3  ltn(min[0], max[1], max[2]);
    const Vector3  rbf(max[0], min[1], min[2]);
    const Vector3  rbn(max[0], min[1], max[2]);
    const Vector3  rtf(max[0], max[1], min[2]);
    const Vector3 &rtn(max);

    unsigned char mode = 0;
    for (int i = 0; i < 6; i++) {
        mode &= VERTEX_OUT;
        checkAABBPoint(lbf);
        checkAABBPoint(lbn);
        checkAABBPoint(ltf);
        checkAABBPoint(ltn);
        checkAABBPoint(rbf);
        checkAABBPoint(rbn);
        checkAABBPoint(rtf);
        checkAABBPoint(rtn);
        if (mode == VERTEX_OUT) {
            return COMPLETE_OUT;
        }
    }

    return mode == VERTEX_INT ? INTERSECT : COMPLETE_IN;      
}

std::ostream& operator<<(std::ostream &lhs, const Frustum &rhs) {
    lhs << "Frustum" << std::endl;
    lhs << "  Left   " << rhs._frustum[Frustum::LEFT  ] << std::endl;
    lhs << "  Right  " << rhs._frustum[Frustum::RIGHT ] << std::endl;
    lhs << "  Bottom " << rhs._frustum[Frustum::BOTTOM] << std::endl;
    lhs << "  Top    " << rhs._frustum[Frustum::TOP   ] << std::endl;
    lhs << "  Near   " << rhs._frustum[Frustum::NEAR  ] << std::endl;
    lhs << "  Far    " << rhs._frustum[Frustum::FAR   ] << std::endl;
    return lhs;
}
