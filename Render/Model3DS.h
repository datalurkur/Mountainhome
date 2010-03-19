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
#include <Base/ResourceManager.h>
#include "Model.h"

class Model3DS : public Model {
public:
    static void Setup();
    static void Teardown();
    static bool CanLoad(const std::string &name);
    static Model* Load(const std::string &name);

protected:
    Model3DS() {}
    virtual ~Model3DS() {}

};

#endif
