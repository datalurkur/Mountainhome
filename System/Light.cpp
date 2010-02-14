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

Light::Light(): _enabled(true), _ambient(0,0,0,1), _diffuse(1,1,1,1), _specular(1,1,1,1) {}

void Light::setPosition(Real x, Real y, Real z) {
	_position.x = x;
	_position.y = y;
	_position.z = z;
}

void Light::setPosition(const Vector3 &vec) {
	_position = vec;
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

const Vector3& Light::getPosition() { 
	return _position; 
}

void Light::enable()  { _enabled = true;  }
void Light::disable() { _enabled = false; }

void Light::setupState(int index) {
    if (_enabled) {
        ///\todo Move this to RenderContext, where it belongs.
        GLenum lightIndex = GL_LIGHT0 + index;
        glEnable(lightIndex);
        glEnable(GL_LIGHTING);
        glLightfv(lightIndex, GL_POSITION, _position.ptr());
        glLightfv(lightIndex, GL_AMBIENT,  _ambient.ptr());
        glLightfv(lightIndex, GL_DIFFUSE,  _diffuse.ptr());
        glLightfv(lightIndex, GL_SPECULAR, _specular.ptr());
    } else {
        glDisable(GL_LIGHT0 + index);
    }
}
