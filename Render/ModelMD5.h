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

#include "ModelAnimated.h"
#include "Model.h"

class ModelMD5 : public Model {
public:
    static void Setup();
    static void Teardown();
    static bool CanLoad(const std::string &name);
    static Model* Load(const std::string &name);

protected:
    ModelMD5() {}
    virtual ~ModelMD5() {}

};

#endif
