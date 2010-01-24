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
#include "RenderContext.h"

//TODO Add the ability to lock on to things
//TODO Add the FOV and Aspect ration information here, as well as any other
//view specific information.

#define MOUSE_SPEED 50.0f

Camera::Camera(): _up(0.0f, 1.0f, 0.0f), _lookAt(0.0f, 0.0f, -1.0f),
_position(0.0f, 0.0f, 0.0f) {}

void Camera::resize(int width, int height) {
    _frustum.resize(width, height);
}

Camera::~Camera() {}

void Camera::normalize() {
    _up.normalize();
    _lookAt.normalize();
}

void Camera::rotateViewRelative(Real deltaX, Real deltaY) {
    Vector3 verticalAxis = _lookAt.crossProduct(_up);
    verticalAxis.normalize();

    Matrix m = Matrix(deltaX, _up) * Matrix(deltaY, verticalAxis);
    rotateView(m);
}

void Camera::rotateView(Real deltaX, Real deltaY) {
    Vector3 verticalAxis = _lookAt.crossProduct(_up);
    verticalAxis.normalize();

    Matrix m = Matrix(deltaX, Vector3(0, 1, 0));
    Vector3 newUp = Matrix(deltaY, verticalAxis) * _up;
    if (newUp.y > 0) {
        m = m * Matrix(deltaY, verticalAxis);
    }

    rotateView(m);
}

void Camera::rotateView(const Matrix &m) {
    m.apply(_lookAt);
    m.apply(_up);
    normalize();
}

void Camera::rotateView(float degrees, const Vector3 &axis) {
    Matrix m(Math::Radians(degrees), axis);
    rotateView(m);
}


#include <GLUT/glut.h>
#include <OpenGL/gl.h>
void Camera::render(RenderContext *context) {
    context->resetModelviewMatrix();
    Vector3 temp = _position + _lookAt;
    gluLookAt(_position[0], _position[1], _position[2],
              temp[0],      temp[1],      temp[2],
              _up[0],       _up[1],       _up[2]);

    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
    _frustum.updateFrustum(Matrix(modelview));
    context->setProjectionMatrix(_frustum.getProjectionMatrix());



Info("Rendering scene");
///\FIXME TEST CODE
    glPushMatrix(); {
        glTranslatef(-10.0, 0.0, -10.0);
        glutSolidTeapot(1);
    } glPopMatrix();
    glPushMatrix(); {
        glTranslatef( 10, 0.0, -10);
        glutSolidTeapot(1);
    } glPopMatrix();
///\FIXME TEST CODE
} // Update

void Camera::moveForward(float dist) {
    _position += _lookAt * dist;
} // MoveForward

void Camera::moveUpward(float dist) {
    _position += _up * dist;
} // moveUpward

void Camera::strafeRight(float dist) {
    _position += _lookAt.crossProduct(_up).getNormalized() * dist;
} // StrafeRight

void Camera::adjustPitch(float angle) {
    rotateView(angle, Vector3(1, 0, 0));
} // RotateX

void Camera::adjustYaw(float angle) {
    rotateView(angle, Vector3(0, 1, 0));
} // RotateY

void Camera::adjustRoll(float angle) {
    rotateView(angle, Vector3(0, 0, 1));
} // RotateZ

const Frustum& Camera::getFrustum() const {
    return _frustum;
}

const Vector3& Camera::getPosition() const {
    return _position;
}

void Camera::setPosition(const Vector3 &pos) {
    _position = pos;
}

void Camera::lookAtPos(const Vector3 &pos) {
    Vector3 newLookAt((pos - _position).getNormalized());
    _lookAt.x = newLookAt.x;
    _lookAt.z = newLookAt.z;
    _lookAt.normalize();

    rotateView(Matrix(_lookAt, newLookAt));
}

void Camera::lookAtDir(const Vector3 &pos) {
    Vector3 newLookAt(pos.getNormalized());
    _lookAt.x = newLookAt.x;
    _lookAt.z = newLookAt.z;
    _lookAt.normalize();

    rotateView(Matrix(_lookAt, newLookAt));
}

std::ostream& operator<<(std::ostream &lhs, const Camera &rhs) {
    lhs << "Camera" << std::endl;
    lhs << " Position " << rhs._position << std::endl;
    lhs << " Look Dir " << rhs._lookAt   << std::endl;
    lhs << rhs._frustum;
    return lhs;
}
