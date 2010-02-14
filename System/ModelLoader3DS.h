/*
 *  ModelLoader3DS.h
 *  Engine
 *
 *  Created by Brent Wilson on 10/15/05.
 *  Copyright 2005 Brent Wilson. All rights reserved.
 *
 */

#ifndef _MODELLOADER3DS_H_
#define _MODELLOADER3DS_H_

#include "Model.h"

class ModelLoader3DS {
public:
    static bool CanLoad(const std::string &name);
    static Model* Load(const std::string &name);

};

#endif
