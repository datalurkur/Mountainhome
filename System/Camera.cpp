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
#include "Scene.h"
#include "RenderContext.h"

//TODO Add the ability to lock on to things
//TODO Add the FOV and Aspect ration information here, as well as any other
//view specific information.

Camera::Camera(): _parent(NULL) {}
Camera::Camera(Scene *parent): _parent(parent) {}
Camera::~Camera() {}

const PositionableObject* Camera::getParent() const {
    return NULL;
}

const Frustum& Camera::getFrustum() const { return _frustum; }
Vector3 Camera::getUpDirection() const { return _orientation * Vector3(0, 1, 0);  }
Vector3 Camera::getDirection()   const { return _orientation * Vector3(0, 0, -1); }

void Camera::lookAt(const Vector3 &pos) {
    setDirection(pos - _position);
}

void Camera::setDirection(const Vector3 &newDir) {
    //\TODO Need to handle fixed yaw axis case.
    rotate(Quaternion(getDirection(), newDir.getNormalized()));
} // setDirection

void Camera::resize(int width, int height) {
    _frustum.resize(width, height);
} // resize

void Camera::render(RenderContext *context) {
    Matrix view(_orientation.getInverse());
    view.setTranslation(-_position);

//Info("View matrix:\n" << view);
//Info("Result: " << view * Vector3(0, 0, 0));

Matrix temp_trans;
temp_trans.setTranslation(-_position);
Matrix temp_orien(_orientation.getInverse());

//Info("View matrix:\n" << temp_orien * temp_trans);
//Info("Result: " << temp_orien * temp_trans * Vector3(0, 0, 0));

    Info("Position: " << _position);
    Info("Direction: " << _orientation * Vector3(0, 0, -1));
//    Info("World  space position: " << Vector3(0, 0, 0));
//    Info("Camera space position: " << view * Vector3(0, 0, 0));

    context->resetModelviewMatrix();
    context->setProjectionMatrix(_frustum.getProjectionMatrix());
    context->setViewMatrix(temp_orien * temp_trans);
    _frustum.updateFrustum(temp_orien * temp_trans);
    _parent->render(context, this);
} // render

std::ostream& operator<<(std::ostream &lhs, const Camera &rhs) {
    lhs << "Camera" << std::endl;
    lhs << " Position " << rhs._position << std::endl;
    lhs << " Look Dir " << rhs._orientation * Vector3(0, 0, -1)   << std::endl;
    lhs << rhs._frustum;
    return lhs;
} // operator<<
