/*
 *  ViewFrustum.cpp
 *  Mountainhome
 *
 *  Created by loch on 10/16/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ViewFrustum.h"

ViewFrustum::ViewFrustum(): _projectionType(PERSPECTIVE), _fov(45.0), _ratio(4.0/3.0),
_near(1.0), _far(1000.0), _left(0.0), _right(1.0), _bottom(0.0), _top(1.0),
_valid(false) {}

ViewFrustum::~ViewFrustum() {}

Real ViewFrustum::getRatio() {
    return _ratio;
}

void ViewFrustum::setTransformation(const Matrix &transformation) {
    _transormationMatrix = transformation;
    updatePlanes();
}

void ViewFrustum::updatePlanes() {
    Matrix clipping = _projectionMatrix * _transormationMatrix;

    extractLeft  (clipping);
    extractRight (clipping);
    extractBottom(clipping);
    extractTop   (clipping);
    extractNear  (clipping);
    extractFar   (clipping);

    normalize();
}

void ViewFrustum::updateProjectionMatrix() {
    if (_projectionType == PERSPECTIVE) {
        Real f = Math::Cot(_fov / 2.0);
        _projectionMatrix(0, 0) = f / _ratio;
        _projectionMatrix(1, 1) = f;
        _projectionMatrix(2, 2) = (_far + _near) / (_near - _far);
        _projectionMatrix(2, 3) = (2 * _far * _near) / (_near - _far);
        _projectionMatrix(3, 2) = -1;
        _projectionMatrix(3, 3) = 0;
    } else {
        _projectionMatrix(0, 0) =  2.0f / (_right - _left);
        _projectionMatrix(1, 1) =  2.0f / (_top - _bottom);
        _projectionMatrix(2, 2) = -2.0f / (_far - _near);
        _projectionMatrix(0, 3) = -(_right + _left)   / (_right - _left);
        _projectionMatrix(1, 3) = -(_top   + _bottom) / (_top   - _bottom);
        _projectionMatrix(2, 3) = -(_far   + _near)   / (_far   - _near);
    }

    updatePlanes();
}

void ViewFrustum::resize(int width, int height) {
    _ratio = static_cast<Real>(width);
    if (height > 0) { _ratio /= static_cast<Real>(height); }
    updateProjectionMatrix();
}

void ViewFrustum::makePerspective(Radian fov, Real n, Real f) {
    _projectionType = PERSPECTIVE;

    _fov = fov;
    _near = n;
    _far = f;

    updateProjectionMatrix();
}

void ViewFrustum::makePerspective(int width, int height, Radian fov, Real n, Real f) {
    resize(width, height);
    makePerspective(fov, n, f);
}

void ViewFrustum::makeOrtho(Real left, Real right, Real bottom,
                            Real top, Real n, Real f) {
    _projectionType = ORTHOGRAPHIC;

    _left = left;
    _right = right;
    _bottom = bottom;
    _top = top;
    _near = n;
    _far = f;

    updateProjectionMatrix();
}

void ViewFrustum::centerOrtho(Real width, const Vector2 &center, Real near, Real far) {
    Real height = width / _ratio;
    makeOrtho(center[0] - (width / 2.0), center[0] + (width / 2.0), 
              center[1] - (height/2.0),  center[1] + (height/2.0), 
              near, far);
}

const Matrix& ViewFrustum::getProjectionMatrix() {
    return _projectionMatrix;
}

// Note that we divide the resulting distance by zero because our Plane implementation
// specifies the distance as the distance from the origin in the direction of the plane's
// normal.

void ViewFrustum::extractLeft(const Matrix &clipping) {
    const float *clip = clipping.getMatrix();
    _frustum[LEFT].setNormal(   clip[ 3] + clip[ 0],
                                clip[ 7] + clip[ 4],
                                clip[11] + clip[ 8]);
    _frustum[LEFT].setDistance((clip[15] + clip[12]) * -1);
}

void ViewFrustum::extractRight(const Matrix &clipping) {
    const float *clip = clipping.getMatrix();
    _frustum[RIGHT].setNormal(   clip[ 3] - clip[ 0],
                                 clip[ 7] - clip[ 4],
                                 clip[11] - clip[ 8]);
    _frustum[RIGHT].setDistance((clip[15] - clip[12]) * -1);
}

void ViewFrustum::extractBottom(const Matrix &clipping) {
    const float *clip = clipping.getMatrix();
    _frustum[BOTTOM].setNormal(   clip[ 3] + clip[ 1],
                                  clip[ 7] + clip[ 5],
                                  clip[11] + clip[ 9]);
    _frustum[BOTTOM].setDistance((clip[15] + clip[13]) * -1);
}

void ViewFrustum::extractTop(const Matrix &clipping) {
    const float *clip = clipping.getMatrix();
    _frustum[TOP].setNormal(   clip[ 3] - clip[ 1],
                               clip[ 7] - clip[ 5],
                               clip[11] - clip[ 9]);
    _frustum[TOP].setDistance((clip[15] - clip[13]) * -1);
}

void ViewFrustum::extractNear(const Matrix &clipping) {
    const float *clip = clipping.getMatrix();
    _frustum[NEAR].setNormal(   clip[ 3] + clip[ 2],
                                clip[ 7] + clip[ 6],
                                clip[11] + clip[10]);
    _frustum[NEAR].setDistance((clip[15] + clip[14]) * -1);
}

void ViewFrustum::extractFar(const Matrix &clipping) {
    const float *clip = clipping.getMatrix();
    _frustum[FAR].setNormal(   clip[ 3] - clip[ 2],
                               clip[ 7] - clip[ 6],
                               clip[11] - clip[10]);
    _frustum[FAR].setDistance((clip[15] - clip[14]) * -1);
}

std::ostream& operator<<(std::ostream &lhs, const ViewFrustum &rhs) {
    lhs << "ViewFrustum (";
    if (rhs._projectionType == ViewFrustum::PERSPECTIVE) {
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

