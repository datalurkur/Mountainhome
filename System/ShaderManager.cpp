/*
 *  ShaderManager.cpp
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include <FileSystem.h>
#include <TextStream.h>

#include "ShaderManager.h"
#include "Shader.h"
#include "File.h"

ShaderManager::ShaderManager() {}
ShaderManager::~ShaderManager() {}

Shader* ShaderManager::loadResource(const string &vertex, const string &fragment) {
    string mapName = vertex + fragment;
    Shader *shader = getCachedResource(mapName);
    if (shader) {
        return shader;
    }

    return loadFromFile(mapName, vertex, fragment);
}

Shader* ShaderManager::loadFromFile(const string &name, const string &vertFilename, const string &fragFilename) {
    string vertString, fragString;

    if (vertFilename.length()) {
        File *f = FileSystem::GetFile(vertFilename);
        TextStream(f).readAll(vertString);
        if (!vertString.length()) {
            Error("Vertex file was empty:" << f->fullName());
        }
    }
    
    if (fragFilename.length()) {
        File *f = FileSystem::GetFile(fragFilename);
        TextStream(f).readAll(fragString);
        if (!fragString.length()) {
            Error("Fragment file was empty:" << f->fullName());
        }
    }

    return loadFromText(name, vertString, fragString);
}

Shader* ShaderManager::loadFromText(const string &name, const string &vertString, const string &fragString) {
    bool customVertShader = vertString.length() > 0;
    bool customFragShader = fragString.length() > 0;
    GLhandleARB programHandle = NULL;
    GLhandleARB fragmentShader = NULL;
    GLhandleARB vertexShader = NULL;
    GLint compiled, linked;
    GLcharARB errors[1024];
    GLsizei length;

    Shader *shader = new Shader();

    if (customVertShader) {
        const char *vertChars = vertString.c_str();
        vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        shader->setVertex(vertexShader);

        glShaderSourceARB(vertexShader, 1, &vertChars, NULL);
        glCompileShaderARB(vertexShader);
        glGetObjectParameterivARB(vertexShader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
        if (!compiled) {
            glGetInfoLogARB(vertexShader, 1024, &length, errors);
            Error("Could not compile vertex shader.");
            Error(errors);
            
            delete shader;
            return NULL;
        }
    }

    if (customFragShader) {
        const char *fragChars = fragString.c_str();
        fragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
        shader->setFragment(fragmentShader);

        glShaderSourceARB(fragmentShader, 1, &fragChars, NULL);
        glCompileShaderARB(fragmentShader);
        glGetObjectParameterivARB(fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
        if (!compiled) {
            glGetInfoLogARB(fragmentShader, 1024, &length, errors);
            Error("Could not compile fragment shader.");
            Error(errors);

            delete shader;
            return NULL;
        }
    }

    programHandle = glCreateProgramObjectARB();
    shader->setProgram(programHandle);

    if (customVertShader) { glAttachObjectARB(programHandle, vertexShader); }
    if (customFragShader) { glAttachObjectARB(programHandle, fragmentShader); }
    glLinkProgramARB(programHandle);
    
    glGetObjectParameterivARB(programHandle, GL_OBJECT_LINK_STATUS_ARB, &linked);
    if (!linked) {
        glGetInfoLogARB(programHandle, 1024, &length, errors);
        Error("Error linking shaders to programable object");
        Error(errors);

        delete shader;
        return NULL;
    }

    registerResource(name, shader);
    return shader;
}
