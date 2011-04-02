/*
 *  Light.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "GL_Helper.h"
#include "Light.h"

Light::Light(): _enabledOn(-1), _position(0.0), _ambient(0,0,0,1), _diffuse(1.0), _specular(1.0) {}

Light::~Light() {}

void Light::makePositionalLight(Real x, Real y, Real z) {
    setPosition(x, y, z, 1);
}

void Light::makePositionalLight(const Vector3 &vec) {
    makePositionalLight(vec.x, vec.y, vec.z);
}

void Light::makeDirectionalLight(Real x, Real y, Real z) {
    makeDirectionalLight(Vector3(x, y, z));
}

void Light::makeDirectionalLight(const Vector3 &vec) {
    Vector3 base = vec.getNormalized();
    setPosition(base.x, base.y, base.z, 0);
}

void Light::setPosition(Real x, Real y, Real z, Real w) {
	_position = Vector4(x, y, z, w);
}

void Light::setAmbient(Real r, Real g, Real b, Real a) { 
	_ambient.r = r;
	_ambient.g = g;
	_ambient.b = b;
	_ambient.a = a;
}

void Light::setDiffuse(Real r, Real g, Real b, Real a) { 
	_diffuse.r = r;
	_diffuse.g = g;
	_diffuse.b = b;
	_diffuse.a = a;
}

void Light::setSpecular(Real r, Real g, Real b, Real a) {
	_specular.r = r;
	_specular.g = g;
	_specular.b = b;
	_specular.a = a;
}

void Light::enable(int index)  {
    if (_enabledOn == -1) {
        _enabledOn = index;
        glEnable( GL_LIGHT0 + _enabledOn);
        glLightfv(GL_LIGHT0 + _enabledOn, GL_POSITION, _position.ptr());
        glLightfv(GL_LIGHT0 + _enabledOn, GL_AMBIENT,  _ambient.ptr());
        glLightfv(GL_LIGHT0 + _enabledOn, GL_DIFFUSE,  _diffuse.ptr());
        glLightfv(GL_LIGHT0 + _enabledOn, GL_SPECULAR, _specular.ptr());
    } else {
        THROW(InternalError, "Attempting to enable a Light that is already enabled.");
    }
}
void Light::disable() {
    if (_enabledOn >= 0) {
        glDisable(GL_LIGHT0 + _enabledOn);
        _enabledOn = -1;
    } else {
        THROW(InternalError, "Attempting to disable a Light that is not enabled.");
    }
}
