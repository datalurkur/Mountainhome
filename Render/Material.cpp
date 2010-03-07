/*
 *  Material.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "Material.h"

Material::Material() {
	_texture = NULL;
	_color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Material::setColor(Real r, Real g, Real b, Real a) {
	_color.r = r;
	_color.g = g;
	_color.b = b;
	_color.a = a;
}

void Material::setAmbient(Real r, Real g, Real b, Real a) {
	_ambient.r = r;
	_ambient.g = g;
	_ambient.g = b;
}

void Material::setDiffuse(Real r, Real g, Real b, Real a) {
	_diffuse.r = r;
	_diffuse.g = g;
	_diffuse.b = b;
}

void Material::setTexture(Texture *t) {
	_texture = t;
}

Texture* Material::getTexture() const {
	return _texture;
}

Vector4* Material::getAmbient() const {
	return (Vector4*)&_ambient;
}

Vector4* Material::getDiffuse() const {
	return (Vector4*)&_diffuse;
}

Vector4* Material::getColor() const {
	return (Vector4*)&_color;
}
