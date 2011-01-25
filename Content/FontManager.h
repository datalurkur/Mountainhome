/*
 *  FontManager.h
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#ifndef _FONTMANAGER_H_
#define _FONTMANAGER_H_
#include "ResourceManager.h"
#include "MaterialManager.h"
#include "Font.h"

class ShaderManager;

/*! \brief Provides the functionality for Font caching and Loading. */
class FontManager : public ResourceManager<Font> {
public:
    FontManager(ResourceGroupManager *rManager, MaterialManager *mManager, TextureManager *tManager);
    virtual ~FontManager();

};

#endif
