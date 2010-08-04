/*
 *  ShaderGLSL.h
 *  Mountainhome
 *
 *  Created by loch on 3/21/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _SHADERGLSL_H_
#define _SHADERGLSL_H_
#include <Base/PTreeResourceFactory.h>
#include "GL_Helper.h"
#include "Shader.h"

class ShaderGLSL : public Shader {
public:
    class Factory : public PTreeResourceFactory<Shader> {
    public:
        Factory(ResourceGroupManager *manager);
        virtual ~Factory();

        bool canLoad(const std::string &args);
        Shader* load(const std::string &args);
    };

protected:
    friend class ShaderManager;
    template <typename Resource> friend class ResourceManager;
    
    ShaderGLSL(const std::string &vertexString, const std::string &geomString, const std::string &fragmentString);
    virtual ~ShaderGLSL();

    GLuint compile(const std::string &programString, GLenum type);
    void unload();

public:
    //Functions    
    void on();
    void off();
    bool hasVertexShader();
    bool hasGeometryShader();
    bool hasFragmentShader();

    //Shader Accessors
    void setTexture(const std::string &strVariable, int newValue);

    void setInt(const std::string &strVariable, int newValue);
    void setIntArray(const std::string &strVariable, int size, int *newValue);

    void setFloat(const std::string &strVariable, float newValue);
    void setFloatArray(const std::string &strVariable, int size, float *newValue);

private:
    GLuint _vertexShader;
    GLuint _geometryShader;
    GLuint _fragmentShader;
    GLuint _programHandle;
    
    int getVariable(const std::string &strVariable) const;

};

#endif
