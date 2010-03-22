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
#include <string>

/*! \brief Allows the user to attach/detach shader objects to/from the rendering pipline. */
class Shader {
public:
    //Functions    
    virtual void on() = 0;
    virtual void off() = 0;
    
    //Shader Accessors
    virtual void setTexture(const std::string &strVariable, int newValue) = 0;

    virtual void setInt(const std::string &strVariable, int newValue) = 0;
    virtual void setIntArray(const std::string &strVariable, int size, int *newValue) = 0;

    virtual void setFloat(const std::string &strVariable, float newValue) = 0;
    virtual void setFloatArray(const std::string &strVariable, int size, float *newValue) = 0;

};

#endif
