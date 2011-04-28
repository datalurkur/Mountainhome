/*
 *  Material.cpp
 *  System
 *
 *  Created by loch on 1/6/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include <Base/FileSystem.h>
#include "Material.h"
#include "Shader.h"

#if DEBUG
// Ensures that pre/post render calls are not made inappropriately. This ensures
// pushParameters/popParameters will be properly paired.
static bool SafetyCheck_BetweenCalls = false;
#endif

Material::Material():
    _name(""),
    _shader(NULL)
{}

Material::Material(const std::string &name):
    _name(name),
    _shader(NULL)
{}

Material::~Material() {}

const std::string & Material::getName() {
    return _name;
}

void Material::setName(const std::string &name) {
    _name = name;
}

void Material::enable() {
#if DEBUG
    if (SafetyCheck_BetweenCalls) {
        THROW(InternalError, "Nested calls to Material::enable!!!");
    }

    SafetyCheck_BetweenCalls = true;
#endif // DEBUG

    // This needs to happen before we push parameters to the shader.
    if (_shader) {
        _shader->enable();
    } else {
        glUseProgram(0);
    }

    pushParameters(_shader);
}

void Material::disable() {
#if DEBUG
    if (!SafetyCheck_BetweenCalls) {
        THROW(InternalError, "Out of order call to Material::disable!!!");
    }

    SafetyCheck_BetweenCalls = false;
#endif // DEBUG

    popParameters();

    // Make sure to disable the shader to cleanup certain parameter state!
    if(_shader) {
        _shader->disable();
    }
}

void Material::setShader(Shader *shader) {
    _shader = shader;
}

Shader * Material::getShader() {
    return _shader;
}
