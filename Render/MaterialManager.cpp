/*
 *  MaterialManager.cpp
 *  System
 *
 *  Created by loch on 2/16/10.
 *  Copyright 2010 Brent Wilson. All rights reserved.
 *
 */

#include "MaterialManager.h"

MaterialManager::MaterialManager(ResourceGroupManager *rManager, ShaderManager *sManager, TextureManager *tManager) {
    registerFactory(new Material::Factory(rManager, sManager, tManager));
}

MaterialManager::~MaterialManager() {}
