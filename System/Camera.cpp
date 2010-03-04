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

//TODO Add the ability to lock on to things
//TODO Add the FOV and Aspect ration information here, as well as any other
//view specific information.

Camera::Camera(): _parent(NULL) {}
Camera::Camera(SceneManager *parent): _parent(parent) {}
Camera::~Camera() {}

PositionableObject* Camera::getParent() const { return NULL; }
void Camera::updateImplementationValues() {}

const Frustum& Camera::getFrustum() const { return _frustum; }
Vector3 Camera::getUpDirection() const { return _orientation * Vector3(0, 1, 0);  }
Vector3 Camera::getDirection()   const { return _orientation * Vector3(0, 0, -1); }

void Camera::lookAt(const Vector3 &pos) {
    setDirection(pos - _position);
}

void Camera::setDirection(const Vector3 &newDir) {
#if 0
    Vector3 zAxis = (-newDir).getNormalized();
    Vector3 xAxis = zAxis.crossProduct(Vector3(0, 1, 0)).getNormalized();
    Vector3 yAxis = zAxis.crossProduct(xAxis);
    Info("BRENT\n" << Matrix(xAxis, yAxis, zAxis));
    _orientation = Quaternion(xAxis, yAxis, zAxis);
#else
    //\TODO Need to handle fixed yaw axis case.
    rotate(Quaternion(getDirection(), newDir.getNormalized()));
#endif

    ASSERT_EQ(newDir.getNormalized(), (_orientation * Vector3(0,0,-1)).getNormalized());
} // setDirection

void Camera::resize(int width, int height) {
    _frustum.resize(width, height);
} // resize

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
    _frustum.updateFrustum(temp_orien * temp_trans);
    _parent->render(context, this);
} // render

std::ostream& operator<<(std::ostream &lhs, const Camera &rhs) {
    lhs << "Camera" << std::endl;
    lhs << " Position  " << rhs.getPosition() << std::endl;
    lhs << " Direction " << rhs.getDirection() << std::endl;
    lhs << " Up        " << rhs.getUpDirection() << std::endl;
    lhs << " Right     " << rhs._orientation * Vector3(1, 0, 0) << std::endl;
    lhs << rhs._frustum;
    return lhs;
} // operator<<
