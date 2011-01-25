/*
 *  ModelMD5.h
 *  Engine
 *
 *  Created by Brent Wilson on 8/23/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */


#ifndef _MODELFACTORYMD5_H_
#define _MODELFACTORYMD5_H_
#include <Base/ResourceManager.h>
#include <Base/Base.h>

#include "Model.h"

class ModelMD5 : public Model {
public:
    class Factory : public ResourceFactory<Model> {
    public:
        Factory();
        virtual ~Factory();
        bool canLoad(const std::string &args);
        Model* load(const std::string &args);
    };

protected:
    ModelMD5() {}
    virtual ~ModelMD5() {}

};

#endif
