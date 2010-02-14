/*
 *  ModelLoaderMD5.h
 *  Engine
 *
 *  Created by Brent Wilson on 8/23/06.
 *  Copyright 2006 Brent Wilson. All rights reserved.
 *
 */


#ifndef _MODELLOADERMD5_H_
#define _MODELLOADERMD5_H_

#include "Base.h"
#include "ModelAnimated.h"

#include "Model.h"

class ModelLoaderMD5 {
public:
    static bool CanLoad(const std::string &name);
    static Model* Load(const std::string &name);

};

#endif
