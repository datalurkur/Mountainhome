/*
 *  ShaderManager.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "ShaderManager.h"
#include "ShaderGLSL.h"
#include "ShaderCg.h"

ShaderManager::ShaderManager() {
    registerFactory(new ShaderGLSL::Factory());
    registerFactory(new ShaderCg::Factory());
}

ShaderManager::~ShaderManager() {}
