/*
 *  FontManager.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 Brent Wilson. All rights reserved.
 *
 */

#include <Base/FileSystem.h>


#include "FontManager.h"
#include "SDL_Helper.h"
#include <sstream>

class FontFactorySDL : public ResourceFactory<Font, FontArgsList> {
public:
    FontFactorySDL() {
        if (TTF_Init() == -1) {
            THROW(InternalError, "Could not init TTF: " << TTF_GetError());
        }
    }

    virtual ~FontFactorySDL() {
        TTF_Quit();
    }

    bool canLoad(const FontArgsList &args) {
        std::string ext;
        FileSystem::ExtractExtension(args.name, ext);
        return ext == "ttf";    
    }

    Font* load(const FontArgsList &args) {
        Font *font = new Font(args.name, args.size);
        return font;
    }
};

FontManager::FontManager() {
    registerFactory(new FontFactorySDL());
}

FontManager::~FontManager() {}
