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
Vector3 Camera::getUpDirection() const { return _orientation * Vector3(0, 1, 0);  }
Vector3 Camera::getDirection()   const { return _orientation * Vector3(0, 0, -1); }

void Camera::lookAt(const Vector3 &pos) {
    setDirection(pos - _position);
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
        -one.x, two.x - 1,
        -one.y, two.y - 1);
}

void Camera::setDirection(const Vector3 &newDir) {
    if (_fixedYawAxis) {
        Vector3 zAxis = -newDir.getNormalized();
        Vector3 xAxis = _yawAxis.crossProduct(zAxis).getNormalized();
        Vector3 yAxis = zAxis.crossProduct(xAxis);
        _orientation = Quaternion(xAxis, yAxis, zAxis);
    } else {
        rotate(Quaternion(getDirection(), newDir.getNormalized()));
    }

    ASSERT_EQ(newDir.getNormalized(), (_orientation * Vector3(0,0,-1)).getNormalized());
} // setDirection

void Camera::resize(int width, int height) {
    _frustum.resize(width, height);
}

void Camera::render(RenderContext *context) {
    // Generate an affine transformation matrix representing the inverse of the camera's
    // position and orientation. Do this by getting the inverse position and orientations
    // and applying them in reverse order.
    Matrix temp_trans;
    temp_trans.setTranslation(-_position);
    Matrix temp_orien(_orientation.getInverse());

    // And setup out context state.
    context->resetModelviewMatrix();
    context->setProjectionMatrix(_frustum.getProjectionMatrix());
    context->setViewMatrix(temp_orien * temp_trans);
    _frustum.setTranslation(temp_orien * temp_trans);
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
