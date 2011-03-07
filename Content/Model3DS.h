/*
 *  Model3DS.h
 *  Engine
 *
 *  Created by Brent Wilson on 10/15/05.
 *  Copyright 2005 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MODELFACTORY3DS_H_
#define _MODELFACTORY3DS_H_
#include "ResourceManager.h"
#include "Model.h"

class Model3DS : public Model {
public:
    class Factory : public ResourceFactory<Model> {
    public:
        Factory();
        virtual ~Factory();
        bool canLoad(const std::string &args);
        Model* load(const std::string &args);
    };

protected:
    Model3DS() {}
    virtual ~Model3DS() {}

};

#endif
