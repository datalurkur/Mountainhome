/*
 *  LambertMaterial.cpp
 *  Mountainhome
 *
 *  Created by loch on 1/20/11.
 *  Copyright 2011 Mountainhome Project. All rights reserved.
 *
 */

#include "Content.h"
#include "LambertMaterial.h"

LambertMaterial::LambertMaterial() {
    init();
}

LambertMaterial::LambertMaterial(const Vector4 &ambient, const Vector4 &diffuse) {
    init();
    setAmbient(ambient);
    setDiffuse(diffuse);
}

void LambertMaterial::init() {
    setShader(Content::Get<Shader>("lambert"));

    // Point the color parameters at our local variables. We only need to do this once.
    setShaderParameter("ambient", &_ambient);
    setShaderParameter("diffuse", &_diffuse);
}

void LambertMaterial::setAmbient(const Vector4 &ambient) {
    _ambient = ambient;
}

const Vector4 & LambertMaterial::getAmbient() {
    return _ambient;
}

void LambertMaterial::setDiffuse(const Vector4 &diffuse) {
    _diffuse = diffuse;
}

const Vector4 & LambertMaterial::getDiffuse() {
    return _diffuse;
}
