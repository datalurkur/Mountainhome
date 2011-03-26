/*
 *  Camera.cpp
 *  HL2-BSPReader
 *
 *  Created by Brent Wilson on 5/22/06.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "Camera.h"
#include "GL_Helper.h"
#include "Window.h"
#include "Mouse.h"
#include "Viewport.h"
#include "SceneManager.h"
#include "RenderContext.h"

const std::string Camera::TypeName = "Camera";

//TODO Add the ability to lock on to things

Camera::Camera(const std::string &name):
    SceneNode(name, TypeName), _projection(Matrix::Identity()) {}

Camera::Camera(const std::string &name, const std::string &type):
    SceneNode(name, type), _projection(Matrix::Identity()) {}

Camera::~Camera() {}

const Frustum & Camera::getFrustum() { return _frustum; }

void Camera::setProjectionMatrix(const Matrix &matrix) {
    _frustum.setProjectionMatrix(_projection = matrix);
}

const Matrix &Camera::getProjectionMatrix() {
    return _projection;
}

const Matrix & Camera::getViewMatrix() {
    return _view;
}

bool Camera::updateImplementationValues() {
    bool ret = SceneNode::updateImplementationValues();
    _view = _derivedTransform.getInverse();
    _frustum.setWorldMatrix(_view);
    return ret;
}

void Camera::createSelectionFrustum(const Vector2 &one, const Vector2 &two, Frustum &frustum) {
    ASSERT(one.x >= 0.0 && one.x <= 1.0 && one.y >= 0.0 && one.y <= 1.0);
    ASSERT(two.x >= 0.0 && two.x <= 1.0 && two.y >= 0.0 && two.y <= 1.0);

    // Make a copy of the camera's frustum.
    frustum.setPlane(Frustum::LEFT,   *_frustum.getPlane(Frustum::LEFT));
    frustum.setPlane(Frustum::RIGHT,  *_frustum.getPlane(Frustum::RIGHT));
    frustum.setPlane(Frustum::BOTTOM, *_frustum.getPlane(Frustum::BOTTOM));
    frustum.setPlane(Frustum::TOP,    *_frustum.getPlane(Frustum::TOP));
    frustum.setPlane(Frustum::NEAR,   *_frustum.getPlane(Frustum::NEAR));
    frustum.setPlane(Frustum::FAR,    *_frustum.getPlane(Frustum::FAR));

    // Scale based on the selected points.
    frustum.scaleFrustum(
        one.x, 1.0 - two.x,
        one.y, 1.0 - two.y);
}

void Camera::createProjectionVector(const Vector2 &screen, Vector3 &start, Vector3 &dir) {
    Matrix inverseVP = Matrix::Affine(_orientation, _position) * _projection.getInverse();

    // Translate the screen coordinates into normalized device coordinates.
    Real ndcX = screen.x * 2.0 - 1.0;
    Real ndcY = screen.y * 2.0 - 1.0;

    // Compute the ray start and end points. Use an end value midway through the NDC to
    // avoid problems with infinite projections.
    Vector3 rayStart = inverseVP * Vector3(ndcX, ndcY, -1.0);
    Vector3 rayEnd   = inverseVP * Vector3(ndcX, ndcY,  1.0);

    // Set the start point and direction vector
    start = rayStart;
    dir   = (rayEnd - rayStart);
    dir.normalize();
}

std::ostream& operator<<(std::ostream &lhs, const Camera &rhs) {
    lhs << "Camera" << std::endl;
    lhs << " Position  " << rhs.getDerivedPosition() << std::endl;
    lhs << " Direction " << rhs.getDirection() << std::endl;
    lhs << " Up        " << rhs.getUpDirection() << std::endl;
    lhs << " Right     " << rhs._orientation * Vector3(1, 0, 0) << std::endl;
    lhs << rhs._frustum;
    return lhs;
} // operator<<
