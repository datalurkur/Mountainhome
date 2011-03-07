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

ShaderManager::ShaderManager(ResourceGroupManager *manager) {
    registerFactory(new ShaderGLSL::Factory(manager));
    registerFactory(new ShaderCg::Factory(manager));
}

ShaderManager::~ShaderManager() {}
