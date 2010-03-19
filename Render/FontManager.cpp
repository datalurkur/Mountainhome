/*
 *  FontManager.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include "FontManager.h"
#include "FontTTF.h"

FontManager::FontManager() {
    registerConcreteResourceType<FontTTF>();
}

FontManager::~FontManager() {}
