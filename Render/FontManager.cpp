/*
 *  FontManager.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "ShaderManager.h"
#include "FontManager.h"
#include "FontTTF.h"

FontManager::FontManager(ResourceGroupManager *rManager, ShaderManager *sManager) {
    registerFactory(new FontTTF::Factory(rManager, sManager));
}

FontManager::~FontManager() {}
