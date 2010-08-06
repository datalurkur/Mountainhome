/*
 *  Frustum.cpp
 *  Render
 *
 *  Created by Brent Wilson on 4/8/06.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Frustum.h"
#include <math.h>

Frustum::Frustum(): _projectionType(PERSPECTIVE), _fov(45.0), _ratio(4.0/3.0),
_near(1.0), _far(1000.0), _left(0.0), _right(1.0), _bottom(0.0), _top(1.0),
_valid(false) {}

Frustum::~Frustum() {}

Real Frustum::getRatio() {
    return _ratio;
}

void Frustum::updateFrustum(const Matrix &modelview) {
    if (!_valid) {
        if (_projectionType == PERSPECTIVE) {
            updatePerspective();
        } else {
            updateOrtho();
        }

        _valid = true;
    }

    Matrix clipping = _projectionMatrix * modelview;
    extractLeft(clipping);
    extractRight(clipping);
    extractBottom(clipping);
    extractTop(clipping);
    extractNear(clipping);
    extractFar(clipping);
    normalize();
}

void Frustum::updatePerspective() {
    Real f = Math::Cot(_fov / 2.0);
    _projectionMatrix(0, 0) = f / _ratio;
    _projectionMatrix(1, 1) = f;
    _projectionMatrix(2, 2) = (_far + _near) / (_near - _far);
    _projectionMatrix(2, 3) = (2 * _far * _near) / (_near - _far);
    _projectionMatrix(3, 2) = -1;
    _projectionMatrix(3, 3) = 0;
}

void Frustum::updateOrtho() {
    _projectionMatrix(0, 0) = 2.0f / (_right - _left);
    _projectionMatrix(1, 1) = 2.0f / (_top - _bottom);
    _projectionMatrix(2, 2) = 2.0f / (_near - _far);
    _projectionMatrix(0, 3) = -(_right + _left) / (_right - _left);
    _projectionMatrix(1, 3) = -(_top + _bottom) / (_top - _bottom);
    _projectionMatrix(2, 3) = -(_far + _near) / (_far - _near);
}

void Frustum::resize(int width, int height) {
    _ratio = static_cast<Real>(width);
    if (height > 0) { _ratio /= static_cast<Real>(height); }
    _valid = false;
}

void Frustum::makePerspective(Radian fov, Real n, Real f) {
    _fov = fov;
    _near = n;
    _far = f;

    _valid = false;
    _projectionType = PERSPECTIVE;    
}

void Frustum::makePerspective(int width, int height, Radian fov, Real n, Real f) {
    resize(width, height);
    makePerspective(fov, n, f);
}

void Frustum::makeOrtho2D(Real left, Real right, Real bottom, Real top) {
    makeOrtho(left, right, bottom, top, 1, -1);
}

void Frustum::makeOrtho(Real left, Real right, Real bottom,
                       Real top, Real n, Real f) {
    _left = left;
    _right = right;
    _bottom = bottom;
    _top = top;
    _near = n;
    _far = f;

    _valid = false;
    _projectionType = ORTHOGRAPHIC;
}

void Frustum::centerOrtho(Real width, const Vector2 &center, Real near, Real far) {
    Real height = width / _ratio;
    makeOrtho(center[0] - (width / 2.0), center[0] + (width / 2.0), 
              center[1] - (height/2.0),  center[1] + (height/2.0), 
              near, far);
}

const Matrix& Frustum::getProjectionMatrix() {
    return _projectionMatrix;
}

void Frustum::normalize() {
    for (int i = 0; i < 6; i++) {
        _frustum[i].normalize();
    }
}

// FIXME Fixed a bug in the plane code where the distance was being calculated
// incorrectly. That change made me invert all of the distances here to get things working
// again. I'd like to revisit this math and verify everything again.

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
    lhs << "Frustum (";
    if (rhs._projectionType == Frustum::PERSPECTIVE) {
        lhs << "Perspective) -";
        lhs << " FOV: "   << rhs._fov.valueRadians();
        lhs << " Ratio: " << rhs._ratio;
    } else {
        lhs << "Orthographic) -";
        lhs << " Left: "   << rhs._left;
        lhs << " Right: "  << rhs._right;
        lhs << " Bottom: " << rhs._bottom;
        lhs << " Top: "    << rhs._top;
    }

    lhs << " Near: " << rhs._near;
    lhs << " Far: "  << rhs._far << std::endl;

    lhs << "  Left   " << rhs._frustum[Frustum::LEFT  ] << std::endl;
    lhs << "  Right  " << rhs._frustum[Frustum::RIGHT ] << std::endl;
    lhs << "  Bottom " << rhs._frustum[Frustum::BOTTOM] << std::endl;
    lhs << "  Top    " << rhs._frustum[Frustum::TOP   ] << std::endl;
    lhs << "  Near   " << rhs._frustum[Frustum::NEAR  ] << std::endl;
    lhs << "  Far    " << rhs._frustum[Frustum::FAR   ] << std::endl;
    return lhs;
}
