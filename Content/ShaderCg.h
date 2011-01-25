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
#include "PTreeResourceFactory.h"
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
    ShaderCg(const std::string &vertex, const std::string &geometry, const std::string &fragment) {}
    virtual ~ShaderCg() {}

public:
    virtual void enable() {}
    virtual void disable() {}
    virtual void bindAttributeToChannel(const std::string &name, int channel) {}
    virtual void setParameter(const std::string &strVariable, ShaderParameter *param) {}

};

#endif
