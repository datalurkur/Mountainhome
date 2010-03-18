/*
 *  ModelFactoryMD5.h
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

#include "ModelAnimated.h"
#include "Model.h"

class ModelFactoryMD5 : public ResourceFactory<Model> {
public:
    ModelFactoryMD5() {}
    virtual ~ModelFactoryMD5() {}
    bool canLoad(const std::string &name);
    Model* load(const std::string &name);

};

#endif
