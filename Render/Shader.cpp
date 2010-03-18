/*
 *  Shader.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include <fstream>
#include "ShaderManager.h"
#include "Shader.h"

/// \todo I can't use near or far on windows???

Shader::Shader():_vertexShader(NULL), _fragmentShader(NULL), _programHandle(NULL) {}

Shader::~Shader() {
    unload();
}

Shader* Shader::Load(const std::string &vertFilename, const std::string &fragFilename) {
    return ShaderManager::Get()->getOrLoadResource(vertFilename, fragFilename);
}

void Shader::setVertex(GLhandleARB program) {
    _vertexShader = program;
}

void Shader::setFragment(GLhandleARB program) {
    _fragmentShader = program;
}

void Shader::setProgram(GLhandleARB program) {
    _programHandle = program;
}

bool Shader::hasVertexShader() {
    return _vertexShader != 0;
}

bool Shader::hasFragmentShader() {
    return _fragmentShader != 0;
}

void Shader::unload() {
    if(_vertexShader) {
        glDetachObjectARB(_programHandle, _vertexShader);
        glDeleteObjectARB(_vertexShader); _vertexShader = NULL;
    }
    
    if(_fragmentShader) {
        glDetachObjectARB(_programHandle, _fragmentShader);
        glDeleteObjectARB(_fragmentShader); _fragmentShader = NULL;
    }
    
    if(_programHandle) {
        glDeleteObjectARB(_programHandle); _programHandle = NULL;
    }
}

void Shader::on() {
    glUseProgramObjectARB(_programHandle);
}

void Shader::off() {
    glUseProgramObjectARB(0);
}


int Shader::getVariable(const std::string &var) const {
    int id = -1;
    if(_programHandle) {
        id = glGetUniformLocationARB(_programHandle, var.c_str());
    }

    if (id == -1) {
        Error("Error getting shader variable, ID == -1");
    }

    return id;
}

void Shader::setTexture(const std::string &strVariable, GLint newValue) {
    setInt(strVariable, newValue);
}

void Shader::setInt(const std::string &name, GLint newValue) {
    glUniform1iARB(getVariable(name), newValue);
}

void Shader::setIntArray(const std::string &name, GLint size, GLint *newValues) {
    glUniform1ivARB(getVariable(name), size, newValues);
}

void Shader::setFloat(const std::string &name, float newValue) {
    glUniform1fARB(getVariable(name), newValue);
}

void Shader::setFloatArray(const std::string &name, GLint size, float *newValues) {
    glUniform1fvARB(getVariable(name), size, newValues);
}

/*
 void Shader::setFloat2(GLint var, float v0, float v1) {
     glUniform2fARB(var, v0, v1);
 }
 
 void Shader::setFloat3(GLint var, float v0, float v1, float v2) {
     glUniform3fARB(var, v0, v1, v2);
 }
 
 void Shader::setFloat4(GLint var, float v0, float v1, float v2, float v3) {
     glUniform4fARB(var, v0, v1, v2, v3);
 }*/
