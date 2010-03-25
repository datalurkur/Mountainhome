/*
 *  ShaderCg.h
 *  Mountainhome
 *
 *  Created by loch on 3/22/10.
 *  Copyright 2010 Mountainhome Project. All rights reserved.
 *
 */

#ifndef _SHADERCG_H_
#define _SHADERCG_H_
#include <Base/PTreeResourceFactory.h>
#include "Shader.h"

class ShaderCg : public Shader {
public:
    class Factory : public PTreeResourceFactory<Shader> {
    public:
        Factory(ResourceGroupManager *manager);
        virtual ~Factory();

        bool canLoad(const std::string &args);
        Shader* load(const std::string &args);
    };

protected:
    ShaderCg(const std::string &vertex, const std::string &fragment);
    virtual ~ShaderCg();

public:
    //Functions    
    virtual void on() {}
    virtual void off() {}
    
    //Shader Accessors
    virtual void setTexture(const std::string &strVariable, int newValue) {}

    virtual void setInt(const std::string &strVariable, int newValue) {}
    virtual void setIntArray(const std::string &strVariable, int size, int *newValue) {}

    virtual void setFloat(const std::string &strVariable, float newValue) {}
    virtual void setFloatArray(const std::string &strVariable, int size, float *newValue) {}

};

#endif
