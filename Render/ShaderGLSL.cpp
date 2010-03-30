/*
 *  ShaderGLSL.cpp
 *  Mountainhome
 *
 *  Created by loch on 3/21/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include "ShaderGLSL.h"

ShaderGLSL::Factory::Factory(ResourceGroupManager *manager): PTreeResourceFactory<Shader>(manager) {}
ShaderGLSL::Factory::~Factory() {}

bool ShaderGLSL::Factory::canLoad(const std::string &args) {
    return true;
}

Shader* ShaderGLSL::Factory::load(const std::string &args) {
    return new ShaderGLSL(getPathFromKey("vertex"), getPathFromKey("fragment"));;
}

ShaderGLSL::ShaderGLSL(const std::string &vertString, const std::string &fragString) {
    bool customVertShader = vertString.length() > 0;
    bool customFragShader = fragString.length() > 0;
    GLhandleARB programHandle = NULL;
    GLhandleARB fragmentShader = NULL;
    GLhandleARB vertexShader = NULL;
    GLint compiled, linked;
    GLcharARB errors[1024];
    GLsizei length;

    if (customVertShader) {
        const char *vertChars = vertString.c_str();
        vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        setVertex(vertexShader);

        glShaderSourceARB(vertexShader, 1, &vertChars, NULL);
        glCompileShaderARB(vertexShader);
        glGetObjectParameterivARB(vertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
        if (!compiled) {
            glGetInfoLogARB(vertexShader, 1024, &length, errors);
            THROW(InternalError, "Error compiling vertex shader: " << errors);
        }
    }

    if (customFragShader) {
        const char *fragChars = fragString.c_str();
        fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
        setFragment(fragmentShader);

        glShaderSourceARB(fragmentShader, 1, &fragChars, NULL);
        glCompileShaderARB(fragmentShader);
        glGetObjectParameterivARB(fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
        if (!compiled) {
            glGetInfoLogARB(fragmentShader, 1024, &length, errors);
            THROW(InternalError, "Error compiling fragment shader: " << errors);
        }
    }

    programHandle = glCreateProgramObjectARB();
    setProgram(programHandle);

    if (customVertShader) { glAttachObjectARB(programHandle, vertexShader); }
    if (customFragShader) { glAttachObjectARB(programHandle, fragmentShader); }
    glLinkProgramARB(programHandle);
    
    glGetObjectParameterivARB(programHandle, GL_OBJECT_LINK_STATUS_ARB, &linked);
    if (!linked) {
        glGetInfoLogARB(programHandle, 1024, &length, errors);
        THROW(InternalError, "Error linking shaders to programable object: " << errors);
    }
}

ShaderGLSL::~ShaderGLSL() {
    unload();
}

void ShaderGLSL::setVertex(GLhandleARB program) {
    _vertexShader = program;
}

void ShaderGLSL::setFragment(GLhandleARB program) {
    _fragmentShader = program;
}

void ShaderGLSL::setProgram(GLhandleARB program) {
    _programHandle = program;
}

bool ShaderGLSL::hasVertexShader() {
    return _vertexShader != 0;
}

bool ShaderGLSL::hasFragmentShader() {
    return _fragmentShader != 0;
}

void ShaderGLSL::unload() {
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

void ShaderGLSL::on() {
    glUseProgramObjectARB(_programHandle);
}

void ShaderGLSL::off() {
    glUseProgramObjectARB(0);
}


int ShaderGLSL::getVariable(const std::string &var) const {
    int id = -1;
    if(_programHandle) {
        id = glGetUniformLocationARB(_programHandle, var.c_str());
    }

    if (id == -1) {
        Error("Error getting shader variable, ID == -1");
    }

    return id;
}

void ShaderGLSL::setTexture(const std::string &strVariable, int newValue) {
    setInt(strVariable, newValue);
}

void ShaderGLSL::setInt(const std::string &name, int newValue) {
    glUniform1iARB(getVariable(name), newValue);
}

void ShaderGLSL::setIntArray(const std::string &name, int size, int *newValues) {
    glUniform1ivARB(getVariable(name), size, newValues);
}

void ShaderGLSL::setFloat(const std::string &name, float newValue) {
    glUniform1fARB(getVariable(name), newValue);
}

void ShaderGLSL::setFloatArray(const std::string &name, int size, float *newValues) {
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
