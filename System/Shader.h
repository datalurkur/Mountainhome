/*
 *  Shader.h
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SHADER_H
#define _SHADER_H
#include "GL_Helper.h"

/*! \brief Allows the user to attach/detach shader objects to/from the rendering pipline.
    \todo Remove GL_Helper.h
    \author Brent Wilson
    \date 4/22/07 */
class Shader {
private:
    friend class ShaderManager;
    template <class T> friend class ResourceManager;
    
    Shader();
    ~Shader();
    
    void setVertex(GLhandleARB program);
    void setFragment(GLhandleARB program);
    void setProgram(GLhandleARB program);
    void unload();    

public:
    static Shader* Load(const string &vertFilename, const string &fragFilename);
    
    //Functions    
    void on();
    void off();
    bool hasVertexShader();
    bool hasFragmentShader();
    
    //Shader Accessors
    void setTexture(const string &strVariable, GLint newValue);

    void setInt(const string &strVariable, GLint newValue);
    void setIntArray(const string &strVariable, GLint size, GLint *newValue);

    void setFloat(const string &strVariable, float newValue);
    void setFloatArray(const string &strVariable, GLint size, float *newValue);

private:
    GLhandleARB _vertexShader;
    GLhandleARB _fragmentShader;
    GLhandleARB _programHandle;
    
    int getVariable(const string &strVariable) const;
};

#endif
