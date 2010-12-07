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
//TODO Add the FOV and Aspect ration information here, as well as any other
//view specific information.

Camera::Camera(const std::string &name, SceneManager *parent):
    SceneNode(name, TypeName), _parent(parent) {}

Camera::~Camera() {}

ViewFrustum* Camera::getFrustum() { return &_frustum; }

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
    Matrix inverseMVP = Matrix::Affine(_orientation, _position) * _frustum.getProjectionMatrix().getInverse();

    // Translate the screen coordinates into normalized device coordinates.
    Real ndcX = screen.x * 2.0 - 1.0;
    Real ndcY = screen.y * 2.0 - 1.0;

    // Compute the ray start and end points. Use an end value midway through the NDC to
    // avoid problems with infinite projections.
    Vector3 rayStart = inverseMVP * Vector3(ndcX, ndcY, -1.0);
    Vector3 rayEnd   = inverseMVP * Vector3(ndcX, ndcY,  1.0);

    // Set the start point and direction vector
    start = rayStart;
    dir   = (rayEnd - rayStart);
    dir.normalize();
}

void Camera::resize(int width, int height) {
    _frustum.resize(width, height);
}

void Camera::render(RenderContext *context) {
    // The view matrix is the inverse of the camera's affine transformation.
    Matrix viewMatrix = Matrix::InverseAffine(_orientation, _position);

    _frustum.setTransformation(viewMatrix);

    context->setProjectionMatrix(_frustum.getProjectionMatrix());
    context->setModelMatrix(Matrix::Identity());
    context->setViewMatrix(viewMatrix);

    _parent->render(context, this);
} // render

std::ostream& operator<<(std::ostream &lhs, const Camera &rhs) {
    lhs << "Camera" << std::endl;
    lhs << " Position  " << rhs.getDerivedPosition() << std::endl;
    lhs << " Direction " << rhs.getDirection() << std::endl;
    lhs << " Up        " << rhs.getUpDirection() << std::endl;
    lhs << " Right     " << rhs._orientation * Vector3(1, 0, 0) << std::endl;
    lhs << rhs._frustum;
    return lhs;
} // operator<<
