/*
 *  ShaderGLSL.cpp
 *  Mountainhome
 *
 *  Created by loch on 3/21/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#include <Base/FileSystem.h>
#include <Base/TextStream.h>
#include "ShaderParameter.h"
#include "ShaderGLSL.h"

ShaderGLSL::Factory::Factory(ResourceGroupManager *manager): PTreeResourceFactory<Shader>(manager) {
    addRequiredKey("vertex");
    // addRequiredKey("geometry");
    addRequiredKey("fragment");
}

ShaderGLSL::Factory::~Factory() {}

bool ShaderGLSL::Factory::canLoad(const std::string &args) {
    return true;
}

Shader* ShaderGLSL::Factory::load(const std::string &args) {
    std::string vert, vertPath = getPathFromKey("vertex");
    if (vertPath.length() > 0) { TextStream(FileSystem::GetFile(vertPath)).readAll(vert); }
    std::string geom, geomPath = getPathFromKey("geometry");
    if (geomPath.length() > 0) { TextStream(FileSystem::GetFile(geomPath)).readAll(geom); }
    std::string frag, fragPath = getPathFromKey("fragment");
    if (fragPath.length() > 0) { TextStream(FileSystem::GetFile(fragPath)).readAll(frag); }
    return new ShaderGLSL(vert, geom, frag);
}

ShaderGLSL::ShaderGLSL(
    const std::string &vertString,
    const std::string &geomString,
    const std::string &fragString
):
    _vertString(vertString),
    _geomString(geomString),
    _fragString(fragString)
{
    bool customVertShader = vertString.length() > 0;
    bool customGeomShader = geomString.length() > 0;
    bool customFragShader = fragString.length() > 0;

    // Compile the shaders
    if (customVertShader) {
        _vertexShader = compile(vertString, GL_VERTEX_SHADER);
    }
    if (customGeomShader) {
        _geometryShader = compile(geomString, GL_GEOMETRY_SHADER_EXT);
    }
    if (customFragShader) {
        _fragmentShader = compile(fragString, GL_FRAGMENT_SHADER);
    }

    // Create the program and attach the shaders.
    _programHandle = glCreateProgram();

    if (customVertShader) { glAttachShader(_programHandle, _vertexShader);   }
    if (customGeomShader) { glAttachShader(_programHandle, _geometryShader); }
    if (customFragShader) { glAttachShader(_programHandle, _fragmentShader); }

    // This really shouldn't be hardcoded...
    glProgramParameteriEXT(_programHandle, GL_GEOMETRY_VERTICES_OUT_EXT, 3);
    glProgramParameteriEXT(_programHandle, GL_GEOMETRY_INPUT_TYPE_EXT,   GL_TRIANGLES);
    glProgramParameteriEXT(_programHandle, GL_GEOMETRY_OUTPUT_TYPE_EXT,  GL_TRIANGLE_STRIP);

    // Link and display errors.
    glLinkProgram(_programHandle);

    GLint linked;
    glGetProgramiv(_programHandle, GL_LINK_STATUS, &linked);

    if (!linked) {
        GLint length;
        glGetProgramiv(_programHandle, GL_INFO_LOG_LENGTH, &length);

        GLchar errors[length];
        glGetProgramInfoLog(_programHandle, length, &length, errors);
        THROW(InternalError, "Error linking shaders to programable object: " << errors);
    }

}

ShaderGLSL::~ShaderGLSL() {
    unload();
}

GLuint ShaderGLSL::compile(const std::string &programString, GLenum type) {

    const char *chars = programString.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &chars, NULL);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        std::string strType;
        switch (type) {
            case GL_VERTEX_SHADER:   strType = "vertex"; break;
            case GL_GEOMETRY_SHADER_EXT: strType = "geometry"; break;
            case GL_FRAGMENT_SHADER: strType = "fragment"; break;
        }

        GLint length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        GLchar errors[length];
        glGetShaderInfoLog(shader, length, &length, errors);
        THROW(InternalError, "Error compiling " << strType << " shader:\n" << errors << "\nSource:\n" << programString);
    }

    return shader;
}

bool ShaderGLSL::hasVertexShader() {
    return _vertexShader != 0;
}

bool ShaderGLSL::hasGeometryShader() {
    return _geometryShader != 0;
}

bool ShaderGLSL::hasFragmentShader() {
    return _fragmentShader != 0;
}

void ShaderGLSL::unload() {
    if(_vertexShader) {
        glDetachShader(_programHandle, _vertexShader);
        glDeleteShader(_vertexShader); _vertexShader = NULL;
    }

    if(_geometryShader) {
        glDetachShader(_programHandle, _geometryShader);
        glDeleteShader(_geometryShader); _geometryShader = NULL;
    }
    
    if(_fragmentShader) {
        glDetachShader(_programHandle, _fragmentShader);
        glDeleteShader(_fragmentShader); _fragmentShader = NULL;
    }
    
    if(_programHandle) {
        glDeleteProgram(_programHandle); _programHandle = NULL;
    }
}

void ShaderGLSL::enable() {
    glUseProgram(_programHandle);

    // Loop over bound textures and enable them on the correct channel.
    TextureChannelBindingMap::iterator itr;
    for (itr = _textures.begin(); itr != _textures.end(); itr++) {
        if (itr->second.texture) {
            itr->second.texture->enable(itr->second.channel);
        }
    }
}

void ShaderGLSL::disable() {
    glUseProgram(0);
}

void ShaderGLSL::bindAttributeToChannel(const std::string &name, int channel) {
    glBindAttribLocation(_programHandle, channel, name.c_str());
}

void ShaderGLSL::setParameter(const std::string &strVariable, ShaderParameter *param) {
    int paramID = getVariable(strVariable);
    switch(param->getType()) {
    case FloatParam:
        switch(param->getSize()) {
            case 1: glUniform1fv(paramID, param->getCount(), param->getData<GLfloat>()); break;
            case 2: glUniform2fv(paramID, param->getCount(), param->getData<GLfloat>()); break;
            case 3: glUniform3fv(paramID, param->getCount(), param->getData<GLfloat>()); break;
            case 4: glUniform4fv(paramID, param->getCount(), param->getData<GLfloat>()); break;
            default: THROW(InternalError, "Invalid case.");
        }
        break;
    case IntParam: break;
        switch(param->getSize()) {
            case 1: glUniform1iv(paramID, param->getCount(), param->getData<GLint>()); break;
            case 2: glUniform2iv(paramID, param->getCount(), param->getData<GLint>()); break;
            case 3: glUniform3iv(paramID, param->getCount(), param->getData<GLint>()); break;
            case 4: glUniform4iv(paramID, param->getCount(), param->getData<GLint>()); break;
            default: THROW(InternalError, "Invalid case.");
        }
        break;
    case Matrix4Param:
        // TODO: This is only valid if Real is a float. If Real is a double, this will crash and burn. Need a CT assert or an MPL if statement.
        glUniformMatrix4fv(paramID, param->getCount(), false, (param->getData<Matrix>())->getMatrix());
        break;
    case TextureParam: {
        // Store a reference to the relevant map entry.
        TextureChannelBinding &binding = _textures[paramID];

        // Update the channel and texture.
        binding.texture = param->getData<Texture>();
        if (binding.channel == -1) { // -1 indicates a recently constructed object.
            binding.channel = _textures.size() - 1;
        }

        // Bind the sampler to the correct channel.
        glUniform1i(paramID, binding.channel);

        break;
    }
    default:
        THROW(InternalError, "Invalid case.");
    }
}

GLint ShaderGLSL::getVariable(const std::string &var) const {
    GLint id = -1;
    if(_programHandle) {
        id = glGetUniformLocation(_programHandle, var.c_str());
    }

    if (id == -1) {
        THROW(InternalError, "Error getting shader variable '" << var << "': ID == -1");
    }

    return id;
}
