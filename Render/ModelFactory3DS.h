/*
 *  ModelFactory3DS.h
 *  Engine
 *
 *  Created by Brent Wilson on 10/15/05.
 *  Copyright 2005 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MODELFACTORY3DS_H_
#define _MODELFACTORY3DS_H_
#include <Base/ResourceManager.h>
#include "Model.h"

class ModelFactory3DS : public ResourceFactory<Model> {
public:
    ModelFactory3DS() {}
    virtual ~ModelFactory3DS() {}
    bool canLoad(const std::string &name);
    Model* load(const std::string &name);

};

#endif
