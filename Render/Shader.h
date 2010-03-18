/*
 *  Shader.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _SHADER_H
#define _SHADER_H
#include "GL_Helper.h"

/*! \brief Allows the user to attach/detach shader objects to/from the rendering pipline.
    \todo Move GL_Helper.h to the cpp file. */
class Shader {
private:
    friend class ShaderManager;
    template <typename Resource, typename Id> friend class ResourceManager;
    
    Shader();
    virtual ~Shader();
    
    void setVertex(GLhandleARB program);
    void setFragment(GLhandleARB program);
    void setProgram(GLhandleARB program);
    void unload();    

public:
    static Shader* Load(const std::string &vertFilename, const std::string &fragFilename);
    
    //Functions    
    void on();
    void off();
    bool hasVertexShader();
    bool hasFragmentShader();
    
    //Shader Accessors
    void setTexture(const std::string &strVariable, GLint newValue);

    void setInt(const std::string &strVariable, GLint newValue);
    void setIntArray(const std::string &strVariable, GLint size, GLint *newValue);

    void setFloat(const std::string &strVariable, float newValue);
    void setFloatArray(const std::string &strVariable, GLint size, float *newValue);

private:
    GLhandleARB _vertexShader;
    GLhandleARB _fragmentShader;
    GLhandleARB _programHandle;
    
    int getVariable(const std::string &strVariable) const;
};

#endif
