/*
 *  FontManager.cpp
 *  Fight In The Shade
 *
 *  Created by Brent Wilson on 4/22/07.
 *  Copyright 2007 __MyCompanyName__. All rights reserved.
 *
 */

#include "FontManager.h"
#include "SDL_Helper.h"
#include <sstream>

FontManager::FontManager() {
    if (TTF_Init() == -1) {
        Error("Could not init TTF:" << TTF_GetError());
    }
}

FontManager::~FontManager() {
    TTF_Quit();
}

Font* FontManager::loadResource(const string &fontName, int size) {
    std::stringstream ss;
    ss << fontName << size;

    Font *font = getCachedResource(ss.str());
    if (font) { return font; }

    return loadFromFile(ss.str(), fontName, size);
}

Font* FontManager::loadFromFile(const string &mapName, const string &fontName, int size) {
    Font *font = new Font(fontName, size);
    registerResource(mapName, font);
    return font;
}

